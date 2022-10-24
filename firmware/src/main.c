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

volatile uint8_t heartbeat_en=1;

void TIMER1_InterruptSvcRoutine(uint32_t status, uintptr_t context)
{
    if(heartbeat_en) LED1_Toggle();
    else LED1_Off();
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
    
    // Initialize FSM
    char c='0';
    char menuOpt = '0';
    state = PRINT_MENU;

    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
        
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
                        UARTprint("Please press ENTER to continue. ");
                        state = WAIT_RETURN;
                        break;
                    case '2':
                        UARTprint("Option 2 selected\n\r");
                        UARTprint("Please press ENTER to continue. ");
                        state = WAIT_RETURN;
                        break;
                    case '3':
                        UARTprint("Option 3 selected\n\r");
                        UARTprint("Please press ENTER to continue. ");
                        state = WAIT_RETURN;
                        break;
                    default:
                        UARTprint("Invalid option selected\n\r");
                        UARTprint("Please press ENTER to continue. ");
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
                break;
        }                    
       
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

