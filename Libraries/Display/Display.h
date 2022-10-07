/* Project Documentation
  @Company
    Universidad Tecnologica de Queretaro - Visteon

  @Project Name
  ProyetoFinalDisplayV4

  @File Name
    Display.h

  @Author
    Romero, Rodrigo
    Olvera, Diego
    Perches, Gibran

  @Summary
    This is the a header file for C language. The purpose is to
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

#ifndef LIBRARIES_DISPLAY_H_
#define LIBRARIES_DISPLAY_H_

/* *****************************************************************
 *                        Included files
 * *****************************************************************
 */
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "SevenDigitMacros.h"
/* *****************************************************************
 *                        Macros
 * *****************************************************************
 */
// Values defined by the datasheet to send values to the display
#define OP_NOOP        (uint8_t)0x00
#define OP_DIGIT0      (uint8_t)0x01
#define OP_DIGIT1      (uint8_t)0x02
#define OP_DIGIT2      (uint8_t)0x03
#define OP_DIGIT3      (uint8_t)0x04
#define OP_DIGIT4      (uint8_t)0x05
#define OP_DIGIT5      (uint8_t)0x06
#define OP_DIGIT6      (uint8_t)0x07
#define OP_DIGIT7      (uint8_t)0x08
#define OP_DECODEMODE  (uint8_t)0x09
#define OP_INTENSITY   (uint8_t)0x0A
#define OP_SCANLIMIT   (uint8_t)0x0B
#define OP_SHUTDOWN    (uint8_t)0x0C
#define OP_DISPLAYTEST (uint8_t)0x0F

// Shutdown mode table 3
#define SHUTDOWN_SM    (uint8_t)0x00 //(12)+(00) Shutdown mode
#define SHUTDOWN_NO    (uint8_t)0x01 //(12)+(01) Normal Operation

// Decode mode table 4
#define DECOMODE_NODECODE (uint8_t)0x00 //No decode for digits 7-0
#define DECOMODE_CODEB0   (uint8_t)0x01 //Code B decode for digit 0, No decode for digits 7-1
#define DECOMODE_CODEB30  (uint8_t)0x0F //Code B decode for digits 3-0, No decode for digits 7-4
#define DECOMODE_CODEB70  (uint8_t)0xFF //Code B decode for digits 7-0

#define DISPLAY_TEST_NORMAL_OPERATION (uint8_t)0x00
#define DISPLAY_TEST_MODE             (uint8_t)0x01

#undef BLANK_VALUE_DISPLAY
#define BLANK_VALUE_DISPLAY 99

#define DISPLAY_MAX_LENGTH 8

#undef HOUR_FORMAT_12
#define HOUR_FORMAT_12 13

#undef HOUR_FORMAT_24
#define HOUR_FORMAT_24 24

#undef MINUTES_MAX_VALUE
#define MINUTES_MAX_VALUE 60

#define FLOATING_POINT_MULTIPLIER 10

/* *****************************************************************
 *                       Function prototypes
 * *****************************************************************
 */

/**
*****************************************************************************
*
*  @brief Initialize all related registers to use the 7-digit display
*         with the SPI protocol
*
*  @b example
*  @code
*      void initDisplay();
*  @endcode
*
*  @note
*      N/A
*
******************************************************************************
*/

void initDisplay(void);

/**
*****************************************************************************
*
*  @brief Use the SPI protocol to send the information provided
*
*  @param[in]    registerAddress   Register address where the information is going
*  @param[in]   registerValue     Value to be set in the register
*
*  @b example
*  @code
*      void  convertToStringValue((uint8_t)registerAddress, (uint8_t)registerValue);
*  @endcode
*
*  @note
*      N/A
*
******************************************************************************
*/

void SpiSend(uint8_t registerAddress, uint8_t registerValue);

/**
*****************************************************************************
*
*  @brief Cleans the display
*
*  @b example
*  @code
*      void initDisplay();
*  @endcode
*
*  @note
*      N/A
*
******************************************************************************
*/

static void cleanDisplay(void);

/**
*****************************************************************************
*
*  @brief Displays the temperature value
*
*  @param[in]    tempValue     Temperature value including decimal point
*
*  @b example
*  @code
*      void displayTempValue((int)tempValue);
*  @endcode
*
*  @note
*      The temperature value must be multiplied by 10 as the algorithm
*      includes the decimal point
*
******************************************************************************
*/



void displayTempValue(const float tempValue);

/**
*****************************************************************************
*
*  @brief Displays the temperature value
*
*  @param[in]    hours     Hours to be displayed
*  @param[in]    minutes   Minutes to be displayed
*
*  @b example
*  @code
*      void displayTime((int)hours, (int)minutes);
*  @endcode
*
*  @note
*      n/a
*
******************************************************************************
*/

void displayTime(const uint8_t hours, const uint8_t minutes);

/**
*****************************************************************************
*
*  @brief Test the display sending different predefined messages
*
*
*  @b example
*  @code
*      void displayTest();
*  @endcode
*
*  @note
*      n/a
*
******************************************************************************
*/

void displayTest(void);

#endif /* LIBRARIES_DISPLAY_H_ */
