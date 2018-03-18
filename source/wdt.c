/*++

Copyright (c) 2013-2014 Razer Ltd.
All rights reserved. This program contains proprietary and confidential information.

Module Name:
  wdt.c

Compiler:
  Keil Software µVision v5.01, with NXP Semiconductor LPC11U3x proccessor support.

Abstract:
  This file provides the Watchdog Timer support.

--*/

#include "wdt.h"
#include "Type.h"

void wdt_initialization(U32 mode)
/*++

Function Description:
  This function initializes the Watchdog Timer mode, to reset the MCU if it times
  out.

Arguments:
  WDTMode - 0: Generate Interrupt only.
            1: Force MCU reset on expiration.
Returns:
  NONE

--*/
{
	SN_WDT->CFG = 0x5AFA0000;  			//** use WDT Reset!
	SN_SYS1->APBCP1 = (0x0<<20);		//** 31.25 us * 128 = 4ms
	SN_WDT->TC = 0x5AFA00FE;			//** (TC + 2) * 4ms = 256ms @ ILRC / 1 = 32KHz  // 3E = 62+2 = 256ms FE = 254 = 1024ms
//	SN_WDT->CFG |= 0x5AFA0001; 			//** Enable
}


void wdt_configuration  (void)
/*++

Function Description:
  This function configures the clock for the Watchdog Timer.

Arguments:
  NONE

Returns:
  NONE

--*/
{  
//  LPC_SYSCON->CLKOUTSEL = 0x01;	                          // Select Main clock 
//  LPC_SYSCON->CLKOUTUEN = 0x01;		                        // Update clock 
//  LPC_SYSCON->CLKOUTUEN = 0x00;		                        // Toggle update register once 
//  LPC_SYSCON->CLKOUTUEN = 0x01;
//  while ( !(LPC_SYSCON->CLKOUTUEN & 0x01) );		          // Wait until updated 
//  LPC_SYSCON->CLKOUTDIV = 1;			                        // Divided by 1 
//  LPC_SYSCON->PDRUNCFG &= ~(0x1<<6);                      //Let WDT clock run 
//  return;
}


void wdt_resetTimer	(void)
/*++Function Description:
  This function feeds the Watchdog Timer, to prevent the MCU reset.

Arguments:
  NONE

Returns:
  NONE

--*/
{
//  __disable_irq();                                        // Disable MCU interrupts
//  LPC_WWDT->FEED = 0xAA;
//  LPC_WWDT->FEED = 0x55;
//  __enable_irq();                                         // Enable MCU interrupts
  SN_WDT->FEED = 0x5AFA55AA;	
}


void wdt_startTimer(U32 timeout)
/*++

Function Description:
  This function starts Watchdog Timer activity with given timeout value.

Arguments:
  timeout - Timeout value (us).

Returns:
  NONE

--*/
{
//	#if releaseFW
//  wdt_setTimeout(timeout);
//  LPC_WWDT->MOD |= WDT_WDMOD_WDEN;                         // Enable Watchdog Timer
//  wdt_resetTimer();
//	#endif
//  SN_WDT->CFG |= 0x5AFA0001; 			//Enable
}

U8 wdt_setTimeout(U32 timeout)
/*++

Function Description:
  This function sets Watchdog Timer timeout value and Watchdog Timer mode.

Arguments:
  timeout - Timeout value (us).

Returns:
  status - SUCCESS or FAILURE.

--*/
{
//  U32 t;

//  t = ((48) * (timeout / 4));
//  if ((t >= WDT_TIMEOUT_MIN) && (t <= WDT_TIMEOUT_MAX)) { // Check if it valid
//    LPC_WWDT->TC = t;
//    return SUCCESS;
//  }
  return FAILURE;
}

