/* 
 * File:   delay.h
 * Author: chloe
 *
 * Created on February 8, 2023, 6:40 PM
 */

#ifndef DELAY_H
#define	DELAY_H

extern uint16_t dn[8];
extern uint16_t dcp[10];
extern uint32_t Dn_pin_names[10];

void set_ns_delay(char *ns);
void set_ps_delay(char *ps);
void set_full_delay(char *fd);
void lock_delay_chips(void);

void lock_ns(void);
void unlock_ns(void);
uint8_t ns_is_locked(void);

void lock_ps(void);
void unlock_ps(void);
uint8_t ps_is_locked(void);

void print_delay(void);
uint8_t valid_ps(char *ps);
uint8_t valid_ns(char *ns);

#endif	/* DELAY_H */

