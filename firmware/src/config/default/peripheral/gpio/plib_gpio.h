/*******************************************************************************
  GPIO PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_gpio.h

  Summary:
    GPIO PLIB Header File

  Description:
    This library provides an interface to control and interact with Parallel
    Input/Output controller (GPIO) module.

*******************************************************************************/

/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

#ifndef PLIB_GPIO_H
#define PLIB_GPIO_H

#include <device.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data types and constants
// *****************************************************************************
// *****************************************************************************


/*** Macros for DELAY_D2 pin ***/
#define DELAY_D2_Set()               (LATASET = (1<<7))
#define DELAY_D2_Clear()             (LATACLR = (1<<7))
#define DELAY_D2_Toggle()            (LATAINV= (1<<7))
#define DELAY_D2_OutputEnable()      (TRISACLR = (1<<7))
#define DELAY_D2_InputEnable()       (TRISASET = (1<<7))
#define DELAY_D2_Get()               ((PORTA >> 7) & 0x1)
#define DELAY_D2_PIN                  GPIO_PIN_RA7

/*** Macros for GPIO_RB15_CLICK_PWM pin ***/
#define GPIO_RB15_CLICK_PWM_Set()               (LATBSET = (1<<15))
#define GPIO_RB15_CLICK_PWM_Clear()             (LATBCLR = (1<<15))
#define GPIO_RB15_CLICK_PWM_Toggle()            (LATBINV= (1<<15))
#define GPIO_RB15_CLICK_PWM_OutputEnable()      (TRISBCLR = (1<<15))
#define GPIO_RB15_CLICK_PWM_InputEnable()       (TRISBSET = (1<<15))
#define GPIO_RB15_CLICK_PWM_Get()               ((PORTB >> 15) & 0x1)
#define GPIO_RB15_CLICK_PWM_PIN                  GPIO_PIN_RB15

/*** Macros for DELAY_D8 pin ***/
#define DELAY_D8_Set()               (LATASET = (1<<13))
#define DELAY_D8_Clear()             (LATACLR = (1<<13))
#define DELAY_D8_Toggle()            (LATAINV= (1<<13))
#define DELAY_D8_OutputEnable()      (TRISACLR = (1<<13))
#define DELAY_D8_InputEnable()       (TRISASET = (1<<13))
#define DELAY_D8_Get()               ((PORTA >> 13) & 0x1)
#define DELAY_D8_PIN                  GPIO_PIN_RA13

/*** Macros for DELAY_D7 pin ***/
#define DELAY_D7_Set()               (LATASET = (1<<12))
#define DELAY_D7_Clear()             (LATACLR = (1<<12))
#define DELAY_D7_Toggle()            (LATAINV= (1<<12))
#define DELAY_D7_OutputEnable()      (TRISACLR = (1<<12))
#define DELAY_D7_InputEnable()       (TRISASET = (1<<12))
#define DELAY_D7_Get()               ((PORTA >> 12) & 0x1)
#define DELAY_D7_PIN                  GPIO_PIN_RA12

/*** Macros for DELAY_D6 pin ***/
#define DELAY_D6_Set()               (LATASET = (1<<11))
#define DELAY_D6_Clear()             (LATACLR = (1<<11))
#define DELAY_D6_Toggle()            (LATAINV= (1<<11))
#define DELAY_D6_OutputEnable()      (TRISACLR = (1<<11))
#define DELAY_D6_InputEnable()       (TRISASET = (1<<11))
#define DELAY_D6_Get()               ((PORTA >> 11) & 0x1)
#define DELAY_D6_PIN                  GPIO_PIN_RA11

/*** Macros for GPIO_RB1_CLICK_INT pin ***/
#define GPIO_RB1_CLICK_INT_Set()               (LATBSET = (1<<1))
#define GPIO_RB1_CLICK_INT_Clear()             (LATBCLR = (1<<1))
#define GPIO_RB1_CLICK_INT_Toggle()            (LATBINV= (1<<1))
#define GPIO_RB1_CLICK_INT_OutputEnable()      (TRISBCLR = (1<<1))
#define GPIO_RB1_CLICK_INT_InputEnable()       (TRISBSET = (1<<1))
#define GPIO_RB1_CLICK_INT_Get()               ((PORTB >> 1) & 0x1)
#define GPIO_RB1_CLICK_INT_PIN                  GPIO_PIN_RB1

/*** Macros for GPIO_RB2_CLICK_CS pin ***/
#define GPIO_RB2_CLICK_CS_Set()               (LATBSET = (1<<2))
#define GPIO_RB2_CLICK_CS_Clear()             (LATBCLR = (1<<2))
#define GPIO_RB2_CLICK_CS_Toggle()            (LATBINV= (1<<2))
#define GPIO_RB2_CLICK_CS_OutputEnable()      (TRISBCLR = (1<<2))
#define GPIO_RB2_CLICK_CS_InputEnable()       (TRISBSET = (1<<2))
#define GPIO_RB2_CLICK_CS_Get()               ((PORTB >> 2) & 0x1)
#define GPIO_RB2_CLICK_CS_PIN                  GPIO_PIN_RB2

/*** Macros for NS_LATCH pin ***/
#define NS_LATCH_Set()               (LATCSET = (1<<0))
#define NS_LATCH_Clear()             (LATCCLR = (1<<0))
#define NS_LATCH_Toggle()            (LATCINV= (1<<0))
#define NS_LATCH_OutputEnable()      (TRISCCLR = (1<<0))
#define NS_LATCH_InputEnable()       (TRISCSET = (1<<0))
#define NS_LATCH_Get()               ((PORTC >> 0) & 0x1)
#define NS_LATCH_PIN                  GPIO_PIN_RC0

/*** Macros for PS_LATCH pin ***/
#define PS_LATCH_Set()               (LATCSET = (1<<1))
#define PS_LATCH_Clear()             (LATCCLR = (1<<1))
#define PS_LATCH_Toggle()            (LATCINV= (1<<1))
#define PS_LATCH_OutputEnable()      (TRISCCLR = (1<<1))
#define PS_LATCH_InputEnable()       (TRISCSET = (1<<1))
#define PS_LATCH_Get()               ((PORTC >> 1) & 0x1)
#define PS_LATCH_PIN                  GPIO_PIN_RC1

/*** Macros for DELAY_D0 pin ***/
#define DELAY_D0_Set()               (LATASET = (1<<3))
#define DELAY_D0_Clear()             (LATACLR = (1<<3))
#define DELAY_D0_Toggle()            (LATAINV= (1<<3))
#define DELAY_D0_OutputEnable()      (TRISACLR = (1<<3))
#define DELAY_D0_InputEnable()       (TRISASET = (1<<3))
#define DELAY_D0_Get()               ((PORTA >> 3) & 0x1)
#define DELAY_D0_PIN                  GPIO_PIN_RA3

/*** Macros for DELAY_D3 pin ***/
#define DELAY_D3_Set()               (LATASET = (1<<8))
#define DELAY_D3_Clear()             (LATACLR = (1<<8))
#define DELAY_D3_Toggle()            (LATAINV= (1<<8))
#define DELAY_D3_OutputEnable()      (TRISACLR = (1<<8))
#define DELAY_D3_InputEnable()       (TRISASET = (1<<8))
#define DELAY_D3_Get()               ((PORTA >> 8) & 0x1)
#define DELAY_D3_PIN                  GPIO_PIN_RA8

/*** Macros for DELAY_D4 pin ***/
#define DELAY_D4_Set()               (LATASET = (1<<9))
#define DELAY_D4_Clear()             (LATACLR = (1<<9))
#define DELAY_D4_Toggle()            (LATAINV= (1<<9))
#define DELAY_D4_OutputEnable()      (TRISACLR = (1<<9))
#define DELAY_D4_InputEnable()       (TRISASET = (1<<9))
#define DELAY_D4_Get()               ((PORTA >> 9) & 0x1)
#define DELAY_D4_PIN                  GPIO_PIN_RA9

/*** Macros for POT pin ***/
#define POT_Get()               ((PORTC >> 8) & 0x1)
#define POT_PIN                  GPIO_PIN_RC8

/*** Macros for DELAY_D1 pin ***/
#define DELAY_D1_Set()               (LATASET = (1<<5))
#define DELAY_D1_Clear()             (LATACLR = (1<<5))
#define DELAY_D1_Toggle()            (LATAINV= (1<<5))
#define DELAY_D1_OutputEnable()      (TRISACLR = (1<<5))
#define DELAY_D1_InputEnable()       (TRISASET = (1<<5))
#define DELAY_D1_Get()               ((PORTA >> 5) & 0x1)
#define DELAY_D1_PIN                  GPIO_PIN_RA5

/*** Macros for DELAY_D9 pin ***/
#define DELAY_D9_Set()               (LATASET = (1<<14))
#define DELAY_D9_Clear()             (LATACLR = (1<<14))
#define DELAY_D9_Toggle()            (LATAINV= (1<<14))
#define DELAY_D9_OutputEnable()      (TRISACLR = (1<<14))
#define DELAY_D9_InputEnable()       (TRISASET = (1<<14))
#define DELAY_D9_Get()               ((PORTA >> 14) & 0x1)
#define DELAY_D9_PIN                  GPIO_PIN_RA14

/*** Macros for DELAY_D5 pin ***/
#define DELAY_D5_Set()               (LATASET = (1<<10))
#define DELAY_D5_Clear()             (LATACLR = (1<<10))
#define DELAY_D5_Toggle()            (LATAINV= (1<<10))
#define DELAY_D5_OutputEnable()      (TRISACLR = (1<<10))
#define DELAY_D5_InputEnable()       (TRISASET = (1<<10))
#define DELAY_D5_Get()               ((PORTA >> 10) & 0x1)
#define DELAY_D5_PIN                  GPIO_PIN_RA10


// *****************************************************************************
/* GPIO Port

  Summary:
    Identifies the available GPIO Ports.

  Description:
    This enumeration identifies the available GPIO Ports.

  Remarks:
    The caller should not rely on the specific numbers assigned to any of
    these values as they may change from one processor to the next.

    Not all ports are available on all devices.  Refer to the specific
    device data sheet to determine which ports are supported.
*/

typedef enum
{
    GPIO_PORT_A = 0,
    GPIO_PORT_B = 1,
    GPIO_PORT_C = 2,
    GPIO_PORT_D = 3,
} GPIO_PORT;

typedef enum
{
    GPIO_INTERRUPT_ON_MISMATCH,
    GPIO_INTERRUPT_ON_RISING_EDGE,
    GPIO_INTERRUPT_ON_FALLING_EDGE,
    GPIO_INTERRUPT_ON_BOTH_EDGES,
}GPIO_INTERRUPT_STYLE;

// *****************************************************************************
/* GPIO Port Pins

  Summary:
    Identifies the available GPIO port pins.

  Description:
    This enumeration identifies the available GPIO port pins.

  Remarks:
    The caller should not rely on the specific numbers assigned to any of
    these values as they may change from one processor to the next.

    Not all pins are available on all devices.  Refer to the specific
    device data sheet to determine which pins are supported.
*/

typedef enum
{
    GPIO_PIN_RA0 = 0,
    GPIO_PIN_RA1 = 1,
    GPIO_PIN_RA2 = 2,
    GPIO_PIN_RA3 = 3,
    GPIO_PIN_RA4 = 4,
    GPIO_PIN_RA5 = 5,
    GPIO_PIN_RA6 = 6,
    GPIO_PIN_RA7 = 7,
    GPIO_PIN_RA8 = 8,
    GPIO_PIN_RA9 = 9,
    GPIO_PIN_RA10 = 10,
    GPIO_PIN_RA11 = 11,
    GPIO_PIN_RA12 = 12,
    GPIO_PIN_RA13 = 13,
    GPIO_PIN_RA14 = 14,
    GPIO_PIN_RA15 = 15,
    GPIO_PIN_RB0 = 16,
    GPIO_PIN_RB1 = 17,
    GPIO_PIN_RB2 = 18,
    GPIO_PIN_RB3 = 19,
    GPIO_PIN_RB4 = 20,
    GPIO_PIN_RB5 = 21,
    GPIO_PIN_RB6 = 22,
    GPIO_PIN_RB7 = 23,
    GPIO_PIN_RB8 = 24,
    GPIO_PIN_RB9 = 25,
    GPIO_PIN_RB10 = 26,
    GPIO_PIN_RB11 = 27,
    GPIO_PIN_RB13 = 29,
    GPIO_PIN_RB14 = 30,
    GPIO_PIN_RB15 = 31,
    GPIO_PIN_RC0 = 32,
    GPIO_PIN_RC1 = 33,
    GPIO_PIN_RC2 = 34,
    GPIO_PIN_RC3 = 35,
    GPIO_PIN_RC4 = 36,
    GPIO_PIN_RC5 = 37,
    GPIO_PIN_RC6 = 38,
    GPIO_PIN_RC7 = 39,
    GPIO_PIN_RC8 = 40,
    GPIO_PIN_RC9 = 41,
    GPIO_PIN_RC10 = 42,
    GPIO_PIN_RC11 = 43,
    GPIO_PIN_RC12 = 44,
    GPIO_PIN_RC13 = 45,
    GPIO_PIN_RC14 = 46,
    GPIO_PIN_RC15 = 47,
    GPIO_PIN_RD0 = 48,
    GPIO_PIN_RD1 = 49,
    GPIO_PIN_RD2 = 50,
    GPIO_PIN_RD3 = 51,
    GPIO_PIN_RD4 = 52,

    /* This element should not be used in any of the GPIO APIs.
       It will be used by other modules or application to denote that none of the GPIO Pin is used */
    GPIO_PIN_NONE = -1

} GPIO_PIN;


void GPIO_Initialize(void);

// *****************************************************************************
// *****************************************************************************
// Section: GPIO Functions which operates on multiple pins of a port
// *****************************************************************************
// *****************************************************************************

uint32_t GPIO_PortRead(GPIO_PORT port);

void GPIO_PortWrite(GPIO_PORT port, uint32_t mask, uint32_t value);

uint32_t GPIO_PortLatchRead ( GPIO_PORT port );

void GPIO_PortSet(GPIO_PORT port, uint32_t mask);

void GPIO_PortClear(GPIO_PORT port, uint32_t mask);

void GPIO_PortToggle(GPIO_PORT port, uint32_t mask);

void GPIO_PortInputEnable(GPIO_PORT port, uint32_t mask);

void GPIO_PortOutputEnable(GPIO_PORT port, uint32_t mask);

// *****************************************************************************
// *****************************************************************************
// Section: GPIO Functions which operates on one pin at a time
// *****************************************************************************
// *****************************************************************************

static inline void GPIO_PinWrite(GPIO_PIN pin, bool value)
{
    GPIO_PortWrite((GPIO_PORT)(pin>>4), (uint32_t)(0x1) << (pin & 0xF), (uint32_t)(value) << (pin & 0xF));
}

static inline bool GPIO_PinRead(GPIO_PIN pin)
{
    return (bool)(((GPIO_PortRead((GPIO_PORT)(pin>>4))) >> (pin & 0xF)) & 0x1);
}

static inline bool GPIO_PinLatchRead(GPIO_PIN pin)
{
    return (bool)((GPIO_PortLatchRead((GPIO_PORT)(pin>>4)) >> (pin & 0xF)) & 0x1);
}

static inline void GPIO_PinToggle(GPIO_PIN pin)
{
    GPIO_PortToggle((GPIO_PORT)(pin>>4), 0x1 << (pin & 0xF));
}

static inline void GPIO_PinSet(GPIO_PIN pin)
{
    GPIO_PortSet((GPIO_PORT)(pin>>4), 0x1 << (pin & 0xF));
}

static inline void GPIO_PinClear(GPIO_PIN pin)
{
    GPIO_PortClear((GPIO_PORT)(pin>>4), 0x1 << (pin & 0xF));
}

static inline void GPIO_PinInputEnable(GPIO_PIN pin)
{
    GPIO_PortInputEnable((GPIO_PORT)(pin>>4), 0x1 << (pin & 0xF));
}

static inline void GPIO_PinOutputEnable(GPIO_PIN pin)
{
    GPIO_PortOutputEnable((GPIO_PORT)(pin>>4), 0x1 << (pin & 0xF));
}


// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END
#endif // PLIB_GPIO_H
