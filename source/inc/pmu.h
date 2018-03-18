/*++

Copyright (c) 2012-2014 Razer Ltd.
All rights reserved. This program contains proprietary and confidential information.

Module Name:
  pmu.h

Compiler:
  Keil Software µVision v5.11, with NXP Semiconductor LPC11U2x proccessor support.

Abstract:


--*/
#ifndef __PMU_H_ 
#define __PMU_H_

#include "main.h"

#define TEST_DEEPPOWERDOWN      (0)

#define MCU_SLEEP               (0)
#define MCU_DEEP_SLEEP          (1)
#define MCU_POWER_DOWN          (2)

/* System Low Power ----------------------------------------------------------*/
#define NVIC_LP_SEVONPEND       (0x10)
#define NVIC_LP_SLEEPDEEP       (0x04)
#define NVIC_LP_SLEEPONEXIT     (0x02)

#define IRC_OUT_PD              (0x1<<0)  // 1
#define IRC_PD                  (0x1<<1)  // 1
#define FLASH_PD                (0x1<<2)  // 1
#define BOD_PD                  (0x1<<3)  // 1 
#define ADC_PD                  (0x1<<4)  // 1
#define SYS_OSC_PD              (0x1<<5)  // 1
#define WDT_OSC_PD              (0x1<<6)  // 1
#define SYS_PLL_PD              (0x1<<7)  // 1
#define USB_PLL_PD              (0x1<<8)  // 1
#define USB_PHY_PD              (0x1<<10) // 1

#define CLK_SYS                 (1 << 0)  // 1
#define CLK_ROM                 (1 << 1)  // 1
#define CLK_RAM                 (1 << 2)  // 1
#define CLK_FLASHREG            (1 << 3)  // 1
#define CLK_FLASHARRAY          (1 << 4)  // 1
#define CLK_I2C                 (1 << 5)  // 0
#define CLK_GPIO                (1 << 6)  // 1
#define CLK_T16B0               (1 << 7)  // 1
#define CLK_T16B1               (1 << 8)  // 1
#define CLK_T32B0               (1 << 9)  // 0
#define CLK_T32B1               (1 << 10) // 0 
#define CLK_SSP0                (1 << 11) // 0
#define CLK_UART                (1 << 12) // 0
#define CLK_ADC                 (1 << 13) // 0
#define CLK_USBREG              (1 << 14) // 1
#define CLK_WDT                 (1 << 15) // 1
#define CLK_IOCON               (1 << 16) // 1
#define CLK_SSP1				        (1 << 18) // 1
#define CLK_PINT                (1 << 19) // 0
#define CLK_G0INT               (1 << 23) // 0
#define CLK_G1INT               (1 << 24) // 0
#define CLK_USBRAM              (1 << 27) // 1


#define WAKEUP_LFT              ((U32)1 << 23)            // PIO0_23, Left button
#define WAKEUP_RGT              ((U32)1 << 13)            // PIO1_13,  Right button
#define WAKEUP_MID              ((U32)1 << 16)            // PIO0_16, Middle button
#define WAKEUP_FWD              ((U32)1 << 13)            // PIO0_13, forward button
#define WAKEUP_BCK              ((U32)1 << 30)            // PIO1_30, backward button
#define WAKEUP_RFWD							((U32)1 << 12)            // PIO0_12, R forward button
#define WAKEUP_RBCK							((U32)1 << 11)            // PIO0_11, R backward button
#define WAKEUP_DPIUP            ((U32)1 << 29)            // PIO1_29, R backward button   
#define WAKEUP_DPIDOWN          ((U32)1 << 22)            // PIO0_22, R backward button

#define WAKEUP_MOTION           ((U32)1 << 20)            // PIO0_20, Sensor motion IRQ 

#define WAKEUP_PORT0            (WAKEUP_LFT | WAKEUP_MID | WAKEUP_RFWD | WAKEUP_RBCK|WAKEUP_MOTION|WAKEUP_DPIDOWN|WAKEUP_FWD)
#define WAKEUP_PORT1            (WAKEUP_RGT | WAKEUP_BCK | WAKEUP_DPIUP)


/* GPIO interrupt for handling remote wake-up */


void PMU_Init(void);
void PMU_Sleep(void);
void pmu_RemoteWakeup(U8 state);

#endif

