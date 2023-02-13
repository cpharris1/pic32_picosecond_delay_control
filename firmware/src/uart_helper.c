#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <string.h>
#include "definitions.h"                // SYS function prototypes
#include "uart_helper.h"
#include <ctype.h>
#include "delay.h"

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
    UARTprint("PIC32 Picosecond Delay Control (v1.3)\n\r\n\r");
    print_delay();
    UARTprint("\n\r");
    
    UARTprint("Menu Options:\n\r");
    UARTprint("1. Set nanosecond delay (0-200ns)\n\r");
    UARTprint("2. Set picosecond delay (0-999ps)\n\r");
    UARTprint("3. Set total delay (0-200.000ns)\n\r");
    UARTprint("4. Show Current Temperature and Humidity Data\n\r");
    UARTprint("5. Show Temp and Humidity History\n\r");
    UARTprint("6. Enable/Disable Heartbeat LED\n\r");
    UARTprint("7. Show Data from ADC\n\r");
    UARTprint("\n\rDemo and Debug Features:\n\r");
    UARTprint("w. Manually set FTUNE voltage\n\r");
    UARTprint("x. Sweep nanosecond delay (0-200)\n\r");
    UARTprint("y. Sweep picosecond delay (0-999)\n\r");
    UARTprint("z. Sweep FTUNE DAC voltages(1.5V-2.0V)\n\r");;
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

uint8_t isValidDecimal(char* str){
    uint8_t decimalCnt=0;
    for(int i=0; i<strlen(str); i++){
        if(!isdigit(str[i]) && (str[i] != '.'&& str[i] != '\n' && str[i] != '\r'&& str[i] != '-')){
            return 0;
        }
        if(str[i]=='.') decimalCnt++;
    }
    if(decimalCnt>1) return 0;
    return 1;
}

uint8_t hasDecimal(char* str){
    for(int i=0; i<strlen(str); i++){
        if(str[i] == '.') return 1;
    }
    return 0;
}