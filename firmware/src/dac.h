/* 
 * File:   dac.h
 * Author: chloe
 *
 * Created on November 7, 2022, 3:59 PM
 */

#ifndef DAC_H
#define	DAC_H

#define MCP4725_ADDR                        0x60 //7 bit addr, A0, A1 to GND
#define MCP4725_WRITE_DAC                   0x00 // Write DAC cmd 00 to addr 0x00
#define MCP4725_TX_DATA_LENGTH              3    // addr, cmd, high, low
#define APP_ACK_DATA_LENGTH                 1

uint8_t writeDAC(uint16_t val);
void write_voltage_DAC(char *dac_voltage);
void MCP4725_I2CCallback(uintptr_t context );

#endif	/* DAC_H */

