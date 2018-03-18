/*++

Copyright (c) 2009-2013 Razer Ltd.
All rights reserved. This program contains proprietary and confidential information.

Module Name:
  IS31FL3731.h

Compiler:
  Keil Software uVision v5.1.0.0, with NXP Semiconductor LPC11U23 proccessor support.

Abstract:
  This file contains the definitions for led controller IS31FL3236 registers, and others

--*/

#ifndef _IS31FL3199_H_
#define _IS31FL3199_H_



#define LED_CTLR1_ADDR       0xCE //0b11001110                   // I2C address, pin AD connect to Vcc
#define LED_CTLR2_ADDR       0xC8 //0b11001000                   // I2C address, pin AD connect to GND
#define LED_CTLR3_ADDR       0xCC //0b11001100                   // I2C address, pin AD connect to SDA
#define LED_CTLR4_ADDR       0xCA //0b11001010                   // I2C address, pin AD connect to SCL

#define REG_SHUTDOWN        (0x00)        // Register to Configure the operation mode
#define REG_LEDCTRL         (0x01)        // OUT1~ OUT6 enable bit
#define REG_CONFIGURE1      (0x03)        // Set operation mode
#define REG_CONFIGURE2      (0x04)        // Set output current and audio input gain
#define REG_RAMPMODE				(0x05)        // Set the ramping function mode
#define REG_BREATHMARK			(0x06)        // Set the breathing mark function        
#define REG_PWM             (0x07)        // 0x07~0x0C Contrl every pwm value
#define REG_PWMUPDATE       (0x10)        // Update the pwm register and ctrl register
#define REG_T0              (0x11)        // Set the T0 time(OUT1~OUT6)
#define REG_T1              (0x1A)        // Set the T1
#define REG_T2              (0x1B)        // Set the T2
#define REG_T3              (0x1C)        // Set the T3
#define REG_T4              (0x1D)        // Set the T4 time
#define REG_TIMERUPDATE     (0x26)				// Load time registers’ data
#define REG_RESET						(0xFF)        // Reset all registers into default value


#define NORMAL_OPERATION		(0X01)        // Normal operation
#define SHUTDOWN_MODE       (0X00)        // Shutdown mode

#define OUT_NUM              (6)          //Numbers of output

#endif

