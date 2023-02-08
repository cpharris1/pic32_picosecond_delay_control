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

uint8_t state;
enum {
     IDLE = 0,
     PRINT_MENU,
     AWAIT_INPUT,
     PROCESS_INPUT,
     WAIT_RETURN,
};

volatile uint8_t heartbeat_en=1;
void TIMER1_InterruptSvcRoutine(uint32_t status, uintptr_t context)
{
    if(heartbeat_en) LED1_Toggle();
    else LED1_Off();
}

bool validOption(char opt){
    return opt >= '1' && opt <= '6';
}

//Below is for the Temp/Humid Sensor (DHT11)


// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

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
    int i=0;

    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
        ADC_ConversionStart();
        GPIO_PinSet(GPIO_PIN_RD0);
        
        switch(state){
            case IDLE:
                break;
            case PRINT_MENU:
                clearScreen();
                printMenu();
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
                    case '1':
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
                    case '2':
                        /* Wait till ADC conversion result is available */
                        if(result_ready == true)
                        {
                            //TODO: make this into a function.. and use an array to store ADC counts
                            result_ready = false;
                            input_voltage = (float)adc_count * ADC_VREF / ADC_MAX_COUNT;
                            sprintf(str, "RA2(AN5) ADC Count = 0x%03x, ADC Input Voltage = %d.%d V \n\r", adc_count, (int)input_voltage, (int)((input_voltage - (int)input_voltage)*100.0));
                            UARTprint(str);
                            input_voltage = (float)adc_count2 * ADC_VREF / ADC_MAX_COUNT;
                            sprintf(str, "RA3(AN6) ADC Count = 0x%03x, ADC Input Voltage = %d.%d V \n\r", adc_count2, (int)input_voltage, (int)((input_voltage - (int)input_voltage)*100.0));
                            UARTprint(str);
                            input_voltage = (float)adc_count3 * ADC_VREF / ADC_MAX_COUNT;
                            sprintf(str, "POT(AN14) ADC Count = 0x%03x, ADC Input Voltage = %d.%d V \n\r", adc_count3, (int)input_voltage, (int)((input_voltage - (int)input_voltage)*100.0));
                            UARTprint(str);
                        }
                        else{
                            UARTprint("ERROR: ADC result not ready.\n\r");
                        }
                        printWaitReturn();
                        state = WAIT_RETURN;
                        break;
                    case '3':
                        UARTprint("Input voltage to write to DAC: ");
                        char dac[10];
                        getStr(dac, 10);
                        
                        if(isValidDecimal(dac)){
                            float dac_float = atof(dac);
                            uint16_t dac_val = dac_float * 4095 / 3.3;
                            if(dac_float > 3.3){
                                sprintf(str,"%fV is greater than the max value of 3.3V\n\r", dac_float);
                                UARTprint(str);
                            }
                            else if(dac_float < 0){
                                sprintf(str,"%fV is a negative number, cannot write to DAC\n\r", dac_float);
                                UARTprint(str);
                            }
                            else{
                                if(!writeDAC(dac_val)){
                                    UARTprint("Error occurred while writing to DAC\n\r");
                                }
                                else{
                                    sprintf(str,"Successfully wrote %fV to DAC\n\r", dac_float);
                                    UARTprint(str);
                                }
                            }
                        }
                        else{
                            UARTprint("Value inputted is not a valid decimal\n\r");
                        }
                        
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
                    case 'z':
                        writeDAC(1860);
                        CORETIMER_DelayMs(2000);
                        for(uint16_t val = 1860; val < 2482; val++){
                            writeDAC(val);
                            CORETIMER_DelayMs(800);
                        }
                        printWaitReturn();
                        state = WAIT_RETURN;
                        break;
                    case '6':
                        /*run temp/humidity test*/
                        UARTprint("Option 6 selected, temp test \n\r");
                        i = 0;
                        uint8_t Sum2=0;
                        while(i<5){
                            StartSignal();
                            CheckResponse();
                            if(Check == 1){
                                RH_byte1 = ReadData();
                                RH_byte2 = ReadData();
                                T_byte1 = ReadData();
                                T_byte2 = ReadData();
                                Sum = ReadData();
                                Sum2 = ((RH_byte1+RH_byte2+T_byte1+T_byte2) & 0XFF);
                                if(Sum == Sum2){
                                    RHa[i] = RH_byte1;
                                    RHdec[i] = RH_byte2;
                                    T[i] = T_byte1;
                                    Tdec[i] = T_byte2;
                                    sprintf(str,"hit \n\r");
                                    UARTprint(str);
                                } 
                            }
                            sprintf(str, "ARRAY: Temperature is %d.%d degrees C \n\r", T[i],Tdec[i]);
                            UARTprint(str);
                            
                            sprintf(str, "ARRAY: Humidity is %d.%d %%RH \n\r", RHa[i], RHdec[i]);
                            UARTprint(str);
                            
                            CORETIMER_DelayMs(1200); //don't go below 1.2 s
                            i++;
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

