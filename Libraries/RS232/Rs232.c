/* Project Documentation
  @Company
    Universidad Tecnologica de Queretaro - Visteon

  @Project Name
  ProyetoFinalDisplayV4

  @File Name
    Rs232.c

  @Author
    Romero, Rodrigo
    Olvera, Diego
    Perches, Gibran

  @Summary
    The purpose this C language file is to initialize, display and send
    the temperature value by UART protocol and defines from Rs232 header.

  @Date
  07/10/2022

  @Version
  V4.0

  @Description
    This source file is going to implement the communication between Tiva TM4C Series
    with the temperature sensor BM280, and also display the int value.


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

#include "Rs232.h"

void initRs232(void)
{
    // Enables UART0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    // PORTA of UART
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    // GPIOA1 equal to Rx
    GPIOPinConfigure(GPIO_PA0_U0RX);
    // GPIOA1 equal to Tx
    GPIOPinConfigure(GPIO_PA1_U0TX);

    //  Configures GPIOA0 & GPIOA1 to be able to send the information
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //  Configuration of the message to be send: 8bit word, 1bit of stop and parity none
    UARTConfigSetExpClk(UART0_BASE, SYSCLOCK_SPEED, UART_SPEED, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
}

void displayIntValue(const int tempValue, const int state)
{
    int  indexCounter = 0, auxValue = abs(tempValue), digitValue = 0, stringLength = MINIMUM_LENGTH_POSITION;
    char intValueString[NUMBER_MAX_STRING_LENGTH] = {0};
    if(tempValue < 0)
    {
        UARTCharPut(UART0_BASE, MINUS_CHAR);
    }
    //Changes the minimum lenth of the string
    if(state)
    {
        stringLength = FLOATING_POINT_MINIMUM_LENGTH;
    }
    // Fills the string and then it displays the values in order
    do
    {
        digitValue = auxValue % 10 ;
        auxValue = (auxValue - digitValue)/10;
        // Checks if the number is integer or float
        if(indexCounter == DECIMAL_POINT_POSITION && state)
        {
            // Adds the decimal point to the value
            intValueString[NUMBER_MAX_STRING_LENGTH-1-indexCounter] = SINGLE_POINT_CHAR;
            indexCounter++;
        }
        intValueString[NUMBER_MAX_STRING_LENGTH-1-indexCounter] = digitValue + ZERO_CHAR_VALUE;
        indexCounter++;
    }while(auxValue > 0 && indexCounter < NUMBER_MAX_STRING_LENGTH || indexCounter < stringLength);
    // Displays all values different from NULL
    for(indexCounter = 0; indexCounter < NUMBER_MAX_STRING_LENGTH; indexCounter++)
    {
        if( intValueString[indexCounter] != 0)
        {
            UARTCharPut(UART0_BASE, intValueString[indexCounter]);
        }
    }
}

void sendTemperatureValueUart(const float tempValue)
{
    float floatValue = tempValue * FLOATING_POINT_MULTIPLIER;
    static uint32_t labelNumber = 1;
    uint8_t indexCounter = 0;
    uint8_t labelSize = sizeof(TEMPERATURE_START_LABEL)/sizeof(char);

    //Sends initial label
    for(indexCounter = 0; indexCounter < labelSize; indexCounter++)
    {
        UARTCharPut(UART0_BASE, TEMPERATURE_START_LABEL[indexCounter]);
    }
    //Sends number of iteration
    displayIntValue(labelNumber, INT_STATE);
    UARTCharPut(UART0_BASE, SPACE_CHAR);

    // Sends "Temperature" label
    labelSize = sizeof(TEMPERATURE_MID_LABEL)/sizeof(char);
    for(indexCounter = 0; indexCounter < labelSize; indexCounter++)
    {
        UARTCharPut(UART0_BASE, TEMPERATURE_MID_LABEL[indexCounter]);
    }
    UARTCharPut(UART0_BASE, TWO_POINTS_CHAR);
    UARTCharPut(UART0_BASE, SPACE_CHAR);

    // Sends Temperature Value as float
    // The value must be pre-multiplied by 10
    displayIntValue((int)floatValue, FLOAT_STATE);

    UARTCharPut(UART0_BASE, SPACE_CHAR);

    // Sends the label of degree Celsius
    labelSize = sizeof(TEMPERATURE_UNIT_CHAR)/sizeof(char);
    for(indexCounter = 0; indexCounter < labelSize; indexCounter++)
    {
        UARTCharPut(UART0_BASE, TEMPERATURE_UNIT_CHAR[indexCounter]);
    }

    // Sends an enter for the next value
    UARTCharPut(UART0_BASE, CAR_RETURN_CHAR);
    UARTCharPut(UART0_BASE, LINE_JUMP_CHAR);

    labelNumber++;
}
// End of file
