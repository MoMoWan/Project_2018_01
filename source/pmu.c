/*++

Copyright (c) 2013-2014 Razer Ltd.
All rights reserved. This program contains proprietary and confidential information.

Module Name:
  pmu.c

Compiler:
  Keil Software µVision v5.01, with NXP Semiconductor LPC11U2x proccessor support.

Abstract:


--*/

#include "main.h"
#include "type.h"
#include "gpio.h"
#include "pmu.h"
#include "variables.h"
#include "mouse.h"
#include "usb_core.h"


void pmu_RemoteWakeup(U8 state)
/*++

Function Description:
  This function enables or disables MCU wake-up events from sleep or deep sleep
  modes - by enabling wakeup interrupts on all of the buttons, keys and wheel decode
  pins.

Arguments:
  state - 0 = Disable, 1 = Enable.

Returns:
  NONE

--*/
{
  if (state == TRUE) {
    LPC_SYSCON->STARTERP0 |= ((1 << 0) | (1 << 1));       // Enable Channel 0/1: wheel encoder wakeup source, E
    LPC_SYSCON->STARTERP1 |= ((1 << 20) | (1 << 21));     // Enable GPIO Group0/1: button and key wakeup source
    NVIC_ClearPendingIRQ(GINT0_IRQn);                     
    NVIC_EnableIRQ(GINT0_IRQn);
    LPC_GPIO_GROUP_INT1->CTRL |= (1 << 0);                // Enable Group1 Interrupt
    NVIC_ClearPendingIRQ(GINT1_IRQn);
    NVIC_EnableIRQ(GINT1_IRQn);
  } else {
    LPC_SYSCON->STARTERP0 &= ~((1 << 0) | (1 << 1));      // Disable Channel 0/1: wheel encoder wakeup source, E
    LPC_SYSCON->STARTERP1 &= ~((1 << 20) | (1 << 21));    // Disable GPIO Group0/1: button and key wakeup source
    NVIC_ClearPendingIRQ(GINT0_IRQn);
    NVIC_DisableIRQ(GINT0_IRQn);
    NVIC_ClearPendingIRQ(GINT1_IRQn);
    NVIC_DisableIRQ(GINT1_IRQn);
  }
}


void FLEX_INT0_IRQHandler(void)
/*++

Function Description:
  GPIO IRQ channel 0 IRQ handle function.
  It configure scroll wheel decoder up single IRQ detect function.

Arguments:
  NONE
  
Returns:
  NONE

--*/
{
  if (LPC_GPIO_PIN_INT->IST & (1 << 0)) {                 // Clear interupt
    LPC_GPIO_PIN_INT->IST |= (1 << 0);
  }
  scrollWheelHandling();
  if ((USB_DeviceStatus & USB_SUSPEND) && (USB_DeviceStatus & USB_GETSTATUS_REMOTE_WAKEUP)) {
     wakeupEvents |= WAKE_WHEEL;
  }

}


void FLEX_INT1_IRQHandler(void)
/*++

Function Description:
  GPIO IRQ channel 1 IRQ handle function.
  It configure scroll wheel decoder down single IRQ detect function

Arguments:
  NONE
  
Returns:
  NONE

--*/
{
  if (LPC_GPIO_PIN_INT->IST & (1 << 1)) {                 // Clear interupt
    LPC_GPIO_PIN_INT->IST |= (1 << 1);
  }
  scrollWheelHandling();
  if ((USB_DeviceStatus & USB_SUSPEND) && (USB_DeviceStatus & USB_GETSTATUS_REMOTE_WAKEUP)) {
    wakeupEvents |= WAKE_WHEEL;
  }
}



void GINT0_IRQHandler(void)
/*++

Function Description:
  Group Interrupt 0 IRQ handle function.
  It configure to wakeup mouse from suspend.

Arguments:
  NONE
  
Returns:
  NONE

--*/
{
  NVIC_ClearPendingIRQ(GINT0_IRQn);
	LPC_GPIO_GROUP_INT0->CTRL |= (1 << 0);                  // Clear interrupt
  if ((USB_DeviceStatus & USB_SUSPEND) && (USB_DeviceStatus & USB_GETSTATUS_REMOTE_WAKEUP)) {
    wakeupEvents |= WAKE_BUTTON;
  } 
}


void GINT1_IRQHandler(void)
/*++

Function Description:
  Group Interrupt 1 IRQ handle function.
  It configure to wakeup mouse from suspend.

Arguments:
  NONE
  
Returns:
  NONE

--*/
{
  NVIC_ClearPendingIRQ(GINT1_IRQn);
	LPC_GPIO_GROUP_INT1->CTRL |= (1 << 0);                  // Clear interrupt
  if ((USB_DeviceStatus & USB_SUSPEND) && (USB_DeviceStatus & USB_GETSTATUS_REMOTE_WAKEUP)) {
    wakeupEvents |= WAKE_BUTTON;
  } 
}



void PMU_InitGroupedInterrupt(void)
/*++

Function Description:
  Group Interrupt source configuration function API

Arguments:
  NONE
  
Returns:
  NONE

--*/
{
  LPC_GPIO_GROUP_INT1->PORT_POL[0] = 0;                   // Active low
  LPC_GPIO_GROUP_INT1->PORT_POL[1] = 0;                   // Active low
  LPC_GPIO_GROUP_INT1->PORT_ENA[0] = WAKEUP_PORT0;        // Set which pins to generate a interrupt of port 0 
  LPC_GPIO_GROUP_INT1->PORT_ENA[1] = WAKEUP_PORT1;        // Set which pins to generate a interrupt of port 1
  /* as soon as enabled, an edge may be generated       
	clear interrupt flag and NVIC pending interrupt to 
	workaround the potential edge generated as enabled */
  LPC_GPIO_GROUP_INT1->CTRL &= ~(1 << 2);                 // Edge trigger 
  LPC_GPIO_GROUP_INT1->CTRL |= (0 << 1);                  // OR functionality, any enabled pin active will generate a group Interrupt
  LPC_GPIO_GROUP_INT1->CTRL |= (1 << 0);                  // Clear Group1 Interrupt Status once after init

  return;
}


void PMU_Init(void)
/*++
Function name: PMU_Init

Descriptions: Initialize PMU and setup wakeup source.
    For Sleep and deepsleep, any of the I/O pins can be 
    used as the wakeup source.
    For Deep Powerdown, only pin P1.4 can be used as 
    wakeup source from deep powerdown. 

parameters:			
    None

Returned value:		
    None
 
--*/
{
  LPC_SYSCON->SYSAHBCLKCTRL |= (CLK_PINT) | (CLK_G0INT) | (CLK_G1INT); // Enable clock for PIN, GPIO Group0/1 source
  
  GPIOSetFlexInterrupt(CHANNEL0, PORT0, 18, 1, 0);        // Set channel 0 to Wheel encoder down signal and enable its IRQ
  GPIOSetFlexInterrupt(CHANNEL1, PORT0, 19, 1, 0);        // Set channel 1 to Wheel encoder up signal and enable its IRQ  
  PMU_InitGroupedInterrupt();
  pmu_RemoteWakeup(FALSE);                                // Disable Remote wake up function during init.
}


void PMU_Sleep(void)
/*++

Function Description:
  Put some of the peripheral in sleep mode.

Arguments:
  SleepMode -  2 is power down, 1 is deep sleep, 0 is sleep
                      [1, 2 may not correct now, please don't use it]
  SleepCtrl -    default 0x00, only set bit3 or bit6 under mode 1 or 2

Returns:
  NONE

--*/
{
  U8 i;
  
  //LPC_SYSCON->PDAWAKECFG = LPC_SYSCON->PDRUNCFG;
  LPC_SYSCON->PDSLEEPCFG = (1 << 3);                      // BOS_PD powered down, WDTOSC_PD powered 

  LPC_SYSCON->PDRUNCFG &= ~WDT_OSC_PD;                    // Power up WDT Osc
  LPC_SYSCON->WDTOSCCTRL = (0x0F << 5) | (0 << 0);        // Run at 1.7 MHz
  for (i = 0; i < 100; i++);

  LPC_SYSCON->MAINCLKSEL = 2;                             // Switch main clock to WDT
  LPC_SYSCON->MAINCLKUEN = 0;
  LPC_SYSCON->MAINCLKUEN = 1;                             // Toggle to enable
  LPC_SYSCON->MAINCLKUEN = 0;
  LPC_SYSCON->USBCLKSEL = 1;                              // Switch USB clock to main clock
  LPC_SYSCON->USBCLKUEN = 0;
  LPC_SYSCON->USBCLKUEN = 1;                              // Toggle to enable
  LPC_SYSCON->USBCLKUEN = 0;
  LPC_SYSCON->SYSAHBCLKCTRL &= ~(CLK_GPIO | CLK_T16B0 | CLK_T32B0 | CLK_SSP0 | CLK_IOCON | CLK_I2C); 
  LPC_SYSCON->PDRUNCFG |= (IRC_OUT_PD | IRC_PD | BOD_PD | ADC_PD | SYS_OSC_PD | SYS_PLL_PD | USB_PHY_PD | USB_PLL_PD);
  LPC_SYSCON->WDTOSCCTRL = (1 << 5) | (1 << 1);           // Configure WDT to run at 125KHz   
//  LPC_CT32B0->TCR = 1;                                    // Enable Timer16_1 'keep watchdog happy' interrupt
  LPC_PMU->PCON = 0x00;                                   // Set sleep mode
  SCB->SCR &= ~NVIC_LP_SLEEPDEEP;                         // Enter sleep

   __WFI();                                               // Waiting for interrupt
  
//  LPC_CT32B0->TCR = 0;                                    // Disable Timer16_1 'keep watchdog happy' interrupt
  LPC_SYSCON->PDRUNCFG &= ~(IRC_OUT_PD | IRC_PD | BOD_PD | SYS_OSC_PD | SYS_PLL_PD | USB_PHY_PD | USB_PLL_PD);   
  for (i = 0; i < 100; i++);
  while (!(LPC_SYSCON->SYSPLLSTAT & 0x01));               // Wait for main PLL to lock
  LPC_SYSCON->MAINCLKSEL = 3;                             // Switch main clock to PLL
  LPC_SYSCON->MAINCLKUEN = 0;
  LPC_SYSCON->MAINCLKUEN = 1;                             // Toggle to enable
  LPC_SYSCON->MAINCLKUEN = 0;  
  while (!(LPC_SYSCON->USBPLLSTAT & 0x01));               // Wait for USB PLL to lock
  LPC_SYSCON->USBCLKSEL = 0;                              // Switch USB clock to USB PLL
  LPC_SYSCON->USBCLKUEN = 0;
  LPC_SYSCON->USBCLKUEN = 1;                              // Toggle to enable
  LPC_SYSCON->USBCLKUEN = 0;
  LPC_SYSCON->SYSAHBCLKCTRL |= (CLK_GPIO | CLK_T16B0 | CLK_T32B0 | CLK_SSP0 | CLK_IOCON | CLK_I2C);
  
  return;
}

