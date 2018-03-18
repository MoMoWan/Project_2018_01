/*++

Copyright (c) 2012-2014 Razer Ltd.
All rights reserved. This program contains proprietary and confidential information.

Module Name:
  usb_descriptor.h

Compiler:
  Keil Software µVision v5.11, with NXP Semiconductor LPC11U2x proccessor support.

Abstract:

--*/

#ifndef __USBDESC_H__
#define __USBDESC_H__

#include "main.h"

#define _SELFPOWER                      (0x01 << 6)       // Self-powered
#define _REMOTEWAKEUP                   (0x01 << 5)       // Remote Wakeup
#define _DEFAULT                        (0x01 << 7)       // Default Value

#define EP0_BUFF_SIZE                    8                // Can be 8, 16, 32, or 64 for full speed
#define INTERFACE_NUM                    USB_IF_NUM       // Actual number of interfaces 

#define WBVAL(x) ((x) & 0xFF),(((x) >> 8) & 0xFF)

#define USB_DEVICE_DESC_SIZE        (sizeof(USB_DEVICE_DESCRIPTOR))
#define USB_CONFIGUARTION_DESC_SIZE (sizeof(USB_CONFIGURATION_DESCRIPTOR))
#define USB_INTERFACE_DESC_SIZE     (sizeof(USB_INTERFACE_DESCRIPTOR))
#define USB_ENDPOINT_DESC_SIZE      (sizeof(USB_ENDPOINT_DESCRIPTOR))

#define GET_INTERFACE_DESCRIPTOR(n)     (USB_ConfigDescriptor+ USB_CONFIGUARTION_DESC_SIZE + (n * USB_INTERFACE_DESC_SIZE) + (n * HID_DESC_SIZE) + (n * USB_ENDPOINT_DESC_SIZE))
#define GET_HID_DESCRIPTOR(n)          (GET_INTERFACE_DESCRIPTOR(n) + USB_INTERFACE_DESC_SIZE)
#define GET_ENDPOINT_DESCRIPTOR(n)     (GET_INTERFACE_DESCRIPTOR(n) + USB_INTERFACE_DESC_SIZE + HID_DESC_SIZE)

#define HID_DESC_OFFSET                  0x0012
#define HID_DESC_SIZE                   (sizeof(HID_DESCRIPTOR))

typedef struct _REPORT_DESC_LIST {
  U8*     ReportDesc;     /* Point to Report Descriptor */
  U16     ReportDescSize; /* The size of Report Descriptor */
} REPORT_DESC_LIST;

extern const U8 USB_DeviceDescriptor[];

extern const U8 USB_ConfigDescriptor[];
extern const U8 USB_StringDescriptor[];
extern const U8 HIDMouseReport[94];

extern const REPORT_DESC_LIST HID_ReportDescList[];

#endif  /* __USBDESC_H__ */
