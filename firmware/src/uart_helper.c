#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <string.h>
#include "definitions.h"                // SYS function prototypes
#include "uart_helper.h"


void UART_PutC(char data){
    while(U3STAbits.UTXBF);
    U3TXREG = data;
    return;
}

int UARTprint(char *str)
{
    static int i;
    i=0;
    while(str[i])
    {
        UART_PutC(str[i]);
        i++;
    }
    return(i);
}

void printMenu(void){
    UARTprint("PIC32 UART Menu\n\r\n\r");
    UARTprint("1. Enable/Disable Heartbeat LED\n\r");
    UARTprint("2. Show Data from ADC\n\r");
    UARTprint("3. Option 3\n\r");
    UARTprint("\n\rPlease input selection: ");
}

void clearScreen(void){
    UARTprint("\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r");
    UARTprint("\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r");
}

void printWaitReturn(void){
    UARTprint("\n\rPlease press ENTER to continue.");
}
