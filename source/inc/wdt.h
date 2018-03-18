/*++

Copyright (c) 2012-2014 Razer Ltd.
All rights reserved. This program contains proprietary and confidential information.

Module Name:
  watchdog.h

Compiler:
  Keil Software µVision v5.11, with NXP Semiconductor LPC11U2x proccessor support.

Abstract:
  This file contains the definitions for watchdog.c, which provides the control 
  for the Watchdog Timer support.

--*/

#ifndef _WATCHDOG_TIMER_H_
#define _WATCHDOG_TIMER_H_

#include "main.h"

#define WDT_TIMEOUT             2000000                   // WDT Timeout value = 2.0s

#define WDT_WDMOD_WDEN          ((U32)(1 << 0))           // WDT interrupt enable bit
#define WDT_WDMOD_WDRESET       ((U32)(1 << 1))           // WDT interrupt enable bit
#define WDT_WDMOD_WDTOF         ((U32)(1 << 2))           // WDT timeout flag bit
#define WDT_WDMOD_WDINT         ((U32)(1 << 3))           // WDT timeout flag bit
#define WDT_WDMOD(n)            ((U32)(1 << 1))           // WDT Mode
#define WDT_uS_INDEX            ((U32)(1000000))          // Define divider index for microsecond (us)
#define WDT_TIMEOUT_MIN         ((U32)(0xFF))             // WDT Time out minimum value
#define WDT_TIMEOUT_MAX         ((U32)(0xFFFFFFFF))       // WDT Time out maximum value
#define WDT_WDMOD_MASK          (U8)(0x02)                // Watchdog mode register mask
#define WDT_WDTC_MASK           (U8)(0xFFFFFFFF)          // Watchdog timer constant register mask
#define WDT_WDFEED_MASK         (U8)(0x000000FF)          // Watchdog feed sequence register mask
#define WDT_WDCLKSEL_MASK       (U8)(0x03)                // Watchdog timer value register mask
#define WDT_WDCLKSEL_RC         (U8)(0x00)                // Clock selected from internal RC
#define WDT_WDCLKSEL_PCLK       (U8)(0x01)                // Clock selected from PCLK
#define WDT_WDCLKSEL_RTC        (U8)(0x02)                // Clock selected from external RTC
#define WDT_MODE_RESET          0x02


U8   wdt_setTimeout(U32 timeout);
void wdt_initialization(U32 mode);
void wdt_startTimer(U32 timeout);
void wdt_resetTimer(void);
void wdt_configuration(void);

#endif //  _WATCHDOG_TIMER_H_
