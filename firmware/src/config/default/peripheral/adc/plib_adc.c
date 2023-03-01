/*******************************************************************************
  ADC Peripheral Library Interface Source File

  Company
    Microchip Technology Inc.

  File Name
    plib_adc.c

  Summary
    ADC peripheral library source.

  Description
    This file implements the ADC peripheral library.

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
#include "device.h"
#include "plib_adc.h"

// *****************************************************************************
// *****************************************************************************
// Section: ADC Implementation
// *****************************************************************************
// *****************************************************************************

/* Object to hold callback function and context */
ADC_CALLBACK_OBJECT ADC_CallbackObj;

void ADC_Initialize(void)
{
    AD1CON1CLR = _AD1CON1_ON_MASK;

    AD1CON1 = 0x4;
    AD1CON2 = 0x40c;
    AD1CON3 = 0x1f04;
    AD1CHS = 0xe;
    /* Input Scan */
    AD1CSS = 0x88204;

    /* Clear interrupt flag */
    IFS1CLR = _IFS1_AD1IF_MASK;
    /* Interrupt Enable */
    IEC1SET = _IEC1_AD1IE_MASK;

    /* Turn ON ADC */
    AD1CON1SET = _AD1CON1_ON_MASK;
}

void ADC_Enable(void)
{
    AD1CON1SET = _AD1CON1_ON_MASK;
}

void ADC_Disable(void)
{
    AD1CON1CLR = _AD1CON1_ON_MASK;
}

void ADC_SamplingStart(void)
{
    AD1CON1CLR = _AD1CON1_DONE_MASK;
    AD1CON1SET = _AD1CON1_SAMP_MASK;
}

void ADC_ConversionStart(void)
{
    AD1CON1CLR = _AD1CON1_SAMP_MASK;
}

void ADC_InputSelect(ADC_INPUT_POSITIVE positiveInput)
{
    AD1CHSbits.CH0SA = positiveInput;
}

void ADC_InputScanSelect(ADC_INPUTS_SCAN scanInputs)
{
    AD1CSS = scanInputs;
}

/*Check if conversion result is available */
bool ADC_ResultIsReady(void)
{
    return AD1CON1bits.DONE;
}

/* Read the conversion result */
uint32_t ADC_ResultGet(ADC_RESULT_BUFFER bufferNumber)
{
    return (*((&ADC1BUF0) + (bufferNumber << 2)));
}

void ADC_CallbackRegister(ADC_CALLBACK callback, uintptr_t context)
{
    ADC_CallbackObj.callback_fn = callback;
    ADC_CallbackObj.context = context;
}

void ADC_InterruptHandler(void)
{
    IFS1CLR = _IFS1_AD1IF_MASK;

    if (ADC_CallbackObj.callback_fn != NULL)
    {
        ADC_CallbackObj.callback_fn(ADC_CallbackObj.context);
    }
}
