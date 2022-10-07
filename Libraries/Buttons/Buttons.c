/* Project Documentation
  @Company
    Universidad Tecnologica de Queretaro - Visteon

  @Project Name
  ProyetoFinalDisplayV4

  @File Name
    Buttons.c

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
    YOUR ACCEPTANCE OF THESE TERMS..
*/

#include "Buttons.h"

void initButtons(void)
{
    /* ***********************************
     *              SWITCHES Config
     * ***********************************
     */
    /* Enable the GPIOJ peripheral for Switch 1 PJ0 & Switch 2 PJ1*/
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    /* Setup GPIO J0 as input*/
    GPIODirModeSet(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_DIR_MODE_IN);

    /* Set GPIO Pin 0 as a pull-up resistor */
    GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    /* Interrupt type RISING */
    GPIOIntTypeSet(GPIO_PORTJ_BASE, GPIO_INT_PIN_0, GPIO_RISING_EDGE | GPIO_DISCRETE_INT);
    //GPIOIntTypeSet(GPIO_PORTP_BASE, GPIO_INT_PIN_1, GPIO_RISING_EDGE | GPIO_DISCRETE_INT);

    /* Enable Interrupts */
    GPIOIntEnable(GPIO_PORTJ_BASE, GPIO_INT_PIN_0);
    //GPIOIntEnable(GPIO_PORTJ_BASE, GPIO_INT_PIN_1);

}

void initTimers(void)
{
    /* ***********************************
     *              TIMER1 Config
     * ***********************************
     */
    /* Enable the peripheral TIMER1 */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    /* Enable processor interrupts */
    IntMasterEnable();
    /* Configure the 32 bit periodic timer */
    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
    /* Trigger interrupt each second */
    TimerLoadSet(TIMER1_BASE, TIMER_A, SYSCLOCK_SPEED / TIME_UNIT_SECONDS);
    /* Setup the interrupts for the timer interrupts */
    IntEnable(INT_TIMER1A);
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    /* Enable the timer */
    TimerEnable(TIMER1_BASE, TIMER_A);

    /* ***********************************
     *              TIMER2 Config
     * ***********************************
     */
    /* Enable the peripheral TIMER0 */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
    /* Enable processor interrupts */
    IntMasterEnable();
    /* Configure the 32 bit periodic timer */
    TimerConfigure(TIMER2_BASE, TIMER_CFG_PERIODIC);
    /* Trigger interrupt each second */
    TimerLoadSet(TIMER2_BASE, TIMER_A, SYSCLOCK_SPEED / TIME_UNIT_100_MILLISECONDS);
    /* Setup the interrupts for the timer interrupts */
    IntEnable(INT_TIMER2A);
    TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
    /* Enable the timer */
    TimerEnable(TIMER2_BASE, TIMER_A);
}

int stateMachineDecodeFxn()
{
    // Static variables as this function is called multiple times
    // Variables for the states of the pressed button
    static bool buttonPressed = false, buttonReleased = false;
    // Variable that saves the time of the pressed button
    static uint16_t u16PressedButtonTime = 0;
    // Variable that saves the time when the micro enters the STATE_DISPLAY_CLOCK
    static uint16_t u16TimeCountDisplay = 0;
    // Variable that returns the state of the micro
    static int actualState = STATE_TEMPERATURE_SENSOR;

    if (GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0) == 0)
    {
        u16PressedButtonTime = u16PressedButtonTime + 1;
        buttonPressed        = true;
    }
    else
    {
        // Condition added to mitigate the bounce effect approx 10ms
        if(u16PressedButtonTime >= DEBOUNCE_TIME ){
            buttonReleased       = true;
        }
    }
    // if the button is pressed and released
    if(buttonPressed && buttonReleased)
    {
        // Clean button state flags
        buttonPressed  = false;
        buttonReleased = false;
        // Decodes the state of the micro based on the pressed time
        // if time is between 10ms & 900ms
        if( (actualState == STATE_TEMPERATURE_SENSOR) && (DISPLAY_CLOCK_MINIMUM_TIME <= u16PressedButtonTime && DISPLAY_CLOCK_MAXIMUM_TIME >= u16PressedButtonTime) )
        {
            actualState = STATE_DISPLAY_CLOCK;
        }
        else
        {
            // if time is between 1000ms & 3000ms
            if( (actualState == STATE_TEMPERATURE_SENSOR) && (CONFIG_CLOCK_MINIMUM_TIME <= u16PressedButtonTime && CONFIG_CLOCK_MAXIMUM_TIME >= u16PressedButtonTime) )
            {
                actualState = STATE_CONFIGURE_CLOCK_HOURS;
            }
            else
            {
                // if time is between 10ms & 500ms
                if( (actualState == STATE_CONFIGURE_CLOCK_HOURS) && (CONFIG_CLOCK_CHANGE_STATE_MIN <= u16PressedButtonTime && CONFIG_CLOCK_CHANGE_STATE_MAX >= u16PressedButtonTime) )
                {
                    actualState = STATE_CONFIGURE_CLOCK_MINUTES;
                }
                else
                {
                    // if time is between 10ms & 500ms
                    if( (actualState == STATE_CONFIGURE_CLOCK_MINUTES) && (CONFIG_CLOCK_CHANGE_STATE_MIN <= u16PressedButtonTime && CONFIG_CLOCK_CHANGE_STATE_MAX >= u16PressedButtonTime) )
                    {
                        actualState = STATE_TEMPERATURE_SENSOR;
                    }
                }
            }
        }
        u16PressedButtonTime = 0;
    }
    // Time counter up to 3000ms
    if(actualState == STATE_DISPLAY_CLOCK)
    {
        u16TimeCountDisplay = u16TimeCountDisplay + 1;
        if(u16TimeCountDisplay >= DISPLAY_CLOCK_DISPLAY_MAX_TIME)
        {
            actualState = STATE_TEMPERATURE_SENSOR;
            u16TimeCountDisplay = 0;
        }
    }
    return actualState;
}

void digitalClockConfig(uint8_t* timeUnit, int state)
{
    // Static variables as this function is called multiple times
    // Variables for the states of the pressed button
    static bool buttonPressed = false, buttonReleased = false;
    // Variable that saves the time of the pressed button
    static uint16_t u16PressedButtonTime = 0;

    if (GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_1) == 0)
    {
        u16PressedButtonTime = u16PressedButtonTime + 1;
        buttonPressed        = true;
    }
    else
    {
        // Condition added to mitigate the bounce effect approx 10ms
        if(u16PressedButtonTime >= DEBOUNCE_TIME ){
            buttonReleased       = true;
        }
    }
    // if the button is pressed and released
    if(buttonPressed && buttonReleased)
    {
        // Clean button state flags
        buttonPressed  = false;
        buttonReleased = false;
        if( CONFIG_CLOCK_CHANGE_STATE_MIN <= u16PressedButtonTime && CONFIG_CLOCK_CHANGE_STATE_MAX >= u16PressedButtonTime )
        {
            *timeUnit = *timeUnit + 1;
            if(STATE_CONFIGURE_CLOCK_HOURS == state && *timeUnit >= HOUR_FORMAT_24)
            {
                *timeUnit = 0;
            }
            else
            {
                if(STATE_CONFIGURE_CLOCK_MINUTES == state && *timeUnit >= MINUTES_MAX_VALUE)
                {
                    *timeUnit = 0;
                }
            }
        }
        u16PressedButtonTime = 0;
    }

}

// End of file
