#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <string.h>
#include "definitions.h"                // SYS function prototypes
#include <stdint.h>
#include "uart_helper.h"
#include "dac.h"

uint8_t MCP4725TxData[MCP4725_TX_DATA_LENGTH];
volatile uint8_t I2C3TxStatus = 0;



/*
 *  Write a 12-bit code to the DAC over I2C
 *  @param val The new DAC code (12-bits unsigned int). If value is greater than 12-bits, clamps to max value 4095
 *  @return 1 if success, 0 if failure
 */
uint8_t writeDAC(uint16_t val){     
    // Clamp value to 4095 if greater one is passed in
    uint16_t cnt = val;
    if (val > 4095) cnt = 4095;

    // Data packet  
    MCP4725TxData[0] = MCP4725_WRITE_DAC;   // command (write)
    MCP4725TxData[1] = (cnt & 0xff00) >> 8; // MSB of write value 
    MCP4725TxData[2] = (cnt & 0x00ff);      // LSB of write value

    // wait for the current transfer to complete
    while(I2C3_IsBusy());

    // Return 1 if success, 0 if failure
    return I2C3_Write(MCP4725_ADDR, &MCP4725TxData[0], MCP4725_TX_DATA_LENGTH);
    return 0;
}

/*
 *  Write a valid voltage (0-3.3V) to the DAC 
 *  @param dac The voltage as a string
 *  @return void
 */
void write_voltage_DAC(char *dac_voltage){
    char str1[100];
    // Check if the string is a valid number ie X.XX
    if(isValidDecimal(dac_voltage)){
        // Convert the string to a float
        float dac_float = atof(dac_voltage);
        // Calculate the integer DAC code to realize the given voltage
        uint16_t dac_val = dac_float * 4095 / 3.3;
        float actual_dac = 3.3 * dac_val / 4095;
        
        if(dac_float > 3.3){
            // Print an error if the voltage is greater than 3.3 (out of range)
            sprintf(str1,"%fV is greater than the max value of 3.3V\n\r", dac_float);
            UARTprint(str1);
        }
        else if(dac_float < 0){
            // Print an error if the voltage is less than 0 (out of range)
            sprintf(str1,"%fV is a negative number, cannot write to DAC\n\r", dac_float);
            UARTprint(str1);
        }
        else{
            // Write the integer DAC code to the DAC over I2C
            if(!writeDAC(dac_val)){
                UARTprint("Error occurred while writing to DAC\n\r");
            }
            else{
                sprintf(str1,"wrote %.4fV (%d) to DAC\n\r", actual_dac, dac_val);
                UARTprint(str1);
            }
        }
    }
    else{
        UARTprint("Value inputted is not a valid decimal\n\r");
    }
}

void MCP4725_I2CCallback(uintptr_t context )
{
    if(I2C3_ErrorGet() == I2C_ERROR_NONE)
    {
        I2C3TxStatus = 1;
    }
    else
    {
        I2C3TxStatus = 0;
    }
}