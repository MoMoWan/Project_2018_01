/*++

Copyright (c) 2013-2014 Razer Ltd.
All rights reserved. This program contains proprietary and confidential information.

Module Name:
  timer.c

Compiler:
  Keil Software µVision v5.01, with NXP Semiconductor LPC11U2x proccessor support.

Abstract:


--*/

#include "main.h"
#include "timer.h"
#include "button.h"
#include "Mouse.h"
#include "variables.h"
#include "wdt.h"
//#include "usb_hardware.h"
#include "led_driver.h"
#include "avago_api.h"
#include "macro.h"
#include	"CT16B0.h"
#include	"CT16B1.h"
//void TIMER16_0_IRQHandler(void)
///*++

//Function Description:
//  This interrupt handler (1ms Periodic Interrupt) processes all timing tasks.
// 
//Arguments:
//  NONE

//Returns: 
//  NONE

//--*/
//{ 
//	U8 i;
//  LPC_CT16B0->IR = 0x1F;                                  // Clear interrupt flag

//	++ Profiletime;
//	++ Profiledebounce;
//  SROMchecktimer ++;
//  if (milliseconds != 0) {
//    milliseconds--;
//  }

//  if (updateOneshot != 0) {
//    updateOneshot--;
//  }
//	 if ((PIN_MID == 0) && (PIN_RGT == 0) && (PIN_LFT == 0)){
//     CalibrationTimer++;
//   }
//	
//	if(AutoCalibrationFalg == 1) {
//    HaltTimer1++;
//  }
//	if(HaltTimer2 < 0x9FFFF){
//     HaltTimer2++;
//  }
//	
//	if (user.devicestatus == DEVICE_STATUS_RETAIL) {
//		for (i = 0; i < NUMBER_OF_LEDS; i++) {
//        lighting[i].tmr++;
//				lighting[i].extendTimer++;
//      }
//	}
//	
//  if ((USB_Configuration != 0) && (deviceState >= VAR_INIT)) {
//    basicFWTimer ++;
//		if((SUSPEND_LED_fadoutflag == 1)&&(user.devicestatus==DEVICE_STATUS_NORMAL)){
//			ledTiming();
//		}
//		if(sensorIntegrity!=0)
//				sensorIntegrity--;
//  }
//  
//  return;
//}


//void TIMER32_0_IRQHandler(void)
///*++

//Function Description:
//  This interrupt handler which is only enabled during MCU sleep mode has a rate
//  of 100ms, to meant to feed the watchdog timer, scanning wakeup source: button or wireless mouse

//Arguments:
//  NONE

//Returns: 
//  NONE

//--*/
//{
//  LPC_CT32B0->IR = 0x01F;                                 // Clear interrupt flag
//  wdt_resetTimer();                                       // If Watchdog Timer expires the MCU resets
//  return;
//}


void Tmr_initialiation(void)
/*++

Function Description:
  This function initialize the timer #0, sets the timer interval, then reset
  the timer, and install timer interrupt handler. Some of the I/O pins need to be 
  clearfully planned if you use below module because JTAG and TIMER CAP/MAT pins 
  are muxed.

Arguments:
  NONE

Returns:
  NONE

--*/
{
//  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<7);                    // Enable clock to timer16_0
//#if 1  // fine turn the timing to sync with BW chroma
//  LPC_CT16B0->PR  = 1;                                   // (0-47)48 Prescale at 48MHz main clock to get 1us
//  LPC_CT16B0->MR0 = (24000-6);                                 // 1000 * 1us = 1ms
//#else // standard setting
//  LPC_CT16B0->PR  = 47;                                   // (0-47)48 Prescale at 48MHz main clock to get 1us
//  LPC_CT16B0->MR0 = 1000;                                 // 1000 * 1us = 1ms
//#endif  
//  LPC_CT16B0->MCR = (0x3<<0);                             // Interrupt and Reset on MR0
//  NVIC_EnableIRQ(TIMER_16_0_IRQn);                        // Enable the Timer0 Interrupt
//  LPC_CT16B0->TCR = 1;                                    // Enable Timer16_0

//  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<9);                    // Enable clock to timer32_0
//  LPC_CT32B0->PR  = 125;                                  // 125 Prescale at 125KHz main clock [sleep mode only] to get 1ms
//  LPC_CT32B0->MR0 = 200;                                  // 200 * 1ms = 200ms
//  LPC_CT32B0->MCR = (0x3<<0);                             // Interrupt and Reset on MR0

//  NVIC_EnableIRQ(TIMER_32_0_IRQn);                        // Enable the Interrupt
//  LPC_CT32B0->TCR = 0;                                    // Disable Timer32_0, only enable it when enter sleep mode only
//  return;
  	CT16B0_Init();
    CT16B1_Init();
}


