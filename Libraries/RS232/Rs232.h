/* Project Documentation
  @Company
    Universidad Tecnologica de Queretaro - Visteon

  @Project Name
  ProyetoFinalDisplayV4

  @File Name
    Rs232.h

  @Author
    Romero, Rodrigo
    Olvera, Diego
    Perches, Gibran

  @Summary
    This is the a header file for C language. The purpose is to
    initialize all UART libraries and functions.

  @Date
  07/10/2022

  @Version
  V4.0

  @Description
    Using the serial ptrotocol UART, the micro is going to send the information
    obtained from the temperature sensor. The speed is going to be 115 200 bits/s


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

#ifndef LIBRARIES_RS232_RS232_H_
#define LIBRARIES_RS232_RS232_H_

/* *****************************************************************
 *                        Included files
 * *****************************************************************
 */
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

/* *****************************************************************
 *                        Macros
 * *****************************************************************
 */

#undef SYSCLOCK_SPEED
#define SYSCLOCK_SPEED (uint32_t) 120000000

#define UART_SPEED 115200

#define TEMPERATURE_START_LABEL "No. "
#define TEMPERATURE_MID_LABEL   "Temperatura"
#define TEMPERATURE_UNIT_CHAR   "°C"
#define ZERO_CHAR_VALUE         (uint8_t)48
#define SINGLE_POINT_CHAR       '.'
#define MINUS_CHAR              '-'
#define SPACE_CHAR              ' '
#define TWO_POINTS_CHAR         ':'
#define CAR_RETURN_CHAR         13
#define LINE_JUMP_CHAR          10

#define NUMBER_MAX_STRING_LENGTH (uint8_t)10

#define DECIMAL_POINT_POSITION  1
#define MINIMUM_LENGTH_POSITION 1
#define INT_STATE               0
#define FLOAT_STATE             1

#define FLOATING_POINT_MULTIPLIER     10
#define FLOATING_POINT_MINIMUM_LENGTH 3

/* *****************************************************************
 *                       Function prototypes
 * *****************************************************************
 */

/**
*****************************************************************************
*
*  @brief Initialize all related registers to use the serial protocol UART
*
*  @b example
*  @code
*      void initRs232();
*  @endcode
*
*  @note
*      N/A
*
******************************************************************************
*/

void initRs232(void);

/**
*****************************************************************************
*
*  @brief Sends the external temperature value trough the serial protocol
*
*  @param[in]   tempValue     Value to be send
*
*  @b example
*  @code
*      void sendTemperatureValueUart();
*  @endcode
*
*  @note
*      N/A
*
******************************************************************************
*/

void sendTemperatureValueUart(const float tempValue);

/**
*****************************************************************************
*
*  @brief Sends the external temperature value trough the serial protocol
*
*  @param[in]   tempValue     Value to be displayed
*
*  @b example
*  @code
*      void displayIntValue();
*  @endcode
*
*  @note
*      N/A
*
******************************************************************************
*/

void displayIntValue(const int tempValue, const int state);



#endif /* LIBRARIES_RS232_RS232_H_ */
