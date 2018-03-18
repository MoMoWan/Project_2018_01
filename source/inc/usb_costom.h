/*++

Copyright (c) 2012-2014 Razer Ltd.
All rights reserved. This program contains proprietary and confidential information.

Module Name:
 usb_costom.h

Compiler:
  Keil Software µVision v5.11, with NXP Semiconductor LPC11U2x proccessor support.

Abstract:


--*/

#ifndef _USB_DRIVER_H_
#define _USB_DRIVER_H_

#include "stdint.h"
#include "usb_configure.h"
#include "main.h"

#define WAKEUPPOSTPONETIME      800  


#define HID_REPORT_NUM          3 //1//                           // HID Number of Reports

#define HID_BOOT_PROTOCOL       0                           // Boot protocol for set/get protocol 
#define HID_REPORT_PROTOCOL     1                           // Non Boot protocol for set/get protocol 

// [
#if (EP_OVERWRITE_PROTECT)
extern U8 Endpoint_BusyFlag[4];

extern U8 SofCnt[4] ; 
#define isEpBusy(n) (Endpoint_BusyFlag[n])
#define setEpBusy(n)   {Endpoint_BusyFlag[n]  = 1;SofCnt[n] =2;}
#define freeEp(n)     {Endpoint_BusyFlag[n]  = 0;SofCnt[n] =0;}
#endif
// ]


/* USB Core Events Callback Functions */
extern void USB_Configure_Event (void);
extern void USB_Interface_Event (void);
extern void USB_Feature_Event   (void);
extern void USB_CheckStatus(void);
extern void usbRetrieveStatus(void);

/* USB Device Events Callback Functions */
extern void USB_Power_Event     (U32  power);
extern void USB_Reset_Event     (void);
extern void USB_Suspend_Event   (void);
extern void USB_Resume_Event    (void);
extern void USB_WakeUp_Event    (void);
extern void USB_SOF_Event       (void);
extern void USB_Error_Event     (U32 error);

/* USB Endpoint Callback Events */
#define USB_EVT_SETUP       1   /* Setup Packet */
#define USB_EVT_OUT         2   /* OUT Packet */
#define USB_EVT_IN          3   /*  IN Packet */
#define USB_EVT_OUT_NAK     4   /* OUT Packet - Not Acknowledged */
#define USB_EVT_IN_NAK      5   /*  IN Packet - Not Acknowledged */
#define USB_EVT_OUT_STALL   6   /* OUT Packet - Stalled */
#define USB_EVT_IN_STALL    7   /*  IN Packet - Stalled */

/* USB Endpoint Events Callback Functions */
extern void USB_EndPoint0  (uint32_t event);
extern void USB_EndPoint1  (uint32_t event);
extern void USB_EndPoint2  (uint32_t event);
extern void USB_EndPoint3  (uint32_t event);
extern void USB_EndPoint4  (uint32_t event);
extern void USB_checkStatus(void);

extern U8 hid_Protocol;                                     // HID Global Variables
extern U8 hid_IdleTime[HID_REPORT_NUM];


extern U8 hid_Protocol;                                     // HID Global Variables
extern U8 hid_IdleTime[HID_REPORT_NUM];

extern void (* const USB_P_EP[USB_LOGIC_EP_NUM])(U32 event);// USB Endpoint Events Callback Pointers



#endif // _USB_DRIVER_H_
