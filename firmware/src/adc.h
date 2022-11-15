/* 
 * File:   adc.h
 * Author: chloe
 *
 * Created on November 7, 2022, 4:12 PM
 */

#ifndef ADC_H
#define	ADC_H


#define ADC_VREF                (3.3f)
#define ADC_MAX_COUNT           (1023U)

extern uint16_t adc_count, adc_count2, adc_count3;
extern float input_voltage;
extern volatile bool result_ready;

void ADC_ResultHandler(uintptr_t context);

#endif	/* ADC_H */

