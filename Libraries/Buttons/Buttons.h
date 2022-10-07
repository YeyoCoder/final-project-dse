/* Project Documentation
  @Company
    Universidad Tecnologica de Queretaro - Visteon

  @Project Name
  ProyetoFinalDisplayV4

  @File Name
    Buttons.h

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
    This source file is going to implement the buttons module of the final project.
    The following list describes how the module works:
    - From 10 to 900 ms the module should display the current hour in the format hh:mm
    - From 1000 to 3000 the module will enter in a "Hour Adjust" mode


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


#ifndef LIBRARIES_BUTTONS_BUTTONS_H_
#define LIBRARIES_BUTTONS_BUTTONS_H_

/* *****************************************************************
 *                        Included files
 * *****************************************************************
 */
#include <stdbool.h>
#include <stdint.h>
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "inc/hw_types.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"

/* *****************************************************************
 *                        Macros
 * *****************************************************************
 */

// Define the processor speed
#undef SYSCLOCK_SPEED
#define SYSCLOCK_SPEED (uint32_t) 120000000
// Define minimum unit of time to be used
#define TIME_UNIT_SECONDS          1
#define TIME_UNIT_100_MILLISECONDS 100

#undef TASK_DURATION_MILLISECONDS
#define TASK_DURATION_MILLISECONDS 10
// Define the times when the buttons are pressed
#define DISPLAY_CLOCK_MINIMUM_TIME     10   / TASK_DURATION_MILLISECONDS
#define DISPLAY_CLOCK_MAXIMUM_TIME     900  / TASK_DURATION_MILLISECONDS
#define CONFIG_CLOCK_MINIMUM_TIME      1000 / TASK_DURATION_MILLISECONDS
#define CONFIG_CLOCK_MAXIMUM_TIME      3000 / TASK_DURATION_MILLISECONDS
#define CONFIG_CLOCK_CHANGE_STATE_MIN  10   / TASK_DURATION_MILLISECONDS
#define CONFIG_CLOCK_CHANGE_STATE_MAX  500  / TASK_DURATION_MILLISECONDS
#define DISPLAY_CLOCK_DISPLAY_MAX_TIME 3000 / TASK_DURATION_MILLISECONDS
#define DEBOUNCE_TIME                  10   / TASK_DURATION_MILLISECONDS

#undef HOUR_FORMAT_12
#define HOUR_FORMAT_12 13

#undef HOUR_FORMAT_24
#define HOUR_FORMAT_24 24

#undef MINUTES_MAX_VALUE
#define MINUTES_MAX_VALUE 60

/* *****************************************************************
 *                        Definitions
 * *****************************************************************
 */

enum StateMachineValue
{
    STATE_TEMPERATURE_SENSOR = 0,
    STATE_DISPLAY_CLOCK,
    STATE_CONFIGURE_CLOCK_HOURS,
    STATE_CONFIGURE_CLOCK_MINUTES
};

/* *****************************************************************
 *                       Function prototypes
 * *****************************************************************
 */

/**
*****************************************************************************
*
*  @brief Initialize all related registers to use the Port J0 & J1 or the
*         embedded buttons.
*
*  @b example
*  @code
*      void initButtons();
*  @endcode
*
*  @note
*      N/A
*
******************************************************************************
*/

void initButtons(void);

/**
*****************************************************************************
*
*  @brief Initialize all related registers to use the TIMER1A and TIMER2A
*
*  @b example
*  @code
*      void initTimers();
*  @endcode
*
*  @note
*      N/A
*
******************************************************************************
*/

void initTimers(void);

/**
*****************************************************************************
*
*  @brief Decodes the state of the micro based on the time that the Switch1
*         is pressed
*
*
*  @b example
*  @code
*      int state = stateMachineDecodeFxn(state);
*  @endcode
*
*  @note
*      N/A
*
******************************************************************************
*/

int stateMachineDecodeFxn(void);

/**
*****************************************************************************
*
*  @brief Enters the Digital Clock Config state, where the user can add time to
*         the global variables of hours and minutes
*
*
*  @param[in]    timeUnit   Memory location of the variable to be modified
*                           it could be hours or minutes
*  @param[in]    state      State to identify if the hours or minutes are
*                           added
*
*
*  @b example
*  @code
*      void digitalClockConfig(&timeUnit);
*  @endcode
*
*  @note
*      N/A
*
******************************************************************************
*/

void digitalClockConfig(uint8_t* timeUnit,const int state);



#endif /* LIBRARIES_BUTTONS_BUTTONS_H_ */
