#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <string.h>
#include "definitions.h"                // SYS function prototypes
#include "uart_helper.h"
#include "delay.h"
#include <stdlib.h>


uint16_t dn[8];
uint16_t dcp[10];
char strbuf[100];
uint16_t ns_delay;
uint16_t ps_delay;
uint32_t Dn_pin_names[10] = {
    DELAY_D0_PIN,
    DELAY_D1_PIN,
    DELAY_D2_PIN,
    DELAY_D3_PIN,
    DELAY_D4_PIN,
    DELAY_D5_PIN,
    DELAY_D6_PIN,
    DELAY_D7_PIN,
    DELAY_D8_PIN,
    DELAY_D9_PIN
};

void set_ns_delay(char *ns){
    // NS accepts new data when latch is HIGH
    // NS locks when latch is LOW
    uint16_t n;
    // Input validation
    if(isValidDecimal(ns)){
        n = atoi(ns);
        if(n > 200 || n < 0){
            sprintf(strbuf, "Error: %s is not in the accepted 0-200ns range.\n\r", ns);
            UARTprint(strbuf);
            return;
        }
    }
    else{
        sprintf(strbuf, "Error: %s is not a valid number. Please enter a value between 0-200ns\n\r", ns);
        UARTprint(strbuf);
        return;
    }
    char strns[100];
    if(ps_is_locked()){ // If the PS chip is locked, unlock the NS chip
        // FIXME: DEBUG PRINT, GET RID OF
//        sprintf(strns,"DEBUG: Unlocking the NS chip, red=ON, green=ON\n\r");
//        UARTprint(strns);
        unlock_ns();
    }
    else{
        sprintf(strns,"Error: Could not set nanosecond delay. Picosecond chip is still unlocked.\n\r");
        UARTprint(strns);
        return;
    }
    //getStr(strns, 5); //FIXME: GET RID OF THIS, THIS IS JUST FOR DEBUG/DEMO
    CORETIMER_DelayUs(1);
    
    ns_delay = n;
    for(int i=0; i<8; i++){
     dn[i] = n % 2;
     n /= 2;
     GPIO_PinWrite(Dn_pin_names[i], dn[i]);
    }
    sprintf(strns, "Setting nanosecond delay to %dns = [", atoi(ns));
    UARTprint(strns);
    for(int i=7; i>=0; i--){
        sprintf(strns,"%d", dn[i]);
        UARTprint(strns);
    }
    UARTprint("]\n\r");
    
    CORETIMER_DelayUs(1);
//    sprintf(strns,"DEBUG: About to lock the NS chip, red=OFF, green=ON\n\r");
//    UARTprint(strns);
    //getStr(strns, 5); //FIXME: GET RID OF THIS, THIS IS JUST FOR DEBUG/DEMO
    lock_ns();
}

void set_ps_delay(char *ps){
    uint16_t p;
    // Input validation
    if(isValidDecimal(ps)){
        p = atoi(ps);
        if(p > 999 || p < 0){
            sprintf(strbuf, "Error: %s is not in the accepted 0-999ps range.\n\r", ps);
            UARTprint(strbuf);
            return;
        }
    }
    else{
        sprintf(strbuf, "Error: %s is not a valid number. Please enter a value between 0-999ps\n\r", ps);
        UARTprint(strbuf);
        return;
    }
    
    char strps[100];
    // PS chip takes new value when latch pin is LOW
    // PS chip is locked when latch pin is HIGH
    if(ns_is_locked()){
        // FIXME: DEBUG PRINT, GET RID OF
        //sprintf(strps,"DEBUG: Unlocking the PS chip, red=OFF, green=OFF\n\r");
        //UARTprint(strps);
        unlock_ps(); // If the NS chip is locked, then unlock the PS chip
    }
    else{
        sprintf(strps,"Error: Could not set picosecond delay. Nanosecond chip is still unlocked.\n\r");
        UARTprint(strps);
        return;
    }
    CORETIMER_DelayUs(1);
    //getStr(strps, 5); //FIXME: GET RID OF THIS, THIS IS JUST FOR DEBUG/DEMO
    
    ps_delay = p;
    uint16_t cp = p/10;
    uint8_t fp = p%10;
    
    for(int i=0; i<10; i++){
     dcp[i] = cp % 2;
     cp /= 2;
     GPIO_PinWrite(Dn_pin_names[i], dcp[i]);
    }
    
    sprintf(strps,"Setting coarse ps delay to %dps = [", (p/10)*10);
    UARTprint(strps);
    for(int i=9; i>=0; i--){
        sprintf(strps,"%d", dcp[i]);
        UARTprint(strps);
    }
    UARTprint("]\n\r");
    
    CORETIMER_DelayUs(1);
    //sprintf(strps,"DEBUG: About to lock the PS chip, red=OFF, green=ON\n\r");
    //UARTprint(strps);
    //getStr(strps, 5); //FIXME: GET RID OF THIS, THIS IS JUST FOR DEBUG/DEMO
    lock_ps(); // Lock so values cannot be changed.

    
    // Set the FTUNE voltage
    sprintf(strps,"Setting fine ps delay to %dps (TODO)\n\r", fp);
    UARTprint(strps);
    
}

void set_full_delay(char *fd){
    if(isValidDecimal(fd)){
        float delay = atof(fd);
        
        if(delay > 200.0 || delay <0){
            sprintf(strbuf, "%s is not in the accepted 0-200.000 ns\n\r", fd);
            UARTprint(strbuf);
            return;
        }
        char psstr[10];
        char nsstr[10];
        // Split the decimal string into the ns and ps components
        if(hasDecimal(fd)){
            const char s[2] = ".";
            char *nstok;
            char *pstok;
            nstok = strtok(fd, s);
            pstok = strtok(NULL, s);
            strcpy(psstr, pstok);
            psstr[3] = '\0';
            strcpy(nsstr, nstok);
        }
        else{
            strcpy(nsstr, fd);
            strcpy(psstr, "0\0");
        }

        //sprintf(strbuf, "ns: %s ps: %s\n\r", nstok, pstok);
        //UARTprint(strbuf);
        
        
        //sprintf(strbuf, "ns = %d; ps = %d\n\r", nanosecond, picosecond);
        //UARTprint(strbuf);
                
        if(!valid_ps(psstr)){
            sprintf(strbuf, "%s does not have a valid picosecond value. Please enter a value between 0-999ps\n\r", fd);
            UARTprint(strbuf);
            return;
        }
        if(!valid_ns(nsstr)){
            sprintf(strbuf, "%s does not have a valid nanosecond value. Please enter a value between 0-200ns\n\r", fd);
            UARTprint(strbuf);
            return;
        }
        else{
            set_ns_delay(nsstr);
            set_ps_delay(psstr);
        }
    }
    else{
        sprintf(strbuf, "%s is not a valid decimal number. Please enter a value between 0-200.000\n\r", fd);
        UARTprint(strbuf);
        return;
    }
}

void lock_delay_chips(void){
    lock_ns(); //PS chip is locked when latch pin is HIGH
    lock_ps(); //NS chip is locked when latch pin is LOW
}

void lock_ps(void){
    GPIO_PinWrite(PS_LATCH_PIN, 1);
}
void unlock_ps(void){
    GPIO_PinWrite(PS_LATCH_PIN, 0);
}
// Get the lock state of the ps chip. Returns 1 if locked, 0 if unlocked.
uint8_t ps_is_locked(void){
    //PS chip is locked when latch pin is HIGH
    return GPIO_PinRead(PS_LATCH_PIN);
}

void lock_ns(void){
    GPIO_PinWrite(NS_LATCH_PIN, 0);
}
void unlock_ns(void){
    GPIO_PinWrite(NS_LATCH_PIN, 1);
}
// Get the lock state of the ns chip. Returns 1 if locked, 0 if unlocked.
uint8_t ns_is_locked(void){
    //NS chip is locked when latch pin is LOW
    return !GPIO_PinRead(NS_LATCH_PIN);
}

void print_delay(void){
    sprintf(strbuf, "Current delay: %3d.%3d ns\n\r", ns_delay, ps_delay);
    UARTprint(strbuf);
}

uint8_t valid_ns(char *ns){
    uint16_t n = atoi(ns);
    return (n <= 200 && n >= 0);
}

uint8_t valid_ps(char *ps){
    uint16_t p = atoi(ps);
    return (p <= 999 && p >= 0);
}
