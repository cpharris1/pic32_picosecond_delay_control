#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <string.h>
#include "definitions.h"                // SYS function prototypes
#include "uart_helper.h"
#include "dac.h"

uint8_t MCP4725TxData[MCP4725_TX_DATA_LENGTH];
volatile uint8_t I2C3TxStatus = 0;

uint8_t writeDAC(uint16_t val){        
    uint16_t cnt = val;
    if (val > 4095) cnt = 4095;
    // Data 
    MCP4725TxData[0] = MCP4725_WRITE_DAC;
    MCP4725TxData[1] = (cnt & 0xff00) >> 8;
    MCP4725TxData[2] = (cnt & 0x00ff);
    //char buffer[50];
    //sprintf(buffer, "Data bytes: %x %x\n\r", MCP4725TxData[1], MCP4725TxData[2]);
    //UARTprint(buffer);
    // wait for the current transfer to complete
    while(I2C3_IsBusy());

    // perform the next transfer
    return I2C3_Write(MCP4725_ADDR, &MCP4725TxData[0], MCP4725_TX_DATA_LENGTH);
    return 0;
}

void write_voltage_DAC(char *dac){
    char str1[100];
    if(isValidDecimal(dac)){
        float dac_float = atof(dac);
        uint16_t dac_val = dac_float * 4095 / 3.3;
        if(dac_float > 3.3){
            sprintf(str1,"%fV is greater than the max value of 3.3V\n\r", dac_float);
            UARTprint(str1);
        }
        else if(dac_float < 0){
            sprintf(str1,"%fV is a negative number, cannot write to DAC\n\r", dac_float);
            UARTprint(str1);
        }
        else{
            if(!writeDAC(dac_val)){
                UARTprint("Error occurred while writing to DAC\n\r");
            }
            else{
                sprintf(str1,"Successfully wrote %fV to DAC\n\r", dac_float);
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