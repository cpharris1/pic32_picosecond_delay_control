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
extern uint8_t T[300],Tdec[300],RHa[300],RHdec[300];

void StartSignal();
void CheckResponse();
char ReadData();

#endif	/* DHT11_H */

