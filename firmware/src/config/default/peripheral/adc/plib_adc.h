/*******************************************************************************
  ADC Peripheral Library Interface Header File

  Company
    Microchip Technology Inc.

  File Name
    plib_adc.h

  Summary
    ADC peripheral library interface.

  Description
    This file defines the interface to the ADC peripheral library.  This
    library provides access to and control of the associated peripheral
    instance.

*******************************************************************************/

// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END

#ifndef PLIB_ADC_H    // Guards against multiple inclusion
#define PLIB_ADC_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

/*  This section lists the other files that are included in this file.
*/
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
// Section: Data Types
// *****************************************************************************
// *****************************************************************************
/*  The following data type definitions are used by the functions in this
    interface and should be considered part it.
*/
typedef enum {
    ADC_RESULT_BUFFER_0 = 0,
    ADC_RESULT_BUFFER_1,
    ADC_RESULT_BUFFER_2,
    ADC_RESULT_BUFFER_3,
    ADC_RESULT_BUFFER_4,
    ADC_RESULT_BUFFER_5,
    ADC_RESULT_BUFFER_6,
    ADC_RESULT_BUFFER_7,
    ADC_RESULT_BUFFER_8,
    ADC_RESULT_BUFFER_9,
    ADC_RESULT_BUFFER_10,
    ADC_RESULT_BUFFER_11,
    ADC_RESULT_BUFFER_12,
    ADC_RESULT_BUFFER_13,
    ADC_RESULT_BUFFER_14,
    ADC_RESULT_BUFFER_15,
    ADC_RESULT_BUFFER_16,
    ADC_RESULT_BUFFER_17,
    ADC_RESULT_BUFFER_18,
    ADC_RESULT_BUFFER_19,
    ADC_RESULT_BUFFER_20,
    ADC_RESULT_BUFFER_21,
}ADC_RESULT_BUFFER;

typedef enum
{
    ADC_INPUT_POSITIVE_AVDD = 30,
    ADC_INPUT_POSITIVE_AVSS = 29,
    ADC_INPUT_POSITIVE_VBG = 28,
    ADC_INPUT_POSITIVE_VDDCORE = 27,
    ADC_INPUT_POSITIVE_AN19 = 19,
    ADC_INPUT_POSITIVE_AN18 = 18,
    ADC_INPUT_POSITIVE_AN17 = 17,
    ADC_INPUT_POSITIVE_AN16 = 16,
    ADC_INPUT_POSITIVE_AN15 = 15,
    ADC_INPUT_POSITIVE_AN14 = 14,
    ADC_INPUT_POSITIVE_AN13 = 13,
    ADC_INPUT_POSITIVE_AN12 = 12,
    ADC_INPUT_POSITIVE_AN11 = 11,
    ADC_INPUT_POSITIVE_AN10 = 10,
    ADC_INPUT_POSITIVE_AN9 = 9,
    ADC_INPUT_POSITIVE_AN8 = 8,
    ADC_INPUT_POSITIVE_AN7 = 7,
    ADC_INPUT_POSITIVE_AN6 = 6,
    ADC_INPUT_POSITIVE_AN5 = 5,
    ADC_INPUT_POSITIVE_AN4 = 4,
    ADC_INPUT_POSITIVE_AN3 = 3,
    ADC_INPUT_POSITIVE_AN2 = 2,
    ADC_INPUT_POSITIVE_AN1 = 1,
    ADC_INPUT_POSITIVE_AN0 = 0,
}ADC_INPUT_POSITIVE;

typedef enum
{
    ADC_INPUT_SCAN_AN0 = 0x1,
    ADC_INPUT_SCAN_AN1 = 0x2,
    ADC_INPUT_SCAN_AN2 = 0x4,
    ADC_INPUT_SCAN_AN3 = 0x8,
    ADC_INPUT_SCAN_AN4 = 0x10,
    ADC_INPUT_SCAN_AN5 = 0x20,
    ADC_INPUT_SCAN_AN6 = 0x40,
    ADC_INPUT_SCAN_AN7 = 0x80,
    ADC_INPUT_SCAN_AN8 = 0x100,
    ADC_INPUT_SCAN_AN9 = 0x200,
    ADC_INPUT_SCAN_AN10 = 0x400,
    ADC_INPUT_SCAN_AN11 = 0x800,
    ADC_INPUT_SCAN_AN12 = 0x1000,
    ADC_INPUT_SCAN_AN13 = 0x2000,
    ADC_INPUT_SCAN_AN14 = 0x4000,
    ADC_INPUT_SCAN_AN15 = 0x8000,
    ADC_INPUT_SCAN_AN16 = 0x10000,
    ADC_INPUT_SCAN_AN17 = 0x20000,
    ADC_INPUT_SCAN_AN18 = 0x40000,
    ADC_INPUT_SCAN_AN19 = 0x80000,
    ADC_INPUT_SCAN_VDDCORE = 0x8000000,
    ADC_INPUT_SCAN_VBG = 0x10000000,
    ADC_INPUT_SCAN_AVSS = 0x20000000,
    ADC_INPUT_SCAN_AVDD = 0x40000000,
}ADC_INPUTS_SCAN;


// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines
// *****************************************************************************
// *****************************************************************************
/* The following functions make up the methods (set of possible operations) of
   this interface.
*/

void ADC_Initialize (void);

void ADC_Enable(void);
void ADC_Disable(void);

void ADC_SamplingStart(void);
void ADC_ConversionStart(void);

void ADC_InputSelect(ADC_INPUT_POSITIVE positiveInput);
void ADC_InputScanSelect(ADC_INPUTS_SCAN scanInputs);

bool ADC_ResultIsReady(void);
uint32_t ADC_ResultGet(ADC_RESULT_BUFFER bufferNumber);


// *****************************************************************************

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

}
#endif
// DOM-IGNORE-END

#endif //PLIB_ADC_H

/**
 End of File
*/
