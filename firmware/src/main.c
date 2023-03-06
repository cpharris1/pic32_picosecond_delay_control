/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

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
void TIMER1_InterruptSvcRoutine(uint32_t status, uintptr_t context)
{
    if(heartbeat_en) LED1_Toggle();
    else LED1_Off();
    dht11_flag=1;
    adc_flag=1;
}
    

bool validOption(char opt){
    return ((opt >= '1' && opt <= '7') || (opt >= 'w' && opt <= 'z'));
}

//Below is for the Temp/Humid Sensor (DHT11)


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
    
    // Start timer 1 for heartbeat LED
    TMR1_CallbackRegister(TIMER1_InterruptSvcRoutine, (uintptr_t)NULL);
    TMR1_Start();
    
    ADC_CallbackRegister(ADC_ResultHandler, (uintptr_t)NULL);
    ADC_ConversionStart();
    
    
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
        
        if(dht11_flag){
            contData();
            dht11_flag=0;
        }
        
        if(adc_flag){
            ADC_ConversionStart();
            adc_flag=0;
        }
        
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
                    if(validOption(c)) menuSelect = c;
                    if(c=='\r' || c=='\n')          //If that char was "A" then send a bunch of long stuff.
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
                    case '6':
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
                    case '7':
                        UARTprint("Current ADC Data\n\r");
                        //get_ADC(); 
                        UARTprint("\n\rADC Data History\n\r");
                        UARTprint("Samp 3.3V 5V 12V FTUNE\n\r");
                        
                        if(ADCi==(adc_ASIZE-1) && !adc_full){
                            adc_full=1;
                            sprintf(str, "Array is full, adc_full = %d\n\r",adc_full);
                            UARTprint(str);
                        }
                        if(adc_full==1){
                            for(printADC=ADCi;printADC<adc_ASIZE;printADC++){
                                sprintf(str, "%d %.3f %.3f %.3f %.3f\n\r", printADC,thr_v[printADC],five_v[printADC],twelve_v[printADC],ftune[printADC]);
                                UARTprint(str);    
                            }
                        }
                        for(printADC=0;printADC<ADCi;printADC++){//this is the beginning of the array
                            sprintf(str, "%d %.3f %.3f %.3f %.3f\n\r", printADC,thr_v[printADC],five_v[printADC],twelve_v[printADC],ftune[printADC]);
                                UARTprint(str); 
                        }
                        printWaitReturn();
                        state = WAIT_RETURN;
                        break;
                    case 'w':
                        UARTprint("Input voltage to write to DAC: ");
                        char dac[10];
                        getStr(dac, 10);
                        
                        write_voltage_DAC(dac);
                        
                        printWaitReturn();
                        state = WAIT_RETURN;
                        break;
                    case '4':
                     
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
                    case '1':
                        UARTprint("Input nanosecond delay (0-200ns): ");
                        char ns[10];
                        getStr(ns, 10);
                        set_ns_delay(ns);
                        
                        printWaitReturn();
                        state = WAIT_RETURN;
                        break;
                    case '2':
                        UARTprint("Input picosecond delay (0-999ps): ");
                        char ps[10];
                        getStr(ps, 10);
                        set_ps_delay(ps);
                        
                        printWaitReturn();
                        state = WAIT_RETURN;
                        break;
                    case '3':
                        UARTprint("Input total delay in nanoseconds with up to 3 decimal places for picosecond delay (0-200.000ns): ");
                        char fd[10];
                        getStr(fd, 10);
                        
                        set_full_delay(fd);
                        
                        printWaitReturn();
                        state = WAIT_RETURN;
                        break;
                    case 'x':
                        UARTprint("Sweeping 0-200ns\n\r");
                        for(int i=0; i<=200; i++){
                            char ns[10];
                            sprintf(ns, "%d", i);
                            set_ns_delay(ns);
                            CORETIMER_DelayMs(100);
                        }                      
                        printWaitReturn();
                        state = WAIT_RETURN;
                        break;
                    case 'y':
                        UARTprint("Sweeping 0-999ps\n\r");
                        for(int i=0; i<=999; i+=10){
                            char ps[10];
                            sprintf(ps, "%d", i);
                            set_ps_delay(ps);
                            CORETIMER_DelayMs(100);
                        }   
                        
                        printWaitReturn();
                        state = WAIT_RETURN;
                        break;
                    case 'z':
                        writeDAC(1860);
                        // configure MM to sample every 800ms
                        CORETIMER_DelayMs(2000);
                        for(uint16_t val = 1860; val < 2482; val++){
                            writeDAC(val);
                            CORETIMER_DelayMs(800);
                        }
                        printWaitReturn();
                        state = WAIT_RETURN;
                        break;
                    case '5':
                        UARTprint("Temp/Humid Data History\n\r");
                        UARTprint("i C RH\n\r");
                        
                        if(dataCount==(ASIZE-1) && !flag_full){
                            flag_full=1;
                            sprintf(str, "Array is full, flag_full = %d\n\r",flag_full);
                            UARTprint(str);
                        }
                        if(flag_full==1){
                            //sprintf(str, "Back end printing, flag_full = %d\n\r",flag_full);
                            //UARTprint(str);
                            for(printData=dataCount;printData<ASIZE;printData++){
                                sprintf(str, "%d %d.%d %d.%d\n\r", printData,T[printData],Tdec[printData],RHa[printData],RHdec[printData]);
                                UARTprint(str);
                            }
                        }
                        for(printData=0;printData<dataCount;printData++){
                            sprintf(str, "%d %d.%d %d.%d\n\r", printData,T[printData],Tdec[printData],RHa[printData],RHdec[printData]);
                            UARTprint(str);
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
            case WAIT_RETURN:
                if (IFS1bits.U3RXIF)    //If we have received a char,
                {
                    c=U3RXREG;          //read it
                    U3TXREG=c;          //echo it back
                    if(c=='\r' || c=='\n')          //If that char was "A" then send a bunch of long stuff.
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

