#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <string.h>
#include "definitions.h"                // SYS function prototypes
#include "uart_helper.h"
#include "dac.h"
#include "adc.h"
#include "dht11.h"
#include "delay.h"

// States for FSM
uint8_t state;
enum {
     IDLE = 0,
     PRINT_MENU,
     AWAIT_INPUT,
     PROCESS_INPUT,
     WAIT_RETURN,
};

volatile uint8_t dht11_flag=0;
volatile uint8_t adc_flag=0;
volatile uint8_t heartbeat_en=1;

// Timer2 interrupt for heartbeat LED, and timing DHT11 and ADC sampling
void TIMER2_InterruptSvcRoutine(uint32_t status, uintptr_t context)
{
    if(heartbeat_en) LED1_Toggle();
    else LED1_Off();
    dht11_flag=1;
    adc_flag=1;
}
    
/*
    Returns whether the menu option is a valid choice
    @param opt menu option character
    @returns 1 if valid, 0 if invalid
*/
bool validOption(char opt){
    return ((opt >= '1' && opt <= '6') || (opt >= 'a' && opt <= 'e'));
}


// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

float three_voltage;
float five_voltage;
float twelve_voltage;
float ftune_voltage;

int main ( void )
{
    // Initialize all modules
    SYS_Initialize ( NULL );
    
    // Start timer 2 for heartbeat LED, DHT11, and ADC sampling
    TMR2_CallbackRegister(TIMER2_InterruptSvcRoutine, (uintptr_t)NULL);
    TMR2_Start();
    
    // Register the ADC callback
    ADC_CallbackRegister(ADC_ResultHandler, (uintptr_t)NULL);
    //ADC_ConversionStart(); Main should not poll ADCs every time...
    
    // Register the I2C callback and configure SDA and SCL pins
    I2C3_CallbackRegister(MCP4725_I2CCallback, (uintptr_t)NULL );
    TRISBbits.TRISB7 = 1;
    TRISBbits.TRISB13 = 1;
    
    // Initialize FSM
    char c='0';
    char menuSelect = '0';
    state = PRINT_MENU;
    char str[100];
    uint32_t printData =0;
    uint32_t printADC =0;

    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
        GPIO_PinSet(GPIO_PIN_RD0);
        
        // If DHT11 flag is high collect DHT11 data
        if(dht11_flag){
            contData();
            dht11_flag=0;
        }
        // If ADC flag is high collect ADC data
        if(adc_flag){
            ADC_ConversionStart();
            adc_flag=0;
        }
        
        // Finite state machine
        switch(state){
            case IDLE:
                break;
            case PRINT_MENU:
                clearScreen();
                printMenu();
                lock_delay_chips(); // we don't want to accidentally change delay config
                state = AWAIT_INPUT;
                break;
            case AWAIT_INPUT:
                if (IFS1bits.U3RXIF)    //If we have received a char,
                {
                    c=U3RXREG;          //read it
                    U3TXREG=c;          //echo it back
                    // See if the character is a valid menu option, if it is save it
                    if(validOption(c)) menuSelect = c;

                    // if ENTER key was pressed (sends \r\n or \n depending on system) process the menu option
                    if(c=='\r' || c=='\n')
                    {
                        UARTprint("\n\r");
                        state = PROCESS_INPUT;
                    }
                    IFS1bits.U3RXIF=0;
                }
                break;
            case PROCESS_INPUT:
                UARTprint("\n\r");
                switch(menuSelect){
                    case '1':  // Set ns delay
                        UARTprint("Input nanosecond delay (0-200ns): ");
                        // Read in the nanosecond delay as string from the UART
                        char ns[10];
                        getStr(ns, 10);
                        // Make sure it's a valid number, convert string to int, then set the ns delay
                        if(isValidDecimal(ns)){
                            set_ns_delay(atoi(ns));
                        }
                        else{
                            UARTprint("Not a valid number. Please enter valid number between 0-200\n\r");
                        }
                        // Wait for user to press ENTER
                        printWaitReturn();
                        state = WAIT_RETURN;
                        break;
                    case '2': // Set ps delay
                        UARTprint("Input picosecond delay (0-999ps): ");
                        // Read in the picosecond delay as string from the UART
                        char ps[10];
                        getStr(ps, 10);
                        // Make sure it's a valid number, convert string to int, then set the ps delay
                        if(isValidDecimal(ps)){
                            set_ps_delay(atoi(ps));  
                        }
                        else{
                            UARTprint("Not a valid number. Please enter value between 0-999\n\r");
                        }
                        // Wait for user to press ENTER
                        printWaitReturn();
                        state = WAIT_RETURN;
                        break;
                    case '3': // Set full delay in form NNN.PPP nanoseconds
                        UARTprint("Input total delay in ns with up to 3 decimal places (0-200.000ns): ");
                        // Read in full delay as string from the UART
                        char fd[10];
                        getStr(fd, 10);
                        // Set the full delay
                        set_full_delay(fd);
                        // Wait for user to press ENTER
                        printWaitReturn();
                        state = WAIT_RETURN;
                        break;
                    case '4': // Get current temperature/humidity data
                     
                        StartSignal();
                        CheckResponse();
                        if(Check == 1){
                            RH_byte1 = ReadData();
                            RH_byte2 = ReadData();
                            T_byte1 = ReadData();
                            T_byte2 = ReadData();
                            Sum = ReadData();
                            if(Sum == ((RH_byte1+RH_byte2+T_byte1+T_byte2) & 0XFF)){
                                RH = RH_byte1;
                                RH_dec = RH_byte2;
                                Temp = T_byte1;
                                Temp_dec = T_byte2;
                            }
                        }
                        sprintf(str, "Temperature is %d.%d degrees C \n\r", Temp,Temp_dec);
                        UARTprint(str);
                        sprintf(str, "Humidity is %d.%d %%RH \n\r", RH, RH_dec);
                        UARTprint(str);
                        
                        printWaitReturn();
                        state = WAIT_RETURN;
                        break;
                    case '5': // Get temperature/humidity data history
                        UARTprint("Temp/Humid Data History\n\r");
                        UARTprint("i,C,RH\n\r");
                        
                        if(dataCount==(ASIZE-1) && !flag_full){
                            flag_full=1;
                            sprintf(str, "Array is full, flag_full = %d\n\r",flag_full);
                            UARTprint(str);
                        }
                        if(flag_full==1){
                            //sprintf(str, "Back end printing, flag_full = %d\n\r",flag_full);
                            //UARTprint(str);
                            for(printData=dataCount;printData<ASIZE;printData++){
                                sprintf(str, "%d,%d.%d,%d.%d\n\r", printData,T[printData],Tdec[printData],RHa[printData],RHdec[printData]);
                                UARTprint(str);
                            }
                        }
                        for(printData=0;printData<dataCount;printData++){
                            sprintf(str, "%d,%d.%d,%d.%d\n\r", printData,T[printData],Tdec[printData],RHa[printData],RHdec[printData]);
                            UARTprint(str);
                        }
                        printWaitReturn();
                        state = WAIT_RETURN;
                        break;
                    case '6':
                        UARTprint("Current ADC Data\n\r");
                        //get_ADC(); 
                        UARTprint("\n\rADC Data History\n\r");
                        UARTprint("Samp,3.3V,5V,12V,FTUNE\n\r");
                        
                        if(ADCi==(adc_ASIZE-1) && !adc_full){
                            adc_full=1;
                            sprintf(str, "Array is full, adc_full = %d\n\r",adc_full);
                            UARTprint(str);
                        }
                        if(adc_full==1){
                            for(printADC=ADCi;printADC<adc_ASIZE;printADC++){
                                sprintf(str, "%d,%.3f,%.3f,%.3f,%.3f\n\r", printADC,thr_v[printADC],five_v[printADC],twelve_v[printADC],ftune[printADC]);
                                UARTprint(str);    
                            }
                        }
                        for(printADC=0;printADC<ADCi;printADC++){//this is the beginning of the array
                            sprintf(str, "%d,%.3f,%.3f,%.3f,%.3f\n\r", printADC,thr_v[printADC],five_v[printADC],twelve_v[printADC],ftune[printADC]);
                                UARTprint(str); 
                        }
                        printWaitReturn();
                        state = WAIT_RETURN;
                        break;
                    // Alphabetical cases are for debug/demo purposes...
                    case 'a': // Turn the heartbeat LED on or off
                        if(heartbeat_en){
                            UARTprint("Heartbeat LED disabled\n\r");
                            heartbeat_en = 0;
                        }
                        else{
                            UARTprint("Heartbeat LED enabled\n\r");
                            heartbeat_en = 1;
                        }
                        printWaitReturn();
                        state = WAIT_RETURN;
                        break;
                    case 'b': // Manually input the voltage to write to the DAC
                        // Read in the voltage as a string from the UART
                        UARTprint("Input voltage to write to DAC: ");
                        char dac[10];
                        getStr(dac, 10);
                        // Write the voltage to the DAC
                        write_voltage_DAC(dac);
                        // Wait for user to press ENTER
                        printWaitReturn();
                        state = WAIT_RETURN;
                        break;
                    case 'c': // Demo to sweep ns delay chip from 0-200ns to verify GPIO output
                        UARTprint("Sweeping 0-200ns\n\r");
                        // Loop through 0-200ns in 1ns increments
                        for(int i=0; i<=200; i++){
                            char ns[10];
                            sprintf(ns, "%d", i);
                            set_ns_delay(atoi(ns));
                            CORETIMER_DelayMs(100);
                        }                      
                        printWaitReturn();
                        state = WAIT_RETURN;
                        break;
                    case 'd': // Demo to sweep coarse ps delay from 0-990ps to verify GPIO output
                        UARTprint("Sweeping 0-999ps\n\r");
                        // Loop through 0-990ns in 10ps increments
                        for(int i=0; i<=999; i+=10){
                            char ps[10];
                            sprintf(ps, "%d", i);
                            set_ps_delay(atoi(ps));
                            CORETIMER_DelayMs(100);
                        }   
                        
                        printWaitReturn();
                        state = WAIT_RETURN;
                        break;
                    case 'e': // Demo to sweep FTUNE voltage range to verify DAC output
                        writeDAC(1860);
                        CORETIMER_DelayMs(2000);
                        // Loop through 1.6V to 2.0V and st the DAC every 800ms
                        for(uint16_t val = 1860; val < 2482; val++){
                            writeDAC(val);
                            CORETIMER_DelayMs(800);
                        }
                        printWaitReturn();
                        state = WAIT_RETURN;
                        break;
                    default:
                        UARTprint("Invalid option selected\n\r");
                        printWaitReturn();
                        state = WAIT_RETURN;
                        break;
                }
                break;
            case WAIT_RETURN: // Wait for ENTER to be pressed to start new menu option
                if (IFS1bits.U3RXIF)    //If we have received a char,
                {
                    c=U3RXREG;          //read it
                    U3TXREG=c;          //echo it back
                    // If ENTER was pressed, go back to top and accept new menu ption
                    if(c=='\r' || c=='\n')
                    {
                        UARTprint("\n\r");
                        state = PRINT_MENU;
                    }
                    IFS1bits.U3RXIF=0;
                }
                menuSelect = '0';
                break;
            default:
                UARTprint("Invalid option selected\n\r");
                printWaitReturn();
                state = WAIT_RETURN;
        }                    
       
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

