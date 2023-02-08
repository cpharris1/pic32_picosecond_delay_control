/* 
 * File:   dht11.h
 * Author: chloe
 *
 * Created on November 7, 2022, 4:22 PM
 */

#ifndef DHT11_H
#define	DHT11_H

extern unsigned char Check, T_byte1, T_byte2, RH_byte1, RH_byte2, Ch ;
extern uint8_t Temp, RH, RH_dec, Temp_dec, Sum ;
#define ASIZE 300
extern uint8_t T[ASIZE],Tdec[ASIZE],RHa[ASIZE],RHdec[ASIZE];
extern volatile uint32_t dataCount;
extern volatile uint32_t halfSec;
extern volatile uint32_t dataCompCount;
extern volatile uint8_t flag_full;

void StartSignal();
void CheckResponse();
char ReadData();
void contData();

#endif	/* DHT11_H */

