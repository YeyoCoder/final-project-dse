/* Project Documentation
  @Company
    Universidad Tecnologica de Queretaro - Visteon

  @Project Name
  ProyetoFinalDisplayV4

  @File Name
    Sensor.c

  @Author
    Romero, Rodrigo
    Olvera, Diego
    Perches, Gibran

  @Summary
    The purpose of this C language file is to initialize, read, and write the
    temperature sensor BM280 and defines from Sensor header.

  @Date
  07/10/2022

  @Version
  V4.0

  @Description
    This source file is going to implement the reading and the states
    of temperature value for BM280 sensor, with defines in header Sensor.h.


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

#include "Sensor.h"

void initSensor(void)
{
    /* ***********************************
     *              I2C2 Config
     * ***********************************
     */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION));

    GPIOPinConfigure(GPIO_PN5_I2C2SCL);
    GPIOPinConfigure(GPIO_PN4_I2C2SDA);
    GPIOPinTypeI2C(GPIO_PORTN_BASE, GPIO_PIN_4);
    GPIOPinTypeI2CSCL(GPIO_PORTN_BASE, GPIO_PIN_5);

    // I2C Initialization
    SysCtlPeripheralDisable(SYSCTL_PERIPH_I2C2);
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C2);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C2);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_I2C2));
    I2CMasterInitExpClk(I2C2_BASE, SYSCLOCK_SPEED, true);
    I2CMasterIntEnableEx(I2C2_BASE, (I2C_MASTER_INT_STOP | I2C_MASTER_INT_NACK | I2C_MASTER_INT_DATA));
    IntEnable(INT_I2C2);
    while(I2CMasterBusBusy(I2C2_BASE));

    I2CMasterSlaveAddrSet(I2C2_BASE, SLAVE_ADDRESS_EXT, false);
    I2CMasterDataPut(I2C2_BASE, sensor_Parameter_Address);
    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_SEND);
}

void initADC0(void)
{
    /* ***********************************
     *              ADC0 Config
     * ***********************************
     */
    /* Enable the peripheral ADC0 */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    /* Configure the trigger mode and prority */
    ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);

    /* Configure the step */
    ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_TS);
    /* Last step */
    ADCSequenceStepConfigure(ADC0_BASE,1,3,ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);
    /* Enables conversion */
    ADCSequenceEnable(ADC0_BASE, 1);
}

double compensateTemperature(uint32_t sensor_Temperature, uint16_t sensor_Digit_T1, int16_t sensor_Digit_T2, int16_t sensor_Dig_T3)
{
    double aux1;
    double aux2;
    double return_temperature;
    double min_temp = -40;
    double max_temp = 85;

    aux1 = ((double)sensor_Temperature) / 16384.0 - ((double)sensor_Digit_T1) / 1024.0;
    aux1 = aux1 * ((double)sensor_Digit_T2);
    aux2 = (((double)sensor_Temperature) / 131072.0 - ((double)sensor_Digit_T1) / 8192.0);
    aux2 = (aux2 * aux2) * ((double)sensor_Dig_T3);
    return_temperature = (aux1 + aux2) / 5120.0  + sensor_Temperature_Compensation;

    if (return_temperature < min_temp){ return_temperature = min_temp; }
    else if (return_temperature > max_temp) { return_temperature = max_temp; }

    return return_temperature;
}

void temperatureSensor()
{
    uint32_t ui32I2CMasterInterruptStatus = I2CMasterIntStatusEx(I2C2_BASE, true);

    I2CMasterIntClearEx(I2C2_BASE, ui32I2CMasterInterruptStatus);

    switch(sensor_State)
    {
    case state_Read_Calibration:

        if(ui32I2CMasterInterruptStatus & I2C_MASTER_INT_NACK) { sensor_State = state_Error; }
        else {
            sensor_State = state_Write_Calibration;
            I2CMasterSlaveAddrSet(I2C2_BASE, SLAVE_ADDRESS_EXT, true);
            I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
        }

        break;

    case state_Write_Calibration:

        if(ui32I2CMasterInterruptStatus & I2C_MASTER_INT_NACK){ sensor_State = state_Error; }
        else {
            sensor_Parameter_Address++;
            sensor_Dig_Temperature[sensor_Parameter_Count] = I2CMasterDataGet(I2C2_BASE);
            if(sensor_Parameter_Count<5)  {
                sensor_State = state_Read_Calibration;
                sensor_Parameter_Count++;
                I2CMasterSlaveAddrSet(I2C2_BASE, SLAVE_ADDRESS_EXT, false);
                I2CMasterDataPut(I2C2_BASE, sensor_Parameter_Address);
                I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_SEND);
            } else {
                sensor_Digit_T1 = sensor_Dig_Temperature[1]<<8 | sensor_Dig_Temperature [0];
                sensor_Digit_T2 = sensor_Dig_Temperature[3]<<8 | sensor_Dig_Temperature [2];
                sensor_Digit_T3 = sensor_Dig_Temperature[5]<<8 | sensor_Dig_Temperature [4];

                sensor_State = state_Sensor_Configuration_Finished;

                I2CMasterSlaveAddrSet(I2C2_BASE, SLAVE_ADDRESS_EXT, false);
                I2CMasterDataPut(I2C2_BASE, REGISTER_MEASCTRL);
                I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_SEND_START);
            }
        }
        break;

    case state_Sensor_Configuration_Finished:
        sensor_State = state_Adc_Status_Configuration;
        I2CMasterDataPut(I2C2_BASE, sensor_Ctrl_Meas);
        I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
        break;

    case state_Adc_Status_Configuration:
        sensor_State = state_Adc_Status;
        I2CMasterSlaveAddrSet(I2C2_BASE, SLAVE_ADDRESS_EXT, false);
        I2CMasterDataPut(I2C2_BASE, REGISTER_STATUS);
        I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_SEND);
        break;

    case state_Adc_Status:
        sensor_State = state_Adc_Wait_For_Measure_Finish;
        I2CMasterSlaveAddrSet(I2C2_BASE, SLAVE_ADDRESS_EXT, true);
        I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
        break;

    case state_Adc_Wait_For_Measure_Finish :
        if(I2CMasterDataGet(I2C2_BASE) & 0x08 == 0x08) {
            sensor_State = state_Adc_Start_Reading;
            I2CMasterSlaveAddrSet(I2C2_BASE, SLAVE_ADDRESS_EXT, false);
            I2CMasterDataPut(I2C2_BASE, 0xF6);
            I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_SEND);
        } else {
            I2CMasterSlaveAddrSet(I2C2_BASE, SLAVE_ADDRESS_EXT, true);
            I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
        }
        break;

    case state_Adc_Start_Reading:

        if(ui32I2CMasterInterruptStatus & I2C_MASTER_INT_NACK){ sensor_State = state_Error; }
        else {
            sensor_State = state_Adc_Reading_In_Progress;
            I2CMasterSlaveAddrSet(I2C2_BASE, SLAVE_ADDRESS_EXT, true);
            I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
        }
        break;

    case state_Adc_Reading_In_Progress:
        sensor_Data_Counter++;
        if (sensor_Data_Counter == 5){
            sensor_ADC_Data_Temperature[0]   = I2CMasterDataGet(I2C2_BASE);
            sensor_ADC_Data_Temperature[0] <<= 8;
            I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
        } else if (sensor_Data_Counter == 6) {
            sensor_ADC_Data_Temperature[1] =   I2CMasterDataGet(I2C2_BASE);
            sensor_Data_Counter            =   ZERO;
            sensor_ADC_Temperature         =   sensor_ADC_Data_Temperature[0] | sensor_ADC_Data_Temperature[1];
            sensor_ADC_Temperature       <<=   4;
            sensor_Temperature             =   compensateTemperature(sensor_ADC_Temperature, sensor_Digit_T1, sensor_Digit_T2, sensor_Digit_T3);
            // Get integer values from current temperature to print
            sensor_Current_Temperature_Integer  = (uint8_t) (sensor_Temperature + 0.05);
            sensor_Current_Temperature_Fraction = (uint8_t) ((sensor_Temperature + 0.05) * 10);
            sensor_Current_Temperature_Fraction = sensor_Current_Temperature_Fraction - (sensor_Current_Temperature_Integer * 10);
            if (sensor_Current_Sample < TEMPERATURE_SAMPLES) { sensor_Temperature_Array[sensor_Current_Sample] = sensor_Temperature; }
            else {
                sensor_Current_Sample      = ZERO;
                sensor_Temperature_Average = ZERO_FLOAT;
                for(sensor_Current_Sample = ZERO; sensor_Current_Sample < TEMPERATURE_SAMPLES; sensor_Current_Sample++){
                    sensor_Temperature_Average +=  sensor_Temperature_Array[sensor_Current_Sample];
                }
                sensor_Current_Sample      = ZERO;
                sensor_Temperature_Average = sensor_Temperature_Average / (double)TEMPERATURE_SAMPLES;
                // Update average temperature values to print
                sensor_Avg_Temperature_Integer  = (uint8_t) (sensor_Temperature_Average + 0.05);
                sensor_Avg_Temperature_Fraction = (uint8_t) ((sensor_Temperature_Average + 0.05) * 10);
                sensor_Avg_Temperature_Fraction = sensor_Avg_Temperature_Fraction - (sensor_Avg_Temperature_Integer * 10);
                if (sensor_Avg_Temperature_Fraction < 0) { sensor_Avg_Temperature_Fraction *= -1; }
                // Update Temperature Digits to be displayed
                sensor_Temperature_Units    = (uint8_t)sensor_Avg_Temperature_Integer % 10;
                sensor_Temperature_Tens     = sensor_Avg_Temperature_Integer / 10;
                sensor_Temperature_Decimals = sensor_Avg_Temperature_Fraction;
                //Store current ADC reading so that it's not "wasted"
                sensor_Temperature_Array[sensor_Current_Sample] = sensor_Temperature;
            }
            sensor_Current_Sample++;
            if(sensor_First_Read_Flag) { sensor_First_Read_Flag = false; }
        } else {
            sensor_Unused_Param = I2CMasterDataGet(I2C2_BASE);
            I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
        }
        break;

    case state_Error:
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1);
        break;
    }
}

