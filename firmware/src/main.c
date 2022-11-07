/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <string.h>
#include "definitions.h"                // SYS function prototypes
#include "uart_helper.h"
#define ADC_VREF                (3.3f)
#define ADC_MAX_COUNT           (1023U)

uint8_t state;
#define INVALID = 0xff;
enum {
     IDLE = 0,
     PRINT_MENU,
     AWAIT_INPUT,
     PROCESS_INPUT,
     WAIT_RETURN,
};

volatile uint8_t heartbeat_en=1;
uint16_t adc_count, adc_count2, adc_count3;
float input_voltage;
volatile bool result_ready = false;

#define MCP4725_ADDR                        0x62 //7 bit addr, A0 to GND
#define MCP4725_WRITE_DAC                   0x40 // Write DAC cmd 010
#define MCP4725_TX_DATA_LENGTH              3    // addr, cmd, high, low
#define APP_ACK_DATA_LENGTH                 1

uint8_t MCP4725TxData[MCP4725_TX_DATA_LENGTH];


void TIMER1_InterruptSvcRoutine(uint32_t status, uintptr_t context)
{
    if(heartbeat_en) LED1_Toggle();
    else LED1_Off();
}

void ADC_ResultHandler(uintptr_t context)
{
    /* Read the ADC result */
    adc_count = ADC_ResultGet(ADC_RESULT_BUFFER_0);   
    adc_count2 = ADC_ResultGet(ADC_RESULT_BUFFER_1); 
    adc_count3 = ADC_ResultGet(ADC_RESULT_BUFFER_2); 
    result_ready = true;
}

volatile uint8_t I2C3TxStatus = 0;

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

uint8_t writeDAC(uint16_t val){        
    uint16_t cnt = val;
    if (val > 4095) cnt = 4095;
    // Data 
    MCP4725TxData[0] = MCP4725_WRITE_DAC;
    MCP4725TxData[1] = (cnt & 0x0ff0) >> 4;
    MCP4725TxData[2] = (cnt & 0xf) << 4;
    //char buffer[50];
    //sprintf(buffer, "Data bytes: %x %x\n\r", MCP4725TxData[1], MCP4725TxData[2]);
    //UARTprint(buffer);
    // wait for the current transfer to complete
    while(I2C3_IsBusy());

    // perform the next transfer
    return I2C3_Write(MCP4725_ADDR, &MCP4725TxData[0], MCP4725_TX_DATA_LENGTH);
    return 0;
}

void getStr(char* string, int size){
    char x;
    int i=0;
    while(1){
        if (IFS1bits.U3RXIF)    //If we have received a char,
        {
            x=U3RXREG;          //read it
            U3TXREG=x;          //echo it back
            if(x=='\n' || x=='\r')          //If that char was newline
            {
                UARTprint("\n\r");
                string[i] = '\0';
                IFS1bits.U3RXIF=0;
                break;
            }
            else{
                if(x > 33){
                    string[i++] = x;
                }
            }
            IFS1bits.U3RXIF=0;
        }
        if(i > size){
            UARTprint("\n\r");
            string[i] = '\0';
            break;
        }
    }
    return;
}

bool validOption(char opt){
    return opt == '1' || opt == '2' || opt =='3'|| opt =='4';
}

//Below is for the Temp/Humid Sensor (DHT11)

unsigned char Check, T_byte1, T_byte2,
 RH_byte1, RH_byte2, Ch ;
 unsigned Temp, RH, Sum ;
void StartSignal(){
 GPIO_PinOutputEnable(GPIO_PIN_RD0);
 GPIO_PinClear(GPIO_PIN_RD0);
    CORETIMER_DelayMs(18);
 GPIO_PinSet(GPIO_PIN_RD0);
    CORETIMER_DelayUs(30);
 GPIO_PinInputEnable(GPIO_PIN_RD0);
 }
 //////////////////////////////
 void CheckResponse(){
    Check = 0;
    CORETIMER_DelayUs(40);
    if (GPIO_PinRead(GPIO_PIN_RD0) == 0){
       CORETIMER_DelayUs(80);
        if (GPIO_PinRead(GPIO_PIN_RD0) == 1) {
            Check = 1; 
           CORETIMER_DelayUs(40);
        }
    }
 }
   //////////////////////////////
char ReadData(){
    char i=0, j;
    for(j = 0; j < 8; j++){
    while(!GPIO_PinRead(GPIO_PIN_RD0)); //Wait until PORTD.F0 goes HIGH
    CORETIMER_DelayUs(30);
    if(GPIO_PinRead(GPIO_PIN_RD0) == 0)
        i&= ~(1<<(7 - j)); //Clear bit (7-b)
    else {i|= (1 << (7 - j)); //Set bit (7-b)
    while(GPIO_PinRead(GPIO_PIN_RD0));} //Wait until PORTD.F0 goes LOW
    }
 return i;
 }

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************



int main ( void )
{
    // Initialize all modules
    SYS_Initialize ( NULL );
    
    // Start timer 1 for heartbeat LED
    TMR1_CallbackRegister(TIMER1_InterruptSvcRoutine, (uintptr_t)NULL);
    TMR1_Start();
    
    ADC_CallbackRegister(ADC_ResultHandler, (uintptr_t)NULL);
    ADC_ConversionStart();
    
    
    I2C3_CallbackRegister(MCP4725_I2CCallback, (uintptr_t)NULL );
    TRISBbits.TRISB7 = 1;
    TRISBbits.TRISB13 = 1;
    
    // Initialize FSM
    char c='0';
    char menuOpt = '0';
    state = PRINT_MENU;
    char str[100];

    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
        ADC_ConversionStart();
        GPIO_PinSet(GPIO_PIN_RD0);
        
        switch(state){
            case IDLE:
                break;
            case PRINT_MENU:
                clearScreen();
                printMenu();
                state = AWAIT_INPUT;
                break;
            case AWAIT_INPUT:
                /*menuOpt = '0';
                ;char input[10];
                getStr(input, 1);
                if(validOption(input[0])){
                    state = PROCESS_INPUT;
                    menuOpt = input[0];
                }
                else{
                    state = PROCESS_INPUT;
                    menuOpt = 'z';
                }*/
                if (IFS1bits.U3RXIF)    //If we have received a char,
                {
                    c=U3RXREG;          //read it
                    U3TXREG=c;          //echo it back
                    if(validOption(c)) menuOpt = c;
                    if(c=='\r' || c=='\n')          //If that char was "A" then send a bunch of long stuff.
                    {
                        UARTprint("\n\r");
                        state = PROCESS_INPUT;
                    }
                    IFS1bits.U3RXIF=0;
                }
                break;
            case PROCESS_INPUT:
                UARTprint("\n\r");
                switch(menuOpt){
                    case '1':
                        if(heartbeat_en){
                            UARTprint("Heartbeat LED disabled\n\r");
                            heartbeat_en = 0;
                        }
                        else{
                            UARTprint("Heartbeat LED enabled\n\r");
                            heartbeat_en = 1;
                        }
                        printWaitReturn();
                        state = WAIT_RETURN;
                        break;
                    case '2':
                        /* Wait till ADC conversion result is available */
                        if(result_ready == true)
                        {
                            //TODO: make this into a function.. and use an array to store ADC counts
                            result_ready = false;
                            input_voltage = (float)adc_count * ADC_VREF / ADC_MAX_COUNT;
                            sprintf(str, "RB1(AN3) ADC Count = 0x%03x, ADC Input Voltage = %d.%d V \n\r", adc_count, (int)input_voltage, (int)((input_voltage - (int)input_voltage)*100.0));
                            UARTprint(str);
                            input_voltage = (float)adc_count2 * ADC_VREF / ADC_MAX_COUNT;
                            sprintf(str, "RB2(AN4) ADC Count = 0x%03x, ADC Input Voltage = %d.%d V \n\r", adc_count2, (int)input_voltage, (int)((input_voltage - (int)input_voltage)*100.0));
                            UARTprint(str);
                            input_voltage = (float)adc_count3 * ADC_VREF / ADC_MAX_COUNT;
                            sprintf(str, "POT(AN14) ADC Count = 0x%03x, ADC Input Voltage = %d.%d V \n\r", adc_count3, (int)input_voltage, (int)((input_voltage - (int)input_voltage)*100.0));
                            UARTprint(str);
                        }
                        else{
                            UARTprint("ERROR: ADC result not ready.\n\r");
                        }
                        printWaitReturn();
                        state = WAIT_RETURN;
                        break;
                    case '3':
                        UARTprint("Input decimal number to write to DAC: ");
                        //uint16_t dac_val = 0x0000; //Should be half
                        char dac[10];
                        getStr(dac, 10);
                        
                        //uint16_t dac_int = atoi(dac);
                        float dac_float = atof(dac);
                        uint16_t dac_val = dac_float * 4096 / 3.3;
                        if(!writeDAC(dac_val)){
                            UARTprint("Error occurred while writing to DAC\n\r");
                        }
                        else{
                            sprintf(str,"Successfully wrote %f to DAC\n\r", dac_float);
                            UARTprint(str);
                        }
                        printWaitReturn();
                        state = WAIT_RETURN;
                        break;
                    case '4':
                     
                        StartSignal();
                        CheckResponse();
                        if(Check == 1){
                            RH_byte1 = ReadData();
                            RH_byte2 = ReadData();
                            T_byte1 = ReadData();
                            T_byte2 = ReadData();
                            Sum = ReadData();
                            if(Sum == ((RH_byte1+RH_byte2+T_byte1+T_byte2) & 0XFF)){
                                RH = RH_byte1;
                                Temp = T_byte1;
                            }
                        }
                        sprintf(str, "Temperature is %d \n\r", Temp);
                        UARTprint(str);
                        sprintf(str, "Humidity is %d \n\r", RH);
                        UARTprint(str);
                        
                        printWaitReturn();
                        state = WAIT_RETURN;
                        break;
                    default:
                        UARTprint("Invalid option selected\n\r");
                        printWaitReturn();
                        state = WAIT_RETURN;
                        break;
                }
                
                break;
            case WAIT_RETURN:
                if (IFS1bits.U3RXIF)    //If we have received a char,
                {
                    c=U3RXREG;          //read it
                    U3TXREG=c;          //echo it back
                    if(c=='\r' || c=='\n')          //If that char was "A" then send a bunch of long stuff.
                    {
                        UARTprint("\n\r");
                        state = PRINT_MENU;
                    }
                    IFS1bits.U3RXIF=0;
                }
                menuOpt = '0';
                break;
            default:
                UARTprint("Invalid option selected\n\r");
                printWaitReturn();
                state = WAIT_RETURN;
        }                    
       
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

