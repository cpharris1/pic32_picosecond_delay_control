
#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <string.h>
#include "definitions.h"                // SYS function prototypes
#include "uart_helper.h"

unsigned char Check, T_byte1, T_byte2, RH_byte1, RH_byte2, Ch ;
uint8_t Temp, RH, RH_dec, Temp_dec, Sum ;
#define ASIZE 300
uint8_t T[ASIZE],Tdec[ASIZE],RHa[ASIZE],RHdec[ASIZE];
volatile uint32_t dataCount = 0;
volatile uint32_t halfSec = 0;
volatile uint32_t dataCompCount = 0;
volatile uint8_t flag_full=0;
//volatile uint8_t flag_timer=0;

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

void contData(){
  uint8_t Sum2=0;
    if(halfSec == 20){ //Can't be less than 3 halfSec, 1 second is too fast
        if(dataCount<ASIZE){
            LED2_Toggle();
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
                    RHa[dataCount] = RH_byte1;
                    RHdec[dataCount] = RH_byte2;
                    T[dataCount] = T_byte1;
                    Tdec[dataCount] = T_byte2;
                } 
            }
            //sprintf(strint, "%d,%d.%d,%d.%d\n\r", dataCount,T[dataCount],Tdec[dataCount],RHa[dataCount],RHdec[dataCount]);
            //UARTprint(strint);
            dataCount++;
            dataCompCount=dataCount;
            if(dataCompCount==(ASIZE-1) && !flag_full){
                flag_full=1;
                //sprintf(strint, "Array is full, flag_full = %d\n\r",flag_full);
                //UARTprint(strint);
            }
        }
        if(dataCount==ASIZE){
            dataCount=0;
        }
        halfSec = 0;
    }
    halfSec++;
}
