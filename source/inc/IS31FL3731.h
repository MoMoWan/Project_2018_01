/*++

Copyright (c) 2009-2013 Razer Ltd.
All rights reserved. This program contains proprietary and confidential information.

Module Name:
  IS31FL3731.h

Compiler:
  Keil Software uVision v4.50, with NXP Semiconductor LPC11U23 proccessor support.

Abstract:
  This file contains the definitions for led controller IS31FL3731 registers, and others

--*/

#ifndef _IS31FL3731_H_
#define _IS31FL3731_H_

#define LED_CTLR1_ADDR       0xEE //0b11101110                   // I2C address, pin AD connect to Vcc
#define LED_CTLR2_ADDR       0xE8 //0b11101000                   // I2C address, pin AD connect to GND
#define LED_CTLR3_ADDR       0xEC //0b11101100                   // I2C address, pin AD connect to SDA
#define LED_CTLR4_ADDR       0xEA //0b11101010                   // I2C address, pin AD connect to SCL


#define MAX_LED_NUM         (144)                         // One chip support 144 led max

#define FRAME_NUM(x)        (x-1)
#define FRAME_NUM_9         (0x0B) 

// ----------------- Frame Registers --------------------------------
#define REG_FRAME_SEL        0xFD
#define REG_CTRL_S          (0x00)
#define REG_BLK_S           (0x12)
#define REG_PWM_S           (0x24)

// ----------------- Function Registers ------------------------------
#define REG_CFG            0x00                           // Register to Configure the operation mode
#define REG_DISPLAY        0x01                           // Register to Set the display frame in Picture Mode
#define REG_AUTOPLAY1      0x02                           // Register to Set the way of display in Auto Frame Play Mode
#define REG_AUTOPLAY2      0x03                           // Register to Set the delay time in Auto Frame Play Mode
#define REG_OPT            0x05                           // Register to Set the display option
#define REG_AUDIO          0x06                           // Register to Set audio synchronization function
#define REG_FRAME          0x07                           // (Read only)Register to Store the frame display information
#define REG_BREATH1        0x08                           // Register to Set fade in and fade out time for breath function
#define REG_BREATH2        0x09                           // Register to Set the breath function
#define REG_SHUT           0x0A                           // Register to sets software shutdown mode
#define REG_AGC            0x0B                           // Register to Set the AGC function and the audio gain.
#define REG_ADCRATE        0x0C                           // Register to Set the ADC sample rate of the input signal
#define REG_HALF_VCC			 0XC2                           // Register to set the output level to 1/2 vcc

#define LED_BUFFER_SIZE     (1124+2)

#endif

