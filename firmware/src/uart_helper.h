/* 
 * File:   uart_helper.h
 * Author: chloe
 *
 * Created on October 17, 2022, 10:21 PM
 */

#ifndef UART_HELPER_H
#define	UART_HELPER_H

#ifdef	__cplusplus
extern "C" {
#endif

void UART_PutC(char data);
int UARTprint(char *str);
void printMenu(void);
void clearScreen(void);



#ifdef	__cplusplus
}
#endif

#endif	/* UART_HELPER_H */

