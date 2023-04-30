
#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <string.h>
#include "definitions.h"                // SYS function prototypes
#include "uart_helper.h"

unsigned char Check, T_byte1, T_byte2, RH_byte1, RH_byte2, Ch ;
uint8_t Temp, RH, RH_dec, Temp_dec, Sum ;
#define ASIZE 300 //array size
#define PERIOD 60 //seconds, dont go below 1.5s, it messes with the delays for the DHT11
uint8_t T[ASIZE],Tdec[ASIZE],RHa[ASIZE],RHdec[ASIZE];
volatile uint32_t dataCount = 0;
volatile uint32_t halfSec = 0;
volatile uint8_t flag_full=0;

/*
 * Microprocessor sends a start signal (low - 18ms) to the DHT11
 * asking for the temp/humidity data
 */
void StartSignal(){
    GPIO_PinOutputEnable(GPIO_PIN_RD0);
    GPIO_PinClear(GPIO_PIN_RD0);
    CORETIMER_DelayMs(18);
    GPIO_PinSet(GPIO_PIN_RD0);
    CORETIMER_DelayUs(30);
    GPIO_PinInputEnable(GPIO_PIN_RD0);
}
  /*
  * Microprocessor sends an 80us low signal to check if DHT11 is
  * ready to send data.
  * DHT11 will send 80us high when ready to receive data.
  */
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
   
 /*
  * Reads the temperature and humidity data from the DHT11
  */
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

/*
 * Reads DHT11 data and stores into an array for later use. 
 * Integer and decimal values are separate bytes so they are
 * stored in separate arrays.
 */
void contData(){
  uint8_t Sum2=0;
    if(halfSec == PERIOD*2){ 
        if(dataCount<ASIZE){
            LED2_Toggle();
            /* 
            * Sequence for getting DHT11 Data:
            *  Step 1: Send start signal - StartSignal()
            *  Step 2: Check if DHT11 is ready to send data - CheckResponse()
            *  Step 3: Read data from DHT11 and store to respective 
            *          variables - See lines 81-92
            */
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
            dataCount++;
            if(dataCount==(ASIZE-1) && !flag_full){ //checks if array is full
                flag_full=1;
            }
        }
        if(dataCount==ASIZE){//starts over at the beginning of the array when array is full
            dataCount=0;
        }
        halfSec = 0;
    }
    halfSec++;
}
