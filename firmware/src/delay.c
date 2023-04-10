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

/*
 *  Write the appropriate values on the parallel data bus to set the ns delay
 *  @param ns the nanosecond delay
 *  @return nothing
 */
void set_ns_delay(int ns){
    // string buffer for writing to the UART
    char strns[100];

    // Input validation
    if(ns > 200 || ns < 0){
        sprintf(strbuf, "Error: %d is not in the accepted 0-200ns range.\n\r", ns);
        UARTprint(strbuf);
        return;
    }

    // Only unlock the ns delay chip if the ps delay chip is locked
    if(ps_is_locked()){
        unlock_ns();
    }
    else{
        sprintf(strns,"Error: Could not set nanosecond delay. Picosecond chip is still unlocked.\n\r");
        UARTprint(strns);
        return;
    }
    // Delay 1 us to make sure locks are correct
    CORETIMER_DelayUs(1);
    
    // Convert the ns delay to binary, and write to parallel bus
    ns_delay = ns;
    for(int i=0; i<8; i++){
     dn[i] = ns % 2;
     ns /= 2;
     GPIO_PinWrite(Dn_pin_names[i], dn[i]);
    }

    // Print out the ns delay in binary for GPIO debugging
    sprintf(strns, "Setting nanosecond delay to %dns = [", ns_delay);
    UARTprint(strns);
    for(int i=7; i>=0; i--){
        sprintf(strns,"%d", dn[i]);
        UARTprint(strns);
    }
    UARTprint("]\n\r");
    
    // Lock the ns chip
    CORETIMER_DelayUs(1);
    lock_ns();
}

/*
 *  Write the appropriate values on the parallel data bus and DAC to set the ps delay
 *  @param ps the full picosecond delay
 *  @return nothing
 */
void set_ps_delay(int ps){
    // string buffer to print to UART
    char strps[100];

    // Input validation
    if(ps > 999 || ps < 0){
        sprintf(strbuf, "Error: %d is not in the accepted 0-999ps range.\n\r", ps);
        UARTprint(strbuf);
        return;
    }

    // PS chip takes new value when latch pin is LOW
    // PS chip is locked when latch pin is HIGH
    if(ns_is_locked()){
        unlock_ps(); // If the NS chip is locked, then unlock the PS chip
    }
    else{
        sprintf(strps,"Error: Could not set picosecond delay. Nanosecond chip is still unlocked.\n\r");
        UARTprint(strps);
        return;
    }
    // Delay so locks are corret
    CORETIMER_DelayUs(1);
    
    // Separate ps delay into coarse (10ps intervals) and fine (0-9ps)
    ps_delay = ps;
    uint16_t cp = ps/10;
    uint8_t fp = ps%10;

    // Convert coarse ps delay to binary, and write to parallel data bus
    for(int i=0; i<10; i++){
     dcp[i] = cp % 2;
     cp /= 2;
     GPIO_PinWrite(Dn_pin_names[i], dcp[i]);
    }
    
    // Print out the ps delay in binary for GPIO debugging
    sprintf(strps,"Setting coarse ps delay to %dps = [", (ps/10)*10);
    UARTprint(strps);
    for(int i=9; i>=0; i--){
        sprintf(strps,"%d", dcp[i]);
        UARTprint(strps);
    }
    UARTprint("]\n\r");

    // Delay 1us to make sure ps chip takes new changes.
    CORETIMER_DelayUs(1);

    // Lock so values cannot be changed.
    lock_ps(); 

    // Set the FTUNE voltage
    sprintf(strps,"Setting FTUNE voltage for fine ps delay of %dps\n\r", fp);
    UARTprint(strps);
    // TODO: Implement this once zeek provides values for the DAC
    
}

/*
 *  Separate a string to appropriate ps an ns delays, then call the correct functions to set
 *  the delays.
 *  @param fd the full delay as a string
 *  @return nothing
 */
void set_full_delay(char *fd){
    // Check to see if the delay is a valid decimal number (only digits, negative sign, or decimal)
    if(isValidDecimal(fd)){
        // Convert delay to float to check bounds 0-200ns
        float delay = atof(fd);
        if(delay > 200.0 || delay <0){
            sprintf(strbuf, "%s is not in the accepted 0-200.000 ns\n\r", fd);
            UARTprint(strbuf);
            return;
        }

        // Split the string into the ns and ps integer components
        char psstr[10];
        int ps;
        int ns;
        // If the string has a decimal point, it has both ns and ps components (NNN.PPP)
        if(hasDecimal(fd)){   
            // Use sscanf to read the ns component as an integer and the ps component as a string 
            sscanf(fd, "%d.%s", &ns, psstr);
            // Convert the fractional ns to an integer ps string 3 digits wide (0.5ns converts to 500ps)
            // You cannot multiply by 1000. If user inputted 0.500 it would be 500,000ps which is incorrect.
            for(int i=strlen(psstr); i<3; i++){
                strcat(psstr, "0");
            }
            // Finally, convert to an integer
            ps = atoi(psstr);
        }
        else{
            // No decimal point was in the string. User inputted ns delay as an integer. Assume ps delay is 0
            ns = atoi(fd);
            ps = 0;
        }

        // Check to make sure ps and ns values are within acceptable range (0-200ns) and (0-999ps)
        if(!valid_ps(ps)){
            sprintf(strbuf, "%s does not have a valid picosecond value. Please enter a value between 0-999ps\n\r", fd);
            UARTprint(strbuf);
            return;
        }
        if(!valid_ns(ns)){
            sprintf(strbuf, "%s does not have a valid nanosecond value. Please enter a value between 0-200ns\n\r", fd);
            UARTprint(strbuf);
            return;
        }
        else{
            // Both ns and ps values are valid, set the delays
            set_ns_delay(ns);
            set_ps_delay(ps);
        }
    }
    else{
        // String was not a valid decimal number. Return without setting delays
        sprintf(strbuf, "%s is not a valid decimal number. Please enter a value between 0-200.000\n\r", fd);
        UARTprint(strbuf);
        return;
    }
}

/*
 *  Lock both the ns and ps delay chips so changes on data bus do not take effect
 *  @return nothing
 */
void lock_delay_chips(void){
    lock_ns(); //ns chip is locked when latch pin is LOW
    lock_ps(); //ps chip is locked when latch pin is HIGH
}

/*
 *  Lock the ps delay chips so changes on data bus do not take effect.
 *  @return nothing
 */
void lock_ps(void){
    GPIO_PinWrite(PS_LATCH_PIN, 1);
}
/*
 *  Unlock ps delay chips so a new ps value can be written on the data bus.
 *  @return nothing
 */
void unlock_ps(void){
    // PS chip is active low
    GPIO_PinWrite(PS_LATCH_PIN, 0);
}
// Get the lock state of the ps chip. Returns 1 if locked, 0 if unlocked.
uint8_t ps_is_locked(void){
    //PS chip is locked when latch pin is HIGH
    return GPIO_PinRead(PS_LATCH_PIN);
}

// Lock the nanosecond delay chip so changes on data bus do not effect ns delay
void lock_ns(void){
    GPIO_PinWrite(NS_LATCH_PIN, 0);
}
/*
 *  Unlock ns delay chips so a new ns value can be written on the data bus.
 *  @return nothing
 */
void unlock_ns(void){
    // NS chip is active high
    GPIO_PinWrite(NS_LATCH_PIN, 1);
}
// Get the lock state of the ns chip. Returns 1 if locked, 0 if unlocked.
uint8_t ns_is_locked(void){
    //NS chip is locked when latch pin is LOW
    return !GPIO_PinRead(NS_LATCH_PIN);
}

// Helper function to print the current delay in ns
void print_delay(void){
    sprintf(strbuf, "Current delay: %3d.%3d ns\n\r", ns_delay, ps_delay);
    UARTprint(strbuf);
}

// Return whether the ns delay is valid (between 0-200)
uint8_t valid_ns(int n){
    return (n <= 200 && n >= 0);
}

// Return whether the ps delay is valid (between 0-999)
uint8_t valid_ps(int p){
    return (p <= 999 && p >= 0);
}
