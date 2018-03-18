/*++

Copyright (c) 2012-2014 Razer Ltd.
All rights reserved. This program contains proprietary and confidential information.

Module Name:
  main_lpc11Uxx.c

Compiler:
  Keil Software µVision v5.11, with NXP Semiconductor LPC11U2x proccessor support.

Abstract:
  This file provides runtime configuration of the System Clock separate from Bootloader
  configuration.

--*/

#include <stdint.h>
#include "main.h"


void sys_initialization(void)
/*++

Function Description:
  This function initializes the system, configuring the microcontroller.
 
Arguments:
  NONE

Returns: 
  NONE

--*/
{

  U8 i;

  LPC_SYSCON->PDRUNCFG     &= ~(1 << 5);                  // Power-up System Osc
  LPC_SYSCON->SYSOSCCTRL    = 0x00000000;                 // SYSOSCCTRL_Val
  for (i = 0; i < 200; i++) __NOP();

  LPC_SYSCON->SYSPLLCLKSEL  = 0x00000001;                 // Select PLL Input (SYSPLLCLKSEL_Val)
  LPC_SYSCON->SYSPLLCLKUEN  = 0x01;                       // Update Clock Source
  LPC_SYSCON->SYSPLLCLKUEN  = 0x00;                       // Toggle Update Register
  LPC_SYSCON->SYSPLLCLKUEN  = 0x01;
  while (!(LPC_SYSCON->SYSPLLCLKUEN & 0x01));             // Wait Until Updated

  LPC_SYSCON->SYSPLLCTRL    = 0x00000023;                 // 48MHz (SYSPLLCTRL_Val)
  LPC_SYSCON->PDRUNCFG     &= ~(1 << 7);                  // Power-up SYSPLL
  while (!(LPC_SYSCON->SYSPLLSTAT & 0x01));               // Wait Until PLL Locked

  LPC_SYSCON->WDTOSCCTRL    = 0x00;
  LPC_SYSCON->PDRUNCFG     &= ~(1 << 6);                  // Power-up WDT Clock       */
  for (i = 0; i < 200; i++) __NOP();

  LPC_SYSCON->MAINCLKSEL    = 0x00000003;                 // Select PLL Clock Output (MAINCLKSEL_Val)
  LPC_SYSCON->MAINCLKUEN    = 0x01;                       // Update MCLK Clock Source
  LPC_SYSCON->MAINCLKUEN    = 0x00;                       // Toggle Update Register
  LPC_SYSCON->MAINCLKUEN    = 0x01;
  while (!(LPC_SYSCON->MAINCLKUEN & 0x01));               // Wait Until Updated

  LPC_SYSCON->PDRUNCFG     &= ~(1 << 10);                 // Power-up USB PHY
  LPC_SYSCON->PDRUNCFG     &= ~(1 <<  8);                 // Power-up USB PLL

  LPC_SYSCON->USBPLLCLKSEL  = 0x00000001;                 // Select PLL Input (USBPLLCLKSEL_Val)
  LPC_SYSCON->USBPLLCLKUEN  = 0x01;                       // Update Clock Source
  LPC_SYSCON->USBPLLCLKUEN  = 0x00;                       // Toggle Update Register
  LPC_SYSCON->USBPLLCLKUEN  = 0x01;
  while (!(LPC_SYSCON->USBPLLCLKUEN & 0x01));             // Wait Until Updated

  LPC_SYSCON->USBPLLCTRL    = 0x00000023;                 // USBPLLCTRL_Val
  while (!(LPC_SYSCON->USBPLLSTAT   & 0x01));             // Wait Until PLL Locked
  LPC_SYSCON->USBCLKSEL     = 0x00;                       // Select USB PLL

  LPC_SYSCON->SYSAHBCLKDIV  = 0x00000001;                 // SYSAHBCLKDIV_Val
  LPC_SYSCON->SYSAHBCLKCTRL = 0x0001005F;                 // AHBCLKCTRL_Val [Sys, Rom, Ram, FlashREG, FlashARRAY, GPIO, ICON]
}

