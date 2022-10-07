/* Project Documentation
  @Company
    Universidad Tecnologica de Queretaro - Visteon

  @Project Name
  ProyetoFinalDisplayV4

  @File Name
    SevenDigitMacros.h

  @Author
    Romero, Rodrigo
    Olvera, Diego
    Perches, Gibran

  @Summary
    This is the a header file for C language. The purpose is to
    have a separate header file for the macros.

  @Date
  07/10/2022

  @Version
  V4.0

  @Description
    The macros here are going to be decoded as a 7-digit display
    Following the next codification

       A
       _
    F | | B
       -  G
    E | | C
       -
       D  .DP

    Where the order is: DP A B C D E F G in positive logic

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
#ifndef LIBRARIES_SEVENDIGITMACROS_H_
#define LIBRARIES_SEVENDIGITMACROS_H_

/* ***********************************************
 *        Add the necessary macros here
 * ***********************************************
 */

// Numbers
static const uint8_t sevenDigitDisplayMatrix[] = {0b01111110,  // Number 0
                                                  0b00110000,  // Number 1
                                                  0b01101101,  // Number 2
                                                  0b01111001,  // Number 3
                                                  0b00110011,  // Number 4
                                                  0b01011011,  // Number 5
                                                  0b01011111,  // Number 6
                                                  0b01110000,  // Number 7
                                                  0b01111111,  // Number 8
                                                  0b01110011}; // Number 9

//Letters
#define SEVEN_DIGIT_DISPLAY_C_LETTER_CL (uint8_t)0b01001110 // C
#define SEVEN_DIGIT_DISPLAY_H_LETTER_LC (uint8_t)0b00010111 // h

//Symbols
#define SEVEN_DIGIT_DISPLAY_DEGREE    (uint8_t)0b01100011
#define SEVEN_DIGIT_DISPLAY_MINUS     (uint8_t)0b00000001

#endif /* LIBRARIES_SEVENDIGITMACROS_H_ */
