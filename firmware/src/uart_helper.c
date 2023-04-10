#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <string.h>
#include "definitions.h"                // SYS function prototypes
#include "uart_helper.h"
#include <ctype.h>
#include "delay.h"

/*
 *  Put a character into the UART buffer to transmit
 *  @param data character to write to UART
 *  @return void
 */
void UART_PutC(char data){
    while(U3STAbits.UTXBF);
    U3TXREG = data;
    return;
}

/*
 *  Write a string to the UART by putting each character in the buffer
 *  @param str The string you want to print
 *  @return Number of characters written
 */
int UARTprint(char *str)
{
    static int i;
    i=0;
    // Loop through the length of the string, and put each character in the buffer
    while(str[i])
    {
        UART_PutC(str[i]);
        i++;
    }
    return(i);
}

/*
 *  Print out the menu for the user interface
 */
void printMenu(void){
    UARTprint("PIC32 Picosecond Delay Control (v1.4)\n\r\n\r");
    print_delay();
    UARTprint("\n\r");
    
    UARTprint("Menu Options:\n\r");
    UARTprint("1. Set nanosecond delay (0-200ns)\n\r");
    UARTprint("2. Set picosecond delay (0-999ps)\n\r");
    UARTprint("3. Set total delay (0-200.000ns)\n\r");
    UARTprint("4. Show current temperature and humidity data\n\r");
    UARTprint("5. Show temp and humidity history\n\r");
    UARTprint("6. Show power and FTUNE voltage history\n\r");
    UARTprint("\n\rDemo and Debug Features:\n\r");
    UARTprint("a. Enable/Disable Heartbeat LED\n\r");
    UARTprint("b. Manually set FTUNE voltage\n\r");
//    UARTprint("c. Sweep nanosecond delay (0-200)\n\r");
//    UARTprint("d. Sweep picosecond delay (0-999)\n\r");
//    UARTprint("e. Sweep FTUNE DAC voltages(1.5V-2.0V)\n\r");;
    UARTprint("\n\rPlease input selection: ");
}

/*
 *  Print out sufficient newlines to "clear" the putty terminal
 */
void clearScreen(void){
    UARTprint("\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r");
    UARTprint("\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r");
}

/*
 *  Print the prompt to wait for enter to be pressed
 */
void printWaitReturn(void){
    UARTprint("\n\rPlease press ENTER to continue.");
}

/*
 *  Read in a string of user input from the UART
 *  @param string The destination to store the string
 *  @param size The size of the destination buffer (max number of characters to read)
 *  @return nothing
 */
void getStr(char* string, int size){
    char x;
    int i=0;
    while(1){
        if (IFS1bits.U3RXIF)        //If we have received a char,
        {
            x=U3RXREG;              //read it
            U3TXREG=x;              //echo it back
            if(x=='\n' || x=='\r')  //If that char was newline
            {
                UARTprint("\n\r");  //Echo the newline
                string[i] = '\0';   //Terminate the string
                IFS1bits.U3RXIF=0;
                break;
            }
            else{
                if(x > 33){         //If valid character, append it to the string pre-33 codes are control char
                    string[i++] = x;
                }
            }
            IFS1bits.U3RXIF=0;
        }
        if(i > size){               // If the num of characters exceeds buffer size, terminate the string
            UARTprint("\n\r");
            string[i] = '\0';
            break;
        }
    }
    return;
}

/*
 *  Determine whether a string is a valid number (only numbers and max one decimal place or negative sign)
 *  @param str The number as a string
 *  @param size The size of the destination buffer (max number of characters to read)
 *  @return 1 if valid, 0 if not valid
 */
uint8_t isValidDecimal(char* str){
    uint8_t decimalCnt=0;
    // Loop through entire string, not valid if a char is encountered that is not a digit,
    // decimal point, newline, or negative sign
    for(int i=0; i<strlen(str); i++){
        if(!isdigit(str[i]) && (str[i] != '.'&& str[i] != '\n' && str[i] != '\r'&& str[i] != '-')){
            return 0;
        }
        if(str[i]=='.') decimalCnt++;
    }
    if(decimalCnt>1) return 0;
    return 1;
}

/*
 *  Determine whether a string is an integer number or float (decimal)
 *  @param str The number as a string
 *  @return 1 if float, 0 if integer
 */
uint8_t hasDecimal(char* str){
    for(int i=0; i<strlen(str); i++){
        if(str[i] == '.') return 1;
    }
    return 0;
}