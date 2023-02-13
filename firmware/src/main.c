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

volatile uint8_t heartbeat_en=1;
void TIMER1_InterruptSvcRoutine(uint32_t status, uintptr_t context)
{
    if(heartbeat_en) LED1_Toggle();
    else LED1_Off();
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
                        get_ADC();
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
                                Temp = T_byte1;
                            }
                        }
                        sprintf(str, "Temperature is %d degrees C \n\r", Temp);
                        UARTprint(str);
                        sprintf(str, "Humidity is %d %%RH \n\r", RH);
                        UARTprint(str);
                        
                        printWaitReturn();
                        state = WAIT_RETURN;
                        break;
                    case '5':
                        UARTprint("Placeholder for DHT11 History\n\r");
                        
                        printWaitReturn();
                        state = WAIT_RETURN;
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

