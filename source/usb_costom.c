/*++

Copyright (c) 2012-2014 Razer Ltd.
All rights reserved. This program contains proprietary and confidential information.

Module Name:
  usb_costom.c

Compiler:
  Keil Software µVision v5.11, with NXP Semiconductor LPC11U2x proccessor support.

Abstract:


--*/


#include "main.h"
#include "type.h"
#include "usb_costom.h"
#include "usb_hardware.h"
#include "usb_core.h"
#include "usb_hid.h"
#include "variables.h"
#include "pmu.h"
#include "wdt.h"
#include "gpio.h"
#include "button.h"
#include "mouse.h"
#include "spi0_driver.h"
#include "macro.h"

#include "avago_api.h"


// [
#if (EP_OVERWRITE_PROTECT)
U8 SofCnt[4] ; 
U8 Endpoint_BusyFlag[4];                                          // Flag to indicate the usb ep busy or not
#endif
// ]

U8 hid_Protocol;
U8 hid_IdleTime[HID_REPORT_NUM];

#define P_EP(n) ((USB_EP_EVENT & (1 << (n))) ? USB_EndPoint##n : NULL)
void (* const USB_P_EP[USB_LOGIC_EP_NUM]) (uint32_t event) = {
    P_EP(0),
    P_EP(1),
    P_EP(2),
    P_EP(3),
    P_EP(4)
};

void USB_EndPoint1 (uint32_t event) 
/*
 *  USB Endpoint 1 Event Callback
 *   Called automatically on USB Endpoint 1 Event
 *    Parameter:       event
 */
{
#if (EP_OVERWRITE_PROTECT)

  switch (event) {
    case USB_EVT_IN:
      freeEp(1);
      break;
    case USB_EVT_OUT:
      break;
  }
#endif
}
void USB_EndPoint2 (uint32_t event) 
/*
 *  USB Endpoint 2 Event Callback
 *   Called automatically on USB Endpoint 1 Event
 *    Parameter:       event
 */
{
#if (EP_OVERWRITE_PROTECT)
  switch (event) {
    case USB_EVT_IN:
      freeEp(2);
      break;
    case USB_EVT_OUT:
      break;
  }
#endif
}

void USB_EndPoint3 (uint32_t event) 
/*
 *  USB Endpoint 3 Event Callback
 *   Called automatically on USB Endpoint 3 Event
 *    Parameter:       event
 */
{
#if (EP_OVERWRITE_PROTECT)
  switch (event) {
    case USB_EVT_IN:
      freeEp(3);
      break;
    case USB_EVT_OUT:
      break;
  }
#endif
}

void USB_SOF_Event(void) {
	//U8 i;
	if ((USB_Configuration != 0) && (deviceState >= VAR_INIT)) {
		if(SUSPEND_LED_fadoutflag == 0){
      ledTiming();
		}
    optical++;                                            // Sensor polling timer counter	
    scanningButtonStatus();                               // Process Button status detect
  }
}




U32 HID_GetReport(void) 
/*++

Function Description:
  This function Get Report Request Callback Called automatically on HID Get Report 
  Request. Operates on the global SetupPacket and EP0Buf buffers.

Arguments:
  NONE

Returns:
  status - SUCCESS or FAILURE.

--*/
{
  U8 i;

  switch (SetupPacket.wValue.WB.H) {
  case HID_REPORT_INPUT:
    return (FAILURE);                                       // Not Supported
  case HID_REPORT_OUTPUT:
    return (FAILURE);                                       // Not Supported
  case HID_REPORT_FEATURE:
    usbBusyFlag = 1; 
    if (SetupPacket.wLength == PROTOCOL_DATA_SIZE) {
      for (i = 0; i < PROTOCOL_DATA_SIZE; i++) {            // Get Report
        EP0Buf[i] = protocolTransmit[i];
      }
      if (usbReceiveFlag != 0) {                            // Protocol Set/Get are interrrupt, it may read protocol buffer during dock receiving 90 byte.
        EP0Buf[STATUS] = COMMAND_BUSY;                      // Set busy if the cmd hasn't complete its process.
      }
    }
    break;
  }
  return (SUCCESS);
}

U32 HID_SetReport(void) 
/*++

Function Description:
  This function Set Report Request Callback Called automatically on HID Set Report 
  Request. Operates on the global SetupPacket and EP0Buf buffers.

Arguments:
  NONE

Returns:
  status - SUCCESS or FAILURE.

--*/
{
  U8 i;

  switch (SetupPacket.wValue.WB.H) {
  case HID_REPORT_INPUT:
    return (FAILURE);                                     // Not Supported
  case HID_REPORT_OUTPUT:
    break;
  case HID_REPORT_FEATURE:
    usbBusyFlag = 1; 
    if (SetupPacket.wLength == PROTOCOL_DATA_SIZE) {      // Razer Protocol
      if (usbReceiveFlag == 0) {
        for (i = 0; i < PROTOCOL_DATA_SIZE; i++) {
          protocolTransmit[i] = EP0Buf[i];
        }
        protocolTransmit[STATUS] = COMMAND_BUSY;          // Set command packet is being processed
        usbReceiveFlag = 1;
      }
    }
    break;
  }
  return (SUCCESS);
}


U32 HID_GetIdle(void)
/*++

Function Description:
  This function Get Idle Request Callback Called automatically on HID Get Idle 
  Request. Operates on the global SetupPacket and EP0Buf buffers.

Arguments:
  NONE

Returns:
  status - SUCCESS or FAILURE.

--*/
{
  if (SetupPacket.wIndex.W < HID_REPORT_NUM) {
    EP0Buf[0] = hid_IdleTime[SetupPacket.wIndex.W];
  }
  return (SUCCESS);
}


U32 HID_SetIdle(void)
/*++

Function Description:
  This function Set Idle Request Callback Called automatically on HID Set Idle 
  Request. Operates on the global SetupPacket buffers.

Arguments:
  NONE

Returns:
  status - SUCCESS or FAILURE.

--*/
{
  if (SetupPacket.wIndex.W < HID_REPORT_NUM) {
    hid_IdleTime[SetupPacket.wIndex.W] = SetupPacket.wValue.WB.H;
  }
  return (SUCCESS);
}


U32 HID_GetProtocol(void)
/*++

Function Description:
  This function Get Protocol Request Callback Called automatically on HID Get 
  Protocol Request. Operates on the global SetupPacket buffers.

Arguments:
  NONE

Returns:
  status - SUCCESS or FAILURE.

--*/
{
  EP0Buf[0] = hid_Protocol;
  return (SUCCESS);
}


U32 HID_SetProtocol(void)
/*++

Function Description:
  This function Set Protocol Request Callback Called automatically on HID Set 
  Protocol Request. Operates on the global SetupPacket buffers.

Arguments:
  NONE

Returns:
  status - SUCCESS or FAILURE.

--*/
{
  hid_Protocol = SetupPacket.wValue.WB.L;
  return (SUCCESS);
}


#if USB_POWER_EVENT
void USB_Power_Event (U32  power) {
}
#endif


/*
 *  USB Reset Event Callback
 *   Called automatically on USB Reset Event
 */

#if USB_RESET_EVENT
void USB_Reset_Event (void) {
  USB_ResetCore();
  deviceMode = NORMAL_MODE;                               // Reset device mode to normal. SW will detect a new device eumeration and will set to driver mode if it present 
#if (EP_OVERWRITE_PROTECT)
  freeEp(1);
	freeEp(2);
	freeEp(3);
#endif
  if (SUSPEND_LED_fadoutflag == 1) {
    SUSPEND_LED_fadoutflag = 0;
    (void)memset((void*)matrixStaging,(int)0,(int)sizeof(matrixStaging));     //clear staging
  	(void)memset((void*)lighting,(int)0,(int)sizeof(lighting));   // clear lighting
	  forceLightingReload();                                  // Force a reload & refresh of all LEDs
  }
}
#endif


#if USB_CONFIGURE_EVENT
void USB_Configure_Event (void) {

  if (USB_Configuration) {                                // Check if USB is configured
    /* add your code here */
    LPC_USB->INTEN  |= (USB_SOF_EVENT ? FRAME_INT : 0);
	}
}
#endif


#if USB_SUSPEND_EVENT
void USB_Suspend_Event (void)
/*++

Function Description:
  This function automatically called on USB Suspend Event.

Arguments:
  NONE

Returns:
  NONE

--*/
{
  
	SPI_DE_ASSERT;
	delayMilliseconds(1);
	sensorShutDown();
	
  LPC_CT16B0->TCR = 0;                                  // Disable Timer16_0 'periodic' interrupt

	ledDriverWrite(LED_CTLR2_ADDR,REG_SHUT,0);            // set normal mode 
  gpio_SetValue(1, 26, 0);                              // Low to shut down led controller
		
  if ((USB_DeviceStatus & USB_GETSTATUS_REMOTE_WAKEUP) != 0) {		
    pmu_RemoteWakeup(TRUE);
  }  
	LPC_USB->INTEN  &= ~FRAME_INT;
}
#endif


#if USB_RESUME_EVENT
void USB_Resume_Event (void)
/*++

Function Description:
  This function automatically called on USB Resume Event.

Arguments:
  NONE

Returns:
  NONE

--*/
{
	LPC_USB->INTEN  |= FRAME_INT;                           // Enable SOF
  LPC_CT16B0->TCR = 1;                                    // Enable Timer16_0 'periodic interrupt'
  wdt_startTimer(WDT_TIMEOUT);                            // Start Watchdog Timer
  pmu_RemoteWakeup(FALSE);

	(void)memset((void*)lighting,(int)0,(int)sizeof(lighting));   // clear lighting
  (void)memset((void*)matrixStaging,(int)0,(int)sizeof(matrixStaging));     //clear staging
  led_initialization();									                  // Initialize LED PWM/drivers
	forceLightingReload();                                  // Force a reload & refresh of all LEDs
 	
	clearFlag = 0;                                          // clear sensor flag
	__disable_irq();                                        // Disable interrupts
  updateSensorFirmware();                                 // power up sensor
	__enable_irq();                                         // Enable interrupt

  calStatus = CAL_S_OFF;

#if (EP_OVERWRITE_PROTECT)
  freeEp(1);
	freeEp(2);
	freeEp(3);
#endif
}
#endif


#if USB_WAKEUP_EVENT
void USB_WakeUp_Event (void) 
/*++

Function Description:
  This function automatically called on USB Remote Wakeup Event.

Arguments:
  NONE

Returns:
  NONE

--*/
{
  if ((wakeupEvents & (WAKE_BUTTON | WAKE_WHEEL | WAKE_MOVEMENT)) != 0) {
    USB_WakeUp();
  }
}
#endif


void USB_checkStatus(void)
/*++

Function Description:
  This function processes the suspend and resume of USB support.

Arguments:
  NONE

Returns:
  NONE

--*/
{    
  if ((USB_DeviceStatus & USB_SUSPEND)) {// && (USB_Configuration != 0)) {
    if (SUSPEND_LED_fadoutflag == 0) {    // If usb suspend , timer for led change to 1ms interupte
		USB_Suspend_Event();
    wakeupEvents = 0;                                     // Clear all wakeup events
			
    do {
      if ((USB_DeviceStatus & USB_SUSPEND) == 0) {        // Usb status maybe changed by usb ISR, check it and eixt suspend if it change
        wakeupEvents |= WAKE_USB_RESET;
      }
      PMU_Sleep();                                        // Set MCU 'sleep' mode
      __NOP();
      __NOP();
      __NOP();
      __NOP();

    } while (wakeupEvents == 0);                          // If it is an 'ignore' event, go back to sleep
	
    USB_WakeUp_Event();                                   // Configure hardware accordingly
    USB_Resume_Event();                                   // Restore lighting and matrix scanning
    wakeupEvents = 0;                                     // Clear all wakeup events
   }
	}
}

