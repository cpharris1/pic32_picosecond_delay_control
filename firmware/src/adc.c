#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <string.h>
#include "definitions.h"                // SYS function prototypes
#include "uart_helper.h"
#include "adc.h"

uint16_t adc_arr[4];
float input_voltage;
volatile bool result_ready = false;
#define adc_ASIZE 20 //array size
#define adc_PERIOD 1.5 //seconds, dont go below 1.5s
float thr_v[adc_ASIZE],five_v[adc_ASIZE],twelve_v[adc_ASIZE],ftune[adc_ASIZE];
volatile uint32_t ADCi = 0;
volatile uint32_t adc_halfSec = 0;
volatile uint8_t adc_full=0;

void ADC_ResultHandler(uintptr_t context)
{
    /* Read the ADC result */
    adc_arr[0] = ADC_ResultGet(ADC_RESULT_BUFFER_0);   //AN2 -- 12V
    adc_arr[1] = ADC_ResultGet(ADC_RESULT_BUFFER_1);   //AN9 -- 3.3V
    adc_arr[2] = ADC_ResultGet(ADC_RESULT_BUFFER_2);   //AN15 -- 5V
    adc_arr[3] = ADC_ResultGet(ADC_RESULT_BUFFER_3);   //AN19 -- FTUNE
    result_ready = true;
    
    //char str_ADChist[100];
    if(adc_halfSec == adc_PERIOD*2){ //Can't be less than 3 adc_halfSec, 1 second is too fast
        if(ADCi<adc_ASIZE){
            
            
            thr_v[ADCi]=(float)adc_arr[1] * ADC_VREF / ADC_MAX_COUNT;
//            sprintf(str_ADChist, "ADC 3.3V Input Voltage = %f V \n\r",thr_v[ADCi]);
//            UARTprint(str_ADChist);
            five_v[ADCi]=((float)adc_arr[2] * ADC_VREF / ADC_MAX_COUNT)*(4.964/2.988);
            twelve_v[ADCi]=((float)adc_arr[0] * ADC_VREF / ADC_MAX_COUNT)*(4.096/1.093);
            ftune[ADCi]=(float)adc_arr[3] * (ADC_VREF / ADC_MAX_COUNT);
            
            ADCi++;
            if(ADCi==(adc_ASIZE-1) && !adc_full){
                adc_full=1;
            }
        }
        if(ADCi==adc_ASIZE){
            ADCi=0;
        }
        adc_halfSec = 0;
    }
    adc_halfSec++;
}

void get_ADC(void){
    if(result_ready == true)
    {
        char str_adc[100];
        //TODO: make this into a function.. and use an array to store ADC counts
        result_ready = false;
        input_voltage = (float)adc_arr[0] * ADC_VREF / ADC_MAX_COUNT;
        sprintf(str_adc, "RB0(AN2) 12V ADC Count = 0x%03x, ADC Input Voltage = %d.%d V \n\r", adc_arr[0], (int)input_voltage, (int)((input_voltage - (int)input_voltage)*100.0));
        UARTprint(str_adc);
        input_voltage = (float)adc_arr[1] * ADC_VREF / ADC_MAX_COUNT;
        sprintf(str_adc, "RB14(AN9) 3.3V ADC Count = 0x%03x, ADC Input Voltage = %d.%d V \n\r", adc_arr[1], (int)input_voltage, (int)((input_voltage - (int)input_voltage)*100.0));
        UARTprint(str_adc);
        input_voltage = (float)adc_arr[2] * ADC_VREF / ADC_MAX_COUNT;
        sprintf(str_adc, "RC5(AN15) 5V ADC Count = 0x%03x, ADC Input Voltage = %d.%d V \n\r", adc_arr[2], (int)input_voltage, (int)((input_voltage - (int)input_voltage)*100.0));
        UARTprint(str_adc);
        input_voltage = (float)adc_arr[3] * ADC_VREF / ADC_MAX_COUNT;
        sprintf(str_adc, "RA6(AN19) FTUNE ADC Count = 0x%03x, ADC Input Voltage = %d.%d V \n\r", adc_arr[3], (int)input_voltage, (int)((input_voltage - (int)input_voltage)*100.0));
        UARTprint(str_adc);
    }
    else{
        UARTprint("ERROR: ADC result not ready.\n\r");
    }
}