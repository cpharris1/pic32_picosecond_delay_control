#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <string.h>
#include "definitions.h"                // SYS function prototypes
#include "uart_helper.h"


uint16_t dn[8];
uint16_t dcp[11];
uint32_t dn_names[8] = {
    NS_D0_PIN,
    NS_D1_PIN,
    NS_D2_PIN,
    NS_D3_PIN,
    NS_D4_PIN,
    NS_D5_PIN,
    NS_D6_PIN,
    NS_D7_PIN
};
uint32_t dcp_names[11] = {
    PS_D0_PIN,
    PS_D1_PIN,
    PS_D2_PIN,
    PS_D3_PIN,
    PS_D4_PIN,
    PS_D5_PIN,
    PS_D6_PIN,
    PS_D7_PIN,
    PS_D8_PIN,
    PS_D9_PIN,
    PS_D10_PIN
};

void set_ns_delay(char *ns){
    uint16_t n = atoi(ns);
    for(int i=0; i<8; i++){
     dn[i] = n % 2;
     n /= 2;
    }
    char strns[100];
    for(int i=0; i<8; i++){
        sprintf(strns,"dn[%d] = %d\n\r", i, dn[i]);
        UARTprint(strns);
        GPIO_PinWrite(dn_names[i], dn[i]);
    }
    /*
    dn[0] ? NS_D0_Set() : NS_D0_Clear();
    dn[1] ? NS_D1_Set() : NS_D1_Clear();
    dn[2] ? NS_D2_Set() : NS_D2_Clear();
    dn[3] ? NS_D3_Set() : NS_D3_Clear();
    dn[4] ? NS_D4_Set() : NS_D4_Clear();
    dn[5] ? NS_D5_Set() : NS_D5_Clear();
    dn[6] ? NS_D6_Set() : NS_D6_Clear();
    dn[7] ? NS_D7_Set() : NS_D7_Clear();
    */
}

void set_ps_delay(char *ps){
    uint16_t p = atoi(ps);
    uint16_t cp = p/10;
    //uint8_t fp = p%10;
    
    for(int i=0; i<11; i++){
     dcp[i] = cp % 2;
     cp /= 2;
    }
    char strps[100];
    for(int i=0; i<11; i++){
        sprintf(strps,"dcp[%d] = %d\n\r", i, dcp[i]);
        UARTprint(strps);
        GPIO_PinWrite(dcp_names[i], dcp[i]);
    }
    /*
    dcp[0] ? PS_D0_Set() : PS_D0_Clear();
    dcp[1] ? PS_D1_Set() : PS_D1_Clear();
    dcp[2] ? PS_D2_Set() : PS_D2_Clear();
    dcp[3] ? PS_D3_Set() : PS_D3_Clear();
    dcp[4] ? PS_D4_Set() : PS_D4_Clear();
    dcp[5] ? PS_D5_Set() : PS_D5_Clear();
    dcp[6] ? PS_D6_Set() : PS_D6_Clear();
    dcp[7] ? PS_D7_Set() : PS_D7_Clear();
    dcp[8] ? PS_D8_Set() : PS_D8_Clear();
    dcp[9] ? PS_D9_Set() : PS_D9_Clear();
    dcp[10] ? PS_D10_Set() : PS_D10_Clear();
    */
}
