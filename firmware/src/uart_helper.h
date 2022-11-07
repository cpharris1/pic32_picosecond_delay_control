/* 
 * File:   uart_helper.h
 * Author: chloe
 *
 * Created on October 17, 2022, 10:21 PM
 */

#ifndef UART_HELPER_H
#define	UART_HELPER_H

void UART_PutC(char data);
int UARTprint(char *str);
void printMenu(void);
void clearScreen(void);
void printWaitReturn(void);
void getStr(char* string, int size);

#endif	/* UART_HELPER_H */

