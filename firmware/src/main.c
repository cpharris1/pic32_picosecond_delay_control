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
#define ADC_VREF                (3.3f)
#define ADC_MAX_COUNT           (1023U)

volatile uint8_t heartbeat_en=1;
uint16_t adc_count, adc_count2, adc_count3;
float input_voltage;
volatile bool result_ready = false;

void TIMER1_InterruptSvcRoutine(uint32_t status, uintptr_t context)
{
    if(heartbeat_en) LED1_Toggle();
    else LED1_Off();
}

void ADC_ResultHandler(uintptr_t context)
{
    /* Read the ADC result */
    adc_count = ADC_ResultGet(ADC_RESULT_BUFFER_0);   
    adc_count2 = ADC_ResultGet(ADC_RESULT_BUFFER_1); 
    adc_count3 = ADC_ResultGet(ADC_RESULT_BUFFER_2); 
    result_ready = true;
}

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

uint8_t state;

enum {
     IDLE = 0,
     PRINT_MENU,
     AWAIT_INPUT,
     PROCESS_INPUT,
     WAIT_RETURN
};

int main ( void )
{
    // Initialize all modules
    SYS_Initialize ( NULL );
    
    // Start timer 1 for heartbeat LED
    TMR1_CallbackRegister(TIMER1_InterruptSvcRoutine, (uintptr_t)NULL);
    TMR1_Start();
    
    ADC_CallbackRegister(ADC_ResultHandler, (uintptr_t)NULL);
    ADC_ConversionStart();
    
    // Initialize FSM
    char c='0';
    char menuOpt = '0';
    state = PRINT_MENU;

    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
        ADC_ConversionStart();
        
        switch(state){
            case IDLE:
                break;
            case PRINT_MENU:
                clearScreen();
                printMenu();
                state = AWAIT_INPUT;
                break;
            case AWAIT_INPUT:
                //UARTprint("in AWAIT\n\r");
                if (IFS1bits.U3RXIF)    //If we have received a char,
                {
                    c=U3RXREG;          //read it
                    U3TXREG=c;          //echo it back
                    if(c=='\n' || c=='\r')          //If that char was "A" then send a bunch of long stuff.
                    {
                        UARTprint("\n\r");
                        state = PROCESS_INPUT;
                    }
                    else{
                        menuOpt = c;
                    }
                    IFS1bits.U3RXIF=0;
                }
                break;
            case PROCESS_INPUT:
                UARTprint("\n\r");
                switch(menuOpt){
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
                            char str[100];
                            result_ready = false;
                            input_voltage = (float)adc_count * ADC_VREF / ADC_MAX_COUNT;
                            sprintf(str, "RB1(AN3) ADC Count = 0x%03x, ADC Input Voltage = %d.%d V \n\r", adc_count, (int)input_voltage, (int)((input_voltage - (int)input_voltage)*100.0));
                            UARTprint(str);
                            input_voltage = (float)adc_count2 * ADC_VREF / ADC_MAX_COUNT;
                            sprintf(str, "RB2(AN4) ADC Count = 0x%03x, ADC Input Voltage = %d.%d V \n\r", adc_count2, (int)input_voltage, (int)((input_voltage - (int)input_voltage)*100.0));
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
                        UARTprint("Option 3 selected\n\r");
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
                    if(c=='\n' || c=='\r')          //If that char was "A" then send a bunch of long stuff.
                    {
                        UARTprint("\n\r");
                        state = PRINT_MENU;
                    }
                    IFS1bits.U3RXIF=0;
                }
                menuOpt = '0';
                break;
        }                    
       
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

