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
    UARTprint("PIC32 Picosecond Delay Control (v1.6)\n\r\n\r");
    UARTprint("1. Enable/Disable Heartbeat LED\n\r");
    UARTprint("2. Show Data from ADC\n\r");
    UARTprint("3. Write to I2C DAC\n\r");
    UARTprint("4. Show Temperature and Humidity\n\r");
    UARTprint("5. DAC Ramp Demo\n\r");;
    UARTprint("\n\rPlease input selection: ");
}

void clearScreen(void){
    UARTprint("\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r");
    UARTprint("\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r");
}

void printWaitReturn(void){
    UARTprint("\n\rPlease press ENTER to continue.");
}

void getStr(char* string, int size){
    char x;
    int i=0;
    while(1){
        if (IFS1bits.U3RXIF)    //If we have received a char,
        {
            x=U3RXREG;          //read it
            U3TXREG=x;          //echo it back
            if(x=='\n' || x=='\r')          //If that char was newline
            {
                UARTprint("\n\r");
                string[i] = '\0';
                IFS1bits.U3RXIF=0;
                break;
            }
            else{
                if(x > 33){
                    string[i++] = x;
                }
            }
            IFS1bits.U3RXIF=0;
        }
        if(i > size){
            UARTprint("\n\r");
            string[i] = '\0';
            break;
        }
    }
    return;
}