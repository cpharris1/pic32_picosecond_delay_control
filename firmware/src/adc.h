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

extern uint16_t adc_arr[4];
extern float input_voltage;
extern volatile bool result_ready;
#define adc_ASIZE 20 //array size
extern float thr_v[adc_ASIZE],five_v[adc_ASIZE],twelve_v[adc_ASIZE],ftune[adc_ASIZE];
extern volatile uint32_t ADCi;
extern volatile uint32_t adc_halfSec;
extern volatile uint8_t adc_full;

void ADC_ResultHandler(uintptr_t context);

void get_ADC(void);

#endif	/* ADC_H */

