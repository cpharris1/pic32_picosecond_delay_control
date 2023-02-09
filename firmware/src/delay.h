/* 
 * File:   delay.h
 * Author: chloe
 *
 * Created on February 8, 2023, 6:40 PM
 */

#ifndef DELAY_H
#define	DELAY_H

extern uint16_t dn[8];
extern uint16_t dcp[11];
extern uint32_t dn_names[8];
extern uint32_t dcp_names[11];

void set_ns_delay(char *ns);
void set_ps_delay(char *ps);

#endif	/* DELAY_H */

