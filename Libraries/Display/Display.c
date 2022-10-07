/* Project Documentation
  @Company
    Universidad Tecnologica de Queretaro - Visteon

  @Project Name
  ProyetoFinalDisplayV4

  @File Name
    Display.c

  @Author
    Romero, Rodrigo
    Olvera, Diego
    Perches, Gibran

  @Summary
    This is the a source file for C language. The purpose is to
    initialize all display libraries and functions.

  @Date
  07/10/2022

  @Version
  V4.0

  @Description
    This source file is going to implement the SPI module of the microcontroller
    to communicate it with a 7-digit segment and the serial controller MAX7219.
    The system will use the pins PD1 - MOSI, PN3 - CS & PD3 - SPI CLK.


    Generation Information :
        Product Revision  :  TM4C129EXL - 1.168.0
        Device            :  TM4C129ENCPDT
    The generated drivers are tested against the following:
        Languaje          :  C ANSI C 89 Mode
        Compiler          :  TIv16.9.6LTS
        CCS               :  CCS v7.4.0.00015
*/

/*
    (c) 2022 Romero Rodrigo, OlveraDiego, Perches Gibran. You may use this
    software and any derivatives exclusively with Texas Instruments products.

    THIS SOFTWARE IS SUPPLIED BY Romero Rodrigo, Olvera Diego, Perches Gibran "AS IS".
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING
    ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH TEXAS INSTRUMENTS PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    Romero Rodrigo, Olvera Diego, Perches Gibran PROVIDES THIS SOFTWARE CONDITIONALLY UPON
    YOUR ACCEPTANCE OF THESE TERMS.
*/

#include "Display.h"

void SpiSend(uint8_t registerAddress, uint8_t registerValue )
{
    uint8_t genericCounter        = 0;
    static const uint8_t ui8Delay = 5;

    // Turns off the PORTN to send information
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_3,0x00);
    SysCtlDelay(ui8Delay);

    // Generates a cycle to send the information bit by bit
    // First send the registeAddress corresponding to the datasheet
    for( genericCounter = 8; genericCounter > 0; genericCounter--)
    {
        // SPI CLK LOW
        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3,0x00);
        SysCtlDelay(ui8Delay);
        //MSB first. Charge Boolean Data in MOSI
        if(registerAddress & 0x80)
        {
            GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1,0x02);
        }
        else
        {
            GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0x00);
        }
        // Arithmetic shift that prepares the following bit
        registerAddress <<= 1;
        SysCtlDelay(ui8Delay);
        // Sends the information in the high value of the SPI CLK
        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3,0x08);
        SysCtlDelay(ui8Delay);
    }

    // Generates a cycle to send the information bit by bit
    // Second send the registeValue
    SysCtlDelay(ui8Delay);
    for(genericCounter = 8; genericCounter > 0; genericCounter--)
    {
        // SPI CLK LOW
        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3,0x00);     // SPI CLK LOW
        SysCtlDelay(ui8Delay);
        //LSB first. Charge Boolean Data in MOSI
        if(registerValue & 0x80){
            GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1,0x02);
        }
        else{
            GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0x00);
        }
        // Arithmetic shift that prepares the following bit
        registerValue <<= 1;
        SysCtlDelay(ui8Delay);
        // Sends the information in the high value of the SPI CLK
        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3,0x08);
        SysCtlDelay(ui8Delay);
    }
    // Ends the transmission of the information
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_3,0x08);
}

//Initialize the PORT N3 and PORT D1, D3 to use the SPI protocol
void initDisplay(void)
{
    uint8_t  indexCounter = 0;
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION | SYSCTL_PERIPH_GPIOD);
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_3);
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_3,0x08);

    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_1|GPIO_PIN_3);

    SpiSend(OP_DISPLAYTEST, DISPLAY_TEST_NORMAL_OPERATION);
    SpiSend(OP_INTENSITY,   0x01);
    SpiSend(OP_SCANLIMIT,   0x07);
    SpiSend(OP_DECODEMODE,  DECOMODE_NODECODE);
    SpiSend(OP_SHUTDOWN,    SHUTDOWN_NO);

    //Clean the display
    for( indexCounter = OP_DIGIT0; indexCounter <= OP_DIGIT7; indexCounter++)
    {
        SpiSend(indexCounter,    0x00);
    }
}

void cleanDisplay(void)
{
    // The functions asumes the operation mode for the display NODECODE
    // Otherwise its behavior will be unexpected
    SpiSend(OP_DECODEMODE,  DECOMODE_NODECODE);
    uint8_t  indexCounter = 0;
    for( indexCounter = OP_DIGIT0; indexCounter <= OP_DIGIT7; indexCounter++)
    {
        SpiSend(indexCounter,    0x00);
    }
}

//Display the string
void displayTempValue(const float tempValue)
{
    float floatValue = tempValue*FLOATING_POINT_MULTIPLIER;
    int  indexCounter = 0, auxValue = abs(floatValue), digitValue = 0 ;

    //Clean the display
    cleanDisplay();

    //Displays degree
    SpiSend(OP_DIGIT0, SEVEN_DIGIT_DISPLAY_C_LETTER_CL);
    SpiSend(OP_DIGIT1, SEVEN_DIGIT_DISPLAY_DEGREE);

    // Validates max possible value
    if(auxValue >= 1000000)
    {
        auxValue = 99999;
    }

    //Begin cycle to convert the int value to array
    /* This do-while covers 3 possible cases
     * 1. When the value is different from 0 the cycle does its routine at least two times.
     * 2. When the value is equal to cero it displays 0.0
     * 3. When the value is above 5 digits it does not displays everything
     */
    indexCounter = OP_DIGIT2;
    do
    {
        digitValue = auxValue % 10 ;
        auxValue = (auxValue - digitValue)/10;
        if(indexCounter == OP_DIGIT3)
        {
            // Adds the decimal point to the value
            SpiSend(indexCounter, sevenDigitDisplayMatrix[digitValue] | 0x80);
        }
        else
        {
            SpiSend(indexCounter, sevenDigitDisplayMatrix[digitValue]);
        }
        indexCounter++;
    }while(auxValue > 0 && indexCounter < DISPLAY_MAX_LENGTH || indexCounter < OP_DIGIT4);
    //Obtain sign
    if(tempValue < 0)
    {
        SpiSend(indexCounter, SEVEN_DIGIT_DISPLAY_MINUS);
    }
}

//Display the string
void displayTime(const uint8_t hours, const uint8_t minutes)
{
    int  indexCounter = 0, auxHours = abs(hours), auxMinutes = abs(minutes), digitValue = 0, displayCounter = 0;
    static bool ampmFlag = false;
    //Clean the display
    cleanDisplay();

    // Max possible hour
    if(auxHours < HOUR_FORMAT_12)
    {
        ampmFlag = false;
    }
    else
    {
        if(auxHours >= HOUR_FORMAT_12 && auxHours != BLANK_VALUE_DISPLAY)
        {
            auxHours = auxHours - 12;
            ampmFlag = true;
        }
        else
        {
            if(auxHours >= HOUR_FORMAT_24 && auxHours != BLANK_VALUE_DISPLAY)
            {
                auxHours = 0;
            }
        }
    }


    //Max possible minutes
    if(auxMinutes >= MINUTES_MAX_VALUE && auxMinutes != BLANK_VALUE_DISPLAY)
    {
        auxMinutes = 0;
    }

    // Displays minutes if the value is different of 99
    if(auxMinutes != BLANK_VALUE_DISPLAY)
    {
        indexCounter = OP_DIGIT3;
        // Displays minutes in two cycles
        for( displayCounter = 0; displayCounter < 2; displayCounter++)
        {
            digitValue = auxMinutes % 10;
            auxMinutes = (auxMinutes - digitValue)/10;
            SpiSend(indexCounter, sevenDigitDisplayMatrix[digitValue]);
            indexCounter++;
        }
    }
    // Displays hours if the value is different of 99
    if(auxHours != BLANK_VALUE_DISPLAY)
    {
        indexCounter = OP_DIGIT5;
        // Displays hours in maybe two cycles
        digitValue = auxHours % 10;
        auxHours   = (auxHours - digitValue)/10;
        SpiSend(indexCounter, sevenDigitDisplayMatrix[digitValue] | 0x80);
        indexCounter++;
        if(auxHours > 0 )
        {
            digitValue = auxHours % 10;
            auxHours   = (auxHours - digitValue)/10;
            SpiSend(indexCounter, sevenDigitDisplayMatrix[digitValue]);
        }
    }

    // Displays an h at the end to indicate time
    SpiSend(OP_DIGIT0, SEVEN_DIGIT_DISPLAY_H_LETTER_LC);
    if(ampmFlag)
    {
        SpiSend(OP_DIGIT1, SEVEN_DIGIT_DISPLAY_DEGREE);
    }
}

void displayTest(void)
{
    // Starts turning on all digits
    uint8_t  indexCounter = 0, timerCount = 0;
    uint8_t uiDigit0 = 0;
    uint8_t uiDigit1 = 1;
    uint8_t uiDigit2 = 2;
    uint8_t uiDigit3 = 3;
    uint8_t uiDigit4 = 4;
    uint8_t uiDigit5 = 5;
    uint8_t uiDigit6 = 6;
    uint8_t uiDigit7 = 7;

    for(timerCount = 0; timerCount < 5; timerCount++)
    {
        cleanDisplay();
        SysCtlDelay(20000000);
        for(indexCounter = OP_DIGIT0; indexCounter <= OP_DIGIT7; indexCounter++)
        {
            SpiSend(indexCounter, sevenDigitDisplayMatrix[0x08] | 0x80);
        }
        SysCtlDelay(20000000);
    }

    cleanDisplay();

    SpiSend(OP_DECODEMODE,  DECOMODE_CODEB70);
    for(indexCounter = 0; indexCounter <= 16; indexCounter++)
    {
        SpiSend(OP_DIGIT0, uiDigit0++);
        SpiSend(OP_DIGIT1, uiDigit1++);
        SpiSend(OP_DIGIT2, uiDigit2++);
        SpiSend(OP_DIGIT3, uiDigit3++);
        SpiSend(OP_DIGIT4, uiDigit4++);
        SpiSend(OP_DIGIT5, uiDigit5++);
        SpiSend(OP_DIGIT6, uiDigit6++);
        SpiSend(OP_DIGIT7, uiDigit7++);
        SysCtlDelay(20000000);
    }
    SpiSend(OP_DECODEMODE,  DECOMODE_NODECODE);
    cleanDisplay();

}

// End of file
