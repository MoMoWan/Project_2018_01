/*++

Copyright (c) 2012-2014 Razer Ltd.
All rights reserved. This program contains proprietary and confidential information.

Module Name:
  gpio.h

Compiler:
  Keil Software µVision v5.11, with NXP Semiconductor LPC11U2x proccessor support.

Abstract:
  This file contains the definitions for main.c, which provides the overall structure
  along with the initialization procedures.

--*/

#ifndef _GPIO_H_ 
#define _GPIO_H_

#include "main.h"

#define PORT0		0
#define PORT1		1

#define GROUP0		0
#define GROUP1		1

#define CHANNEL0	0
#define CHANNEL1	1
#define CHANNEL2	2
#define CHANNEL3	3
#define CHANNEL4	4
#define CHANNEL5	5
#define CHANNEL6	6
#define CHANNEL7	7


void gpio_Direction      (U32 port, U32 bit, U32 direction);
void gpio_SetValue       (U32 port, U32 bit, U32 value);
void gpio_initialization(void);
void GPIOSetFlexInterrupt(U32 channelNum, U32 portNum, U32 bitPosi, U32 sense, U32 event);

#endif

