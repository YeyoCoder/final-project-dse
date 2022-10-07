/**
 * Copyright (c) 2015, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/** Project Documentation
  @Company
    Universidad Tecnologica de Queretaro - Visteon

  @Project Name
  ProyetoFinalDisplayV4

  @File Name
    PFinalMain.c

  @Author
    Romero, Rodrigo
    Olvera, Diego
    Perches, Gibran

  @Summary
    This is a C language file with the purpose to run the main code
    of the project to resolve the client issue.

  @Date
  07/10/2022

  @Version
  V4.0

  @Description
    This file have an state machine to manage diferent task and configurations for temperature
    sensor, display the clock, configure the clock hours and the clock minutes, with two timers
    one for the temperature sensor sampling and the other one to enter to configure the clock,
    also have I2C2 comunication with the sensor BM280 to get temperature value.


    Generation Information :
        Product Revision  :  TM4C129EXL - 1.168.0
        Device            :  TM4C129ENCPDT
    The generated drivers are tested against the following:
        Languaje          :  C ANSI C 89 Mode
        Compiler          :  TIv16.9.6LTS
        CCS               :  CCS v7.4.0.00015
*/

/**
    (c) 2022 Romero Rodrigo, OlveraDiego, Perches Gibran. You may use this
    software and any derivatives exclusively with Texas Instruments products.

    THIS SOFTWARE IS SUPPLIED BY Romero Rodrigo, Olvera Diego, Perches Gibran "AS IS".
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING
    ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH TEXAS INSTRUMENTS PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    Romero Rodrigo, Olvera Diego, Perches Gibran PROVIDES THIS SOFTWARE CONDITIONALLY UPON
    YOUR ACCEPTANCE OF THESE TERMS..
*/

/**
 *  ======== PFinalMain.c ========
 */

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Log.h>                //needed for any Log_info() call
#include <xdc/cfg/global.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/hal/Hwi.h>
#include <inc/hw_ints.h>

#include <ti/drivers/GPIO.h>


#include "Libraries/Display/Display.h"
#include "Libraries/Buttons/Buttons.h"
#include "Libraries/Rs232/Rs232.h"
#include "Libraries/Sensor/Sensor.h"

/* Board Header file */
#include "Board.h"

/**
 * *****************************************************************
 *                        Macros
 * *****************************************************************
 */

#define TASKSTACKSIZE   512
#define BLANK_VALUE_DISPLAY 99

#undef TASK_DURATION_MILLISECONDS
#define TASK_DURATION_MILLISECONDS 10

#define COUNT_TO_SECOND 1000/TASK_DURATION_MILLISECONDS

#undef HOUR_FORMAT_24
#define HOUR_FORMAT_24 24

#undef MINUTES_MAX_VALUE
#define MINUTES_MAX_VALUE 60

#undef SECONDS_MAX_VALUE
#define SECONDS_MAX_VALUE 60

Task_Struct taskSMStruct, taskADCStruct;
Char taskSMStack[TASKSTACKSIZE];
Char taskADCStack[TASKSTACKSIZE];
Hwi_Handle timer1AHwi, timer2AHwi, I2C2Hwi;

/**
*****************************************************************************
*
*  @brief Main task that contains the state machine
*
*  @b example
*  @code
*      N/A
*  @endcode
*
*  @note
*      N/A
*
******************************************************************************
*/
Void stateMachine(UArg arg0, UArg arg1);

/**
*****************************************************************************
*
*  @brief Task to obtain the internal temperature of the micro
*
*  @b example
*  @code
*      N/A
*  @endcode
*
*  @note
*      N/A
*
******************************************************************************
*/
Void ADC0Fxn(UArg arg0, UArg arg1);

/**
*****************************************************************************
*
*  @brief Interrupt routine for the Timer1A
*
*  @b example
*  @code
*      N/A
*  @endcode
*
*  @note
*      N/A
*
******************************************************************************
*/
Void timer1AHwInt(UArg arg);

/**
*****************************************************************************
*
*  @brief Interrupt routine for the Timer2A
*
*  @b example
*  @code
*      N/A
*  @endcode
*
*  @note
*      N/A
*
******************************************************************************
*/
Void timer2AHwInt(UArg arg);

/**
*****************************************************************************
*
*  @brief Interrupt routine for the I2C2 module
*
*  @b example
*  @code
*      N/A
*  @endcode
*
*  @note
*      N/A
*
******************************************************************************
*/
Void i2c2Hwi(UArg arg);

/*
 * Global variables
 */

uint8_t  ui8Hour    = 23; /* Hours, range 0 to 23 */
uint8_t  ui8Minutes = 59; /* Minutes, range 0 to 59 */
uint8_t  ui8Seconds = 59; /* Seconds, range 0 to 59 */
uint16_t u16MillisecondsCount = 0;

bool     sensor_First_Read_Flag              = true;
uint8_t  sensor_Temperature_Units            = ZERO;
uint8_t  sensor_Temperature_Tens             = ZERO;
uint8_t  sensor_Temperature_Decimals         = ZERO;
uint8_t  sensor_Ctrl_Meas                    = MODE_FORCED | T_OS_1;
uint8_t  sensor_Parameter_Address            = PARAM_DIG_T1_LSB;
uint8_t  sensor_Parameter_Count              = ZERO;
uint8_t  sensor_Data_Counter                 = ZERO;
uint32_t sensor_ADC_Temperature              = ZERO;
uint16_t sensor_Digit_T1                     = ZERO;
int16_t  sensor_Digit_T2                     = ZERO;
int16_t  sensor_Digit_T3                     = ZERO;
uint16_t sensor_Unused_Param                 = ZERO;
double   sensor_Temperature                  = ZERO_FLOAT;
double   sensor_Temperature_Average          = ZERO_FLOAT;
int8_t   sensor_Avg_Temperature_Integer      = ZERO;
int8_t   sensor_Avg_Temperature_Fraction     = ZERO;
int8_t   sensor_Current_Temperature_Integer  = ZERO;
int8_t   sensor_Current_Temperature_Fraction = ZERO;
uint8_t  sensor_Current_Sample               = ZERO;

float                   sensor_Temperature_Array_ADC[TEMPERATURE_SAMPLES]  = { ZERO_FLOAT };
double                  sensor_Temperature_Array[TEMPERATURE_SAMPLES]      = { ZERO_FLOAT };
double                  sensor_Temperature_Compensation                    = 3;
volatile uint16_t       sensor_ADC_Data_Temperature[2]                     = { ZERO };
uint8_t                 sensor_Dig_Temperature [6]                         = { ZERO };
BM280State              sensor_State                                       = state_Read_Calibration;
DetaTemperatureStatus   sensor_Delta_Temperature                           = data_Temp_Normal;

/*
 *  ======== stateMachine ========
 */



Void stateMachine(UArg arg0, UArg arg1)
{
    int microState = 0;
    int timeCount = 0;
    float temperature = 0.0;
    while (1)
    {
        Task_sleep((unsigned int)arg0);
        temperature = (float)sensor_Temperature_Average;
        microState = stateMachineDecodeFxn();
        switch(microState)
        {
        case STATE_TEMPERATURE_SENSOR:
            displayTempValue(temperature);
            break;
        case STATE_DISPLAY_CLOCK:
            displayTime(ui8Hour, ui8Minutes);
            break;
        case STATE_CONFIGURE_CLOCK_HOURS:
            timeCount = timeCount + 1;
            digitalClockConfig(&ui8Hour, STATE_CONFIGURE_CLOCK_HOURS);
            // Blinks to indicate hour position
            if(timeCount >= COUNT_TO_SECOND)
            {
                // Restart count
                timeCount = 0;
            }
            else
            {
                if(timeCount >= COUNT_TO_SECOND/2)
                {
                    // Turns off the hours value
                    displayTime(BLANK_VALUE_DISPLAY, ui8Minutes);
                }
                else
                {
                    // Turns on the hours value
                    displayTime(ui8Hour, ui8Minutes);
                }
            }
            break;
        case STATE_CONFIGURE_CLOCK_MINUTES:
            timeCount = timeCount + 1;
            digitalClockConfig(&ui8Minutes, STATE_CONFIGURE_CLOCK_MINUTES);
            // Blinks to indicate minutes position
            if(timeCount >= COUNT_TO_SECOND)
            {
                timeCount = 0;
            }
            else
            {
                if(timeCount >= COUNT_TO_SECOND/2)
                {
                    // Turns off the minutes value
                    displayTime(ui8Hour, BLANK_VALUE_DISPLAY);
                }
                else
                {
                    // Turns on the minutes value
                    displayTime(ui8Hour, ui8Minutes);
                }
            }
            break;
        default:
            displayTempValue(temperature);
            break;
        }
    }
}

Void ADC0Fxn(UArg arg0, UArg arg1)
{
    /* Main temperature */
    float ADCTemperatureValue = 0.0;
    /* Array to store ADC values sampled */
    uint32_t ui32ADC0Value[4];
    /* Array to store ADC values sampled */

    while (1)
    {
        Task_sleep((unsigned int)arg0);
        /* Cleans int for ADC0 */
        ADCIntClear(ADC0_BASE, 1);

        /* Triggers conversion*/
        ADCProcessorTrigger(ADC0_BASE, 1);

        /* Waits for the conversion to end */
        while(!ADCIntStatus(ADC0_BASE, 1, false));

        /* Saves the value of the conversion */
        ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);

        /* Average value from sequencers */
        ADCTemperatureValue = (float)(ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;
        /* Converts the value from digital to analog */
        ADCTemperatureValue = (1475 - ((2475 * ADCTemperatureValue)) / 4096)/10;

    }
}

Void timer1AHwInt(UArg arg)
{
    // Interruption generated every 1 second
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    sendTemperatureValueUart((float)sensor_Temperature_Average);
    // 60 Seconds have passed
    ui8Seconds++;
    if(ui8Seconds >= SECONDS_MAX_VALUE)
    {
        ui8Seconds = 0;
        ui8Minutes++;
        // 60 Minutes have passed
        if(ui8Minutes >= MINUTES_MAX_VALUE)
        {
            ui8Minutes = 0;
            ui8Hour++;
            if(ui8Hour >= HOUR_FORMAT_24)
            {
                ui8Hour = 0;
            }
        }
    }
}

Void timer2AHwInt(UArg arg)
{
    // Interruption generated every 100 milliseconds
    TimerIntClear(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
    // Here goes the temperature value
    if(!sensor_First_Read_Flag)
    {
        sensor_State = state_Sensor_Configuration_Finished;

        I2CMasterSlaveAddrSet(I2C2_BASE, SLAVE_ADDRESS_EXT, false);
        I2CMasterDataPut(I2C2_BASE, REGISTER_MEASCTRL);
        I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_SEND_START);
        sensor_First_Read_Flag = true;
    }

}

Void i2c2Hwi(UArg arg)
{
    temperatureSensor();
}

inline void hwiCreation(void)
{
    Hwi_Params hwiParamsTimer1A, hwiParamsTimer2A, hwiParamsI2C2;
    Error_Block ebTimer1A, ebTimer2A, ebI2C2;

    /* Hardware interrupt for TIMER1A */
    Hwi_Params_init(&hwiParamsTimer1A);
    Error_init(&ebTimer1A);
    hwiParamsTimer1A.maskSetting = Hwi_MaskingOption_SELF;
    timer1AHwi = Hwi_create(INT_TIMER1A, timer1AHwInt, &hwiParamsTimer1A, &ebTimer1A);

    /* Hardware interrupt for TIMER2A */
    Hwi_Params_init(&hwiParamsTimer2A);
    Error_init(&ebTimer2A);
    hwiParamsTimer2A.maskSetting = Hwi_MaskingOption_SELF;
    timer2AHwi = Hwi_create(INT_TIMER2A, timer2AHwInt, &hwiParamsTimer2A, &ebTimer2A);

    /* Hardware interrupt for TIMER1A */
    Hwi_Params_init(&hwiParamsI2C2);
    Error_init(&ebI2C2);
    hwiParamsI2C2.maskSetting = Hwi_MaskingOption_SELF;
    I2C2Hwi = Hwi_create(INT_I2C2, i2c2Hwi, &hwiParamsI2C2, &ebI2C2);
}

/*
 *  ======== main ========
 */
int main(void)
{
    Task_Params taskParams;

    /* Hardware interrupt creation */
    hwiCreation();

    /* Call board init functions */
    Board_initGeneral();
    Board_initGPIO();

    /* Peripherals intialization */
    initDisplay();
    initButtons();
    initTimers();
    initRs232();
    initSensor();
    initADC0();

    /* Display Test before showing temperature */
    //displayTest();

    /* Construct stateMachine Task  thread */
    Task_Params_init(&taskParams);
    taskParams.arg0 = TASK_DURATION_MILLISECONDS;
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &taskSMStack;
    Task_construct(&taskSMStruct, (Task_FuncPtr)stateMachine, &taskParams, NULL);

    /* Construct stateMachine Task  thread */
    taskParams.arg0 = 1000;
    taskParams.stack = &taskADCStack;
    Task_construct(&taskADCStruct, (Task_FuncPtr)ADC0Fxn, &taskParams, NULL);


     /* Turn on user LED to indicates operation*/
    GPIO_write(Board_LED0, Board_LED_ON);

    /* SysMin will only print to the console when you call flush or exit */
    System_flush();

    /* Start BIOS */
    BIOS_start();

    return (0);
}
