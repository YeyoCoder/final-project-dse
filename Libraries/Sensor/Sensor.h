/* Project Documentation
  @Company
    Universidad Tecnologica de Queretaro - Visteon

  @Project Name
  ProyetoFinalDisplayV4

  @File Name
    Sensor.h

  @Author
    Romero, Rodrigo
    Olvera, Diego
    Perches, Gibran

  @Summary
    This is the a header file for C language. The purpose is to
    initialize all Sensor libraries and functions to utilize for temperature sensor.

  @Date
  07/10/2022

  @Version
  V4.0

  @Description
    Using the I2C libraries it will obtain the temperature value of the sensor


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

#ifndef LIBRARIES_SENSOR_SENSOR_H_
#define LIBRARIES_SENSOR_SENSOR_H_

/* *****************************************************************
 *                        Included files
 * *****************************************************************
 */
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_i2c.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/adc.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"

/* *****************************************************************
 *                        Macros
 * *****************************************************************
 */

// Define the processor speed
#undef SYSCLOCK_SPEED
#define SYSCLOCK_SPEED (uint32_t) 120000000

#define SLAVE_ADDRESS_EXT             0x76

#define REGISTER_ID 0xD0
#define REGISTER_RESET 0xE0
#define REGISTER_STATUS 0xF3
#define REGISTER_MEASCTRL 0xF4
#define REGISTER_CONFIG 0xF5
#define ADDRESS_READING_START 0xF7
#define ADDRESS_READ_TEMP_MSB 0xFA
#define ADDRESS_READ_TEMP_LSB 0xFB
#define ADDRESS_READ_TEMP_XLSB 0xFC

//Operation Modes
#define MODE_SLEEP 0x0
#define MODE_FORCED 0x1
#define MODE_NORMAL 0x3

//Standby Time Temperature
#define T_SB__5MS 0x00
#define T_SB_62_5MS 0x20
#define T_SB_125MS 0x40
#define T_SB_250MS 0x60
#define T_SB_500MS 0x80
#define T_SB_1000MS 0xA0
#define T_SB_2000MS 0xD0
#define T_SB_4000MS 0xE0

//Pressure Oversampling
#define P_OS_OFF 0x00
#define P_OS_1 0x04
#define P_OS_2 0x08
#define P_OS_4 0x0C
#define P_OS_8 0x10
#define P_OS_16 0x14

//Temperature Oversampling
#define T_OS_OFF 0x00
#define T_OS_1 0x20
#define T_OS_2 0x40
#define T_OS_4 0x60
#define T_OS_8 0x80
#define T_OS_16 0xA0

//IIR Filter
#define IIR_OFF 0x00
#define IIR_2 0x04
#define IIR_4 0x08
#define IIR_8 0x0C
#define IIR_16 0x10

//Compensation Parameters
#define PARAM_DIG_T1_MSB 0x89
#define PARAM_DIG_T1_LSB 0x88
#define PARAM_DIG_T2_MSB 0x8B
#define PARAM_DIG_T2_LSB 0x8A
#define PARAM_DIG_T3_MSB 0x8D
#define PARAM_DIG_T3_LSB 0x8C

#define SLAVE_ADDRESS_EXT             0x76
#define TEMPERATURE_SAMPLES           10
#define ZERO                          0
#define ZERO_FLOAT                    0.0
#define DELTA_TEMPERATURE_UPPER_LIMIT 15
#define DELTA_TEMPERATURE_LOWER_LIMIT 3
#define MAXIMUM_STABILIZING_SAMPLES   10
#define LOST_CONNECTION_LIMIT         10


/* *****************************************************************
 *                       Function variables
 * *****************************************************************
 */

typedef enum
{
    state_Read_Calibration              ,
    state_Write_Calibration             ,
    state_Sensor_Configuration_Finished ,
    state_Adc_Status_Configuration      ,
    state_Adc_Status                    ,
    state_Adc_Wait_For_Measure_Finish   ,
    state_Adc_Start_Reading             ,
    state_Adc_Reading_In_Progress       ,
    state_Error
}BM280State;

typedef enum
{
    data_Temp_Normal      ,
    data_Temp_Stabilizing ,
    data_Temp_Abnormal    ,
    data_Temp_LostConnection
}DetaTemperatureStatus;

extern bool     sensor_First_Read_Flag;
extern uint8_t  sensor_Temperature_Units            ;
extern uint8_t  sensor_Temperature_Tens             ;
extern uint8_t  sensor_Temperature_Decimals         ;
extern uint8_t  sensor_Ctrl_Meas                    ;
extern uint8_t  sensor_Parameter_Address            ;
extern uint8_t  sensor_Parameter_Count              ;
extern uint8_t  sensor_Data_Counter                 ;
extern uint8_t  sensor_Current_Sample               ;
extern int8_t   sensor_Avg_Temperature_Integer      ;
extern int8_t   sensor_Avg_Temperature_Fraction     ;
extern int8_t   sensor_Current_Temperature_Integer  ;
extern int8_t   sensor_Current_Temperature_Fraction ;
extern uint16_t sensor_Digit_T1                     ;
extern int16_t  sensor_Digit_T2                     ;
extern int16_t  sensor_Digit_T3                     ;
extern uint16_t sensor_Unused_Param                 ;
extern uint32_t sensor_ADC_Temperature              ;
extern double   sensor_Temperature                  ;
extern double   sensor_Temperature_Average          ;

extern uint8_t                 sensor_Dig_Temperature[6]                             ;
extern volatile uint16_t       sensor_ADC_Data_Temperature[2]                        ;
extern float                   sensor_Temperature_Array_ADC[TEMPERATURE_SAMPLES]     ;
extern double                  sensor_Temperature_Array[TEMPERATURE_SAMPLES]         ;
extern double                  sensor_Temperature_Compensation                       ;
extern BM280State              sensor_State                                          ;
extern DetaTemperatureStatus   sensor_Delta_Temperature                              ;

/* *****************************************************************
 *                       Function prototypes
 * *****************************************************************
 */

/**
*****************************************************************************
*
*  @brief Initialize all related registers to use in the Sensor
*
*  @b example
*  @code
*      void initSensor();
*  @endcode
*
*  @note
*      N/A
*
******************************************************************************
*/

void initSensor(void);

/**
*****************************************************************************
*
*  @brief Initialize all related registers to use in the ADC0
*
*  @b example
*  @code
*      void initSensor();
*  @endcode
*
*  @note
*      N/A
*
******************************************************************************
*/

void initADC0(void);

/**
*****************************************************************************
*
*  @brief Function of the Sensor BM280 with all the states to get the Temperature
*
*  @b example
*  @code
*      void temperatureSensor();
*  @endcode
*
*  @note
*      N/A
*
******************************************************************************
*/

void temperatureSensor(void);

/**
*****************************************************************************
*
*  @brief This function compensated the temperature captured by the I2C protocol based on a formula
*         provided by the manufacturer
*
*  @param[in]   sensor_Temperature    Temperature value obtained in ADC
*  @param[in]   sensor_Digit_T1       Compensation value 1 generated from calibration
*  @param[in]   sensor_Digit_T2       Compensation value 2 generated from calibration
*  @param[in]   sensor_Digit_T3       Compensation value 3 generated from calibration
*
*  @b example
*  @code
*      double compensateTemperature = CompensateTemperature(sensor_Temperature, sensor_Digit_T1, sensor_Digit_T2, sensor_Digit_T3);
*  @endcode
*
*  @note
*      N/A
*
******************************************************************************
*/

double compensateTemperature(uint32_t sensor_Temperature, uint16_t sensor_Digit_T1, int16_t sensor_Digit_T2, int16_t sensor_Digit_T3);

#endif /* LIBRARIES_SENSOR_SENSOR_H_ */
