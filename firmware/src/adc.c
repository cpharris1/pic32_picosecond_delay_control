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

void ADC_ResultHandler(uintptr_t context)
{
    /* Read the ADC result */
    adc_arr[0] = ADC_ResultGet(ADC_RESULT_BUFFER_0);   //AN2
    adc_arr[1] = ADC_ResultGet(ADC_RESULT_BUFFER_1);   //AN9
    adc_arr[2] = ADC_ResultGet(ADC_RESULT_BUFFER_2);   //AN15
    adc_arr[3] = ADC_ResultGet(ADC_RESULT_BUFFER_3);   //AN19
    result_ready = true;
}

void get_ADC(void){
    if(result_ready == true)
    {
        char str_adc[100];
        //TODO: make this into a function.. and use an array to store ADC counts
        result_ready = false;
        input_voltage = (float)adc_arr[0] * ADC_VREF / ADC_MAX_COUNT;
        sprintf(str_adc, "RB0(AN2) 3.3V ADC Count = 0x%03x, ADC Input Voltage = %d.%d V \n\r", adc_arr[0], (int)input_voltage, (int)((input_voltage - (int)input_voltage)*100.0));
        UARTprint(str_adc);
        input_voltage = (float)adc_arr[1] * ADC_VREF / ADC_MAX_COUNT;
        sprintf(str_adc, "RB14(AN9) 5V ADC Count = 0x%03x, ADC Input Voltage = %d.%d V \n\r", adc_arr[1], (int)input_voltage, (int)((input_voltage - (int)input_voltage)*100.0));
        UARTprint(str_adc);
        input_voltage = (float)adc_arr[2] * ADC_VREF / ADC_MAX_COUNT;
        sprintf(str_adc, "RC5(AN15) 12V ADC Count = 0x%03x, ADC Input Voltage = %d.%d V \n\r", adc_arr[2], (int)input_voltage, (int)((input_voltage - (int)input_voltage)*100.0));
        UARTprint(str_adc);
        input_voltage = (float)adc_arr[3] * ADC_VREF / ADC_MAX_COUNT;
        sprintf(str_adc, "RA6(AN19) FTUNE ADC Count = 0x%03x, ADC Input Voltage = %d.%d V \n\r", adc_arr[3], (int)input_voltage, (int)((input_voltage - (int)input_voltage)*100.0));
        UARTprint(str_adc);
    }
    else{
        UARTprint("ERROR: ADC result not ready.\n\r");
    }
}