
#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <string.h>
#include "definitions.h"                // SYS function prototypes
#include "uart_helper.h"

unsigned char Check, T_byte1, T_byte2, RH_byte1, RH_byte2, Ch ;
unsigned Temp, RH, Sum ;

void StartSignal(){
    GPIO_PinOutputEnable(GPIO_PIN_RD0);
    GPIO_PinClear(GPIO_PIN_RD0);
    CORETIMER_DelayMs(18);
    GPIO_PinSet(GPIO_PIN_RD0);
    CORETIMER_DelayUs(30);
    GPIO_PinInputEnable(GPIO_PIN_RD0);
}
 //////////////////////////////
 void CheckResponse(){
    Check = 0;
    CORETIMER_DelayUs(40);
    if (GPIO_PinRead(GPIO_PIN_RD0) == 0){
       CORETIMER_DelayUs(80);
        if (GPIO_PinRead(GPIO_PIN_RD0) == 1) {
            Check = 1; 
           CORETIMER_DelayUs(40);
        }
    }
 }
   //////////////////////////////
char ReadData(){
    char i=0, j;
    for(j = 0; j < 8; j++){
        while(!GPIO_PinRead(GPIO_PIN_RD0)); //Wait until PORTD.F0 goes HIGH
        CORETIMER_DelayUs(30);
        if(GPIO_PinRead(GPIO_PIN_RD0) == 0) i&= ~(1<<(7 - j)); //Clear bit (7-b)
        else {i|= (1 << (7 - j)); //Set bit (7-b)
        while(GPIO_PinRead(GPIO_PIN_RD0));} //Wait until PORTD.F0 goes LOW
    }
    return i;
 }
