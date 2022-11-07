#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <string.h>
#include "definitions.h"                // SYS function prototypes
#include "uart_helper.h"
#include "adc.h"

uint16_t adc_count, adc_count2, adc_count3;
float input_voltage;
volatile bool result_ready = false;

void ADC_ResultHandler(uintptr_t context)
{
    /* Read the ADC result */
    adc_count = ADC_ResultGet(ADC_RESULT_BUFFER_0);   
    adc_count2 = ADC_ResultGet(ADC_RESULT_BUFFER_1); 
    adc_count3 = ADC_ResultGet(ADC_RESULT_BUFFER_2); 
    result_ready = true;
}