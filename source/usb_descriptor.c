/*++

Copyright (c) 2012-2014 Razer Ltd.
All rights reserved. This program contains proprietary and confidential information.

Module Name:
  usb_descriptor.c

Compiler:
  Keil Software µVision v5.11, with NXP Semiconductor LPC11U2x proccessor support.

Abstract:
  This file provides the USB Descriptor table data.

--*/

#include "main.h"
#include "usb_hid.h"
#include "usb_core.h"
#include "usb_configure.h"
#include "usb_descriptor.h"
#include "usb_hid.h"


const U8 USB_DeviceDescriptor[] = {                       // USB Standard Device Descriptor
  USB_DEVICE_DESC_SIZE,                                   // bLength
  USB_DEVICE_DESCRIPTOR_TYPE,                             // bDescriptorType
  WBVAL(0x0200),                                          // bcdUSB
  0x00,                                                   // bDeviceClass
  0x00,                                                   // bDeviceSubClass
  0x00,                                                   // bDeviceProtocol
  USB_MAX_PACKET0,                                        // bMaxPacketSize0
  WBVAL(DEVICE_VID),                                      // idVendor
  WBVAL(DEVICE_PID),                                      // idProduct
  WBVAL(0x0200),                                          // bcdDevice
  0x01,                                                   // iManufacturer
  0x02,                                                   // iProduct
  0x00,                                                   // iSerialNumber
  0x01                                                    // bNumConfigurations: one possible configuration
};

const U8 HIDMouseReport[] = {                             // Mouse Descriptor (Report Protocol)
  0x05, 0x01,                                             // Usage Page (Generic Desktop)
  0x09, 0x02,                                             // Usage (Mouse)
  0xA1, 0x01,                                             // Collection (Application)
  0x09, 0x01,                                             //   Usage (Pointer)
  0xA1, 0x00,                                             //   Collection (Physical)
  0x05, 0x09,                                             //     Usage Page (Buttons)
  0x19, 0x01,                                             //     Usage Minimum (1 buttons)
  0x29, 0x05,                                             //     Usage Maximum (5 buttons)
  0x15, 0x00,                                             //     Logic Minimum (0)
  0x25, 0x01,                                             //     Logic Maximum (1)
  0x75, 0x01,                                             //     Report Size (1)
  0x95, 0x05,                                             //     Report Count (5)
  0x81, 0x02,                                             //     Input (Data, Variable, Absolute)     - Button Byte
  0x75, 0x01,                                             //     Report Size (1)
  0x95, 0x03,                                             //     Report Count (3)
  0x81, 0x03,                                             //     Input (Constant, Variable, Absolute) - Button Padding
  0x06, 0x00, 0xFF,                                       //     Usage Page (Reserved)
  0x09, 0x40,                                             //     Usage (X, Y)
  0x75, 0x08,                                             //     Report Size (8)
  0x95, 0x02,                                             //     Report Count (2)
  0x15, 0x81,                                             //     Logical Minimum (-127)
  0x25, 0x7F,                                             //     Logical Maximum (+127)
  0x81, 0x02,                                             //     Input (Data, Variable, Absolute)     - X, Y Bytes
  0x05, 0x01,                                             //     Usage Page (Generic Desktop)
  0x09, 0x38,                                             //     Usage (Scroll Wheel)
  0x15, 0x81,                                             //     Logical Minimum (-127)
  0x25, 0x7F,                                             //     Logical Maximum (+127)
  0x75, 0x08,                                             //     Report Size (8)
  0x95, 0x01,                                             //     Report Count (1)
  0x81, 0x06,                                             //     Input (Data, Variable, Relative)     - Z Bytes
  0x09, 0x30,                                             //     Usage (X)
  0x09, 0x31,                                             //     Usage (Y)
  0x16, 0x00, 0x80,                                       //     Logical Minimum (-32767)
  0x26, 0xFF, 0x7F,                                       //     Logical Maximum (+32767)
  0x75, 0x10,                                             //     Report Size (16)
  0x95, 0x02,                                             //     Report Count (2)
  0x81, 0x06,                                             //     Output (Data, Variable, Relative)     - X, Y Words
  0xC0,                                                   //   End Collection
  0x06, 0x00, 0xFF,                                       //   Usage Page (Vendor Defined Page 1)
  0x09, 0x02,                                             //   Usage (Mouse)
  0x15, 0x00,                                             //   Logical Min (0)
  0x25, 0x01,                                             //   Logical Max (1)
  0x75, 0x08,                                             //   Report Size (8)
  0x95, 0x5A,                                             //   Report Count (90)
  0xB1, 0x01,                                             //   Feature (Constant, Array, Absolute)
  0xC0                                                    // End Collection
};


const U8 HID_RazerReport[] = {                            // Razer Endpoint Descriptor
  0x05, 0x01,                                             // Usage Page (Generic Desktop)
  0x09, 0x06,                                             // Usage (KeyBoard)
  0xA1, 0x01,                                             // Collection (Application)
  0x85, 0x01,                                             //   Report ID of Extended Report        - Record ID, 1-byte (0x01)
  0x05, 0x07,                                             //   Usage Page (Key Codes)
  0x19, 0xE0,                                             //   Usage Min (L_Ctrl)
  0x29, 0xE7,                                             //   Usage Max (R_GUI)
  0x15, 0x00,                                             //   Logical Min (0)
  0x25, 0x01,                                             //   Logical Max (1)
  0x75, 0x01,                                             //   Report Size (1)
  0x95, 0x08,                                             //   Report Count (8)
  0x81, 0x02,                                             //   Input (Data, Variable, Absolute)    - Modifier Byte
  0x19, 0x00,                                             //   Usage Min (0x00)
  0x2A, 0xFF, 0x00,                                       //   Usage Max (0x00FF)
  0x15, 0x00,                                             //   Logical Min (0x00)
  0x26, 0xFF, 0x00,                                       //   Logical Max (0x00FF)
  0x75, 0x08,                                             //   Report Size (8)
  0x95, 0x0E,                                             //   Report Count (14)
  0x81, 0x00,                                             //   Input (Data, Array, Absolute)       - Keycode Array
  0xC0,                                                   // End_Collection
  0x05, 0x0C,                                             // Usage Page (Consumer)
  0x09, 0x01,                                             // Usage (Consumer)
  0xA1, 0x01,                                             // Collection (Application)
  0x85, 0x02,                                             //   Report ID of Multimedia keys        - Record ID, 1-byte (0x02)
  0x19, 0x00,                                             //   Usage Min (0x0000)
  0x2A, 0x3C, 0x02,                                       //   Usage Max (0x023C)
  0x15, 0x00,                                             //   Logical Min (0x0000)
  0x26, 0x3C, 0x02,                                       //   Logical Max (0x023C)
  0x95, 0x01,                                             //   Report Count (1)
  0x75, 0x10,                                             //   Report Size (16)
  0x81, 0x00,                                             //   Input (Data, Array, Absolute)       - Media Key, 1-word
  0x75, 0x08,                                             //   Report Size (8)
  0x95, 0x0D,                                             //   Report Count (13)
  0x81, 0x01,                                             //   Input (Constant, Array, Absolute)   - Packet Padding, 5-byte
  0xC0,                                                   // End Collection
  0x05, 0x01,                                             // Usage Page (Generic desktop)
  0x09, 0x80,                                             // Usage (System Control)
  0xA1, 0x01,                                             // Collection (Application)
  0x85, 0x03,                                             //   Report ID of Power keys             - Record ID, 1-byte (0x03)
  0x19, 0x81,                                             //   Usage Min (0x81)
  0x29, 0x83,                                             //   Usage Max (0x83)
  0x15, 0x00,                                             //   Logical Min (0)
  0x25, 0x01,                                             //   Logical Max (1)
  0x75, 0x01,                                             //   Report Size (1)
  0x95, 0x03,                                             //   Report Count (3)
  0x81, 0x02,                                             //   Input (Data, Variable, Absolute)    - Power Keys, 3-bits
  0x95, 0x05,                                             //   Report Count (5)
  0x81, 0x01,                                             //   Input (Constant, Array, Absolute)   - Byte Padding, 5-bits
  0x75, 0x08,                                             //   Report Size (8)
  0x95, 0x0E,                                             //   Report Count (14)
  0x81, 0x01,                                             //   Input (Constant, Array, Absolute)   - Packet Padding, 6-bytes
  0xC0,                                                   // End_Collection
  0x05, 0x01,                                             // Usage Page (Generic Desktop)
  0x09, 0x00,                                             // Usage (Undefined)
  0xA1, 0x01,                                             // Collection (Application)
  0x85, 0x04,                                             //   Report ID of Razer keys             - Record ID, 1-byte (0x04)
  0x09, 0x03,                                             //   Usage (Unknown)
  0x15, 0x00,                                             //   Logical Min (0x00)
  0x26, 0xFF, 0x00,                                       //   Logical Max (0x00FF)
  0x35, 0x00,                                             //   Physical Minimum (0)
  0x46, 0xFF, 0x00,                                       //   Physical Maximum (255)
  0x75, 0x08,                                             //   Report Size (8)
  0x95, 0x0F,                                             //   Report Count (15)
  0x81, 0x00,                                             //   Input (Data, Array, Absolute)       - Keycode Array
  0xC0,                                                   // End_Collection
  0x05, 0x01,                                             // Usage Page (Generic Desktop)
  0x09, 0x00,                                             // Usage (Undefined)
  0xA1, 0x01,                                             // Collection (Application)
  0x85, 0x05,                                             //   Report ID of Notifications          - Record ID, 1-byte (0x05)
  0x09, 0x03,                                             //   Usage (Unknown)
  0x15, 0x00,                                             //   Logical Min (0x00)
  0x26, 0xFF, 0x00,                                       //   Logical Max (0x00FF)
  0x35, 0x00,                                             //   Physical Minimum (0)
  0x46, 0xFF, 0x00,                                       //   Physical Maximum (255)
  0x75, 0x08,                                             //   Report Size (8)
  0x95, 0x0F,                                             //   Report Count (15)
  0x81, 0x00,                                             //   Input (Data, Array, Absolute)       - Code Array
  0xC0                                                    // End_Collection
  
};


const U8 HIDKeyboardReport[] = {                          // Keyboard Descriptor (Boot Protocol)
  0x05, 0x01,                                             // Usage Page (Generic Desktop)
  0x09, 0x06,                                             // Usage (Keyboard)
  0xA1, 0x01,                                             // Collection (Application)
  0x05, 0x07,                                             //   Usage Page (Key Codes)
  0x19, 0xE0,                                             //   Usage Min (L_Ctrl)
  0x29, 0xE7,                                             //   Usage Max (R_GUI)
  0x15, 0x00,                                             //   Logical Min (0)
  0x25, 0x01,                                             //   Logical Max (1)
  0x75, 0x01,                                             //   Report Size (1)
  0x95, 0x08,                                             //   Report Count (8)
  0x81, 0x02,                                             //   Output (Data, Variable, Absolute)  - Modifier Byte
  0x81, 0x01,                                             //   Output (Constant, Array, Absolute) - Reserved byte
  0x19, 0x00,                                             //   Usage Min (0x00)
  0x2A, 0xFF, 0x00,                                       //   Usage Max (0x00FF)
  0x15, 0x00,                                             //   Logical Min (0x00)
  0x26, 0xFF, 0x00,                                       //   Logical Max (0x00FF)
  0x75, 0x08,                                             //   Report Size (8)
  0x95, 0x06,                                             //   Report Count (6)
  0x81, 0x00,                                             //   Output (Data, Array, Absolute)     - Keycode Array
  0x05, 0x08,                                             //   Usage Page (LEDs)
  0x19, 0x01,                                             //   Usage Min (NumLock)
  0x29, 0x03,                                             //   Usage Max (ScrLock)
  0x15, 0x00,                                             //   Logical Min (0)
  0x25, 0x01,                                             //   Logical Max (1)
  0x75, 0x01,                                             //   Report Size (1)
  0x95, 0x03,                                             //   Report Count (3)
  0x91, 0x02,                                             //   Input (Data, Variable, Absolute)   - LED Report, 3-bits
  0x95, 0x05,                                             //   Report Count (5)
  0x91, 0x01,                                             //   Input (Constant, Array, Absolute)  - LED Padding, 5-bits
  0xC0                                                    // End Collection
};

const REPORT_DESC_LIST HID_ReportDescList [] = {
  /* INTERFACE_0 */
  { 
    (U8*)HIDMouseReport,     
    sizeof(HIDMouseReport), 
  },
  /* INTERFACE_1 */
  {
    (U8*)HID_RazerReport,
    sizeof(HID_RazerReport),
  },
   /* INTERFACE_2 */
  {
    (U8*)HIDKeyboardReport,
    sizeof(HIDKeyboardReport),
  },
};



const U8 USB_ConfigDescriptor[] = {
	
	USB_CONFIGUARTION_DESC_SIZE,                            // Configuration Descriptor (size in bytes)
  USB_CONFIGURATION_DESCRIPTOR_TYPE,                      // Configuration Descriptor ID
  WBVAL(USB_CONFIGUARTION_DESC_SIZE + (INTERFACE_NUM * USB_INTERFACE_DESC_SIZE) + (INTERFACE_NUM * HID_DESC_SIZE) + (INTERFACE_NUM * USB_ENDPOINT_DESC_SIZE)),  // Total size of Descriptors
  INTERFACE_NUM,                      // # of interfaces in this configuration
  0x01,                                                   // Index value of this configuration
  0x00,                                                   // Configuration string index
  _DEFAULT | _REMOTEWAKEUP,                               // Attributes
  250,                                                    // Max power consumption (2X mA)
                                                          // Interface 0 (HID Mouse, Boot Protocol)
  USB_INTERFACE_DESC_SIZE,                                // Keyboard Descriptor (size in bytes)
  USB_INTERFACE_DESCRIPTOR_TYPE,                          // Interface Descriptor ID
  USB_INTERFACE_0,                                        // Interface Number
  0x00,                                                   // Alternate Setting Number
  0x01,                                                   // # of endpoints in this interface
  USB_DEVICE_CLASS_HUMAN_INTERFACE,                       // Class code
  HID_SUBCLASS_BOOT,                                      // Subclass code
  HID_PROTOCOL_MOUSE,                                     // Protocol code
  0x00,                                                   // Interface string index

  HID_DESC_SIZE,                                          // HID Class Descriptor (size in bytes)
  HID_HID_DESCRIPTOR_TYPE,                                // HID Class Descriptor ID
  WBVAL(0x0111),                                          // HID Spec Release Number in BCD format
  0x00,                                                   // Country Code (0x00 for Not supported)
  0x01,                                                   // Number of class descriptors
  HID_REPORT_DESCRIPTOR_TYPE,                             // Report descriptor type
  WBVAL(sizeof(HIDMouseReport)),                          // Report descriptor size

  USB_ENDPOINT_DESC_SIZE,                                 // Endpoint Descriptor (size in bytes)
  USB_ENDPOINT_DESCRIPTOR_TYPE,                           // Endpoint Descriptor ID
  USB_ENDPOINT_IN(1),                                     // Endpoint number = 1
  USB_ENDPOINT_TYPE_INTERRUPT,                            // Attributes (Interrupt)
  WBVAL(0x0008),                                          // Max packet size (8 bytes)
  0x01,                                                   // Endpoint polling internval (1ms)
                                                          // Interface 1 (HID Keyboard, Extended)
  USB_INTERFACE_DESC_SIZE,                                // Mouse Interface Descriptor (size in bytes)
  USB_INTERFACE_DESCRIPTOR_TYPE,                          // Interface descriptor ID
  USB_INTERFACE_1,                                        // Interface Number
  0x00,                                                   // Alternate Setting Number
  0x01,                                                   // # of endpoints in this interface
  USB_DEVICE_CLASS_HUMAN_INTERFACE,                       // Class code
  HID_SUBCLASS_NONE,                                      // Subclass code
  HID_PROTOCOL_KEYBOARD,                                  // Protocol code
  0x00,                                                   // Interface string index

  HID_DESC_SIZE,                                          // HID Class Descriptor (size in bytes)
  HID_HID_DESCRIPTOR_TYPE,                                // HID Class Descriptor ID
  WBVAL(0x0111),                                          // HID Spec Release Number in BCD format
  0x00,                                                   // Country Code (0x00 for Not supported)
  0x01,                                                   // Number of class descriptors, see usb_hid.h
  HID_REPORT_DESCRIPTOR_TYPE,                             // Report descriptor type
  WBVAL(sizeof(HID_RazerReport)),                         // Report descriptor size

  USB_ENDPOINT_DESC_SIZE,                                 // Endpoint Descriptor (size in bytes)
  USB_ENDPOINT_DESCRIPTOR_TYPE,                           // Endpoint Descriptor ID
  USB_ENDPOINT_IN(2),                                     // Endpoint number = 2
  USB_ENDPOINT_TYPE_INTERRUPT,                            // Attributes (Interrupt)
  WBVAL(0x0010),                                          // Max packet size (16 bytes)
  0x01,                                                   // Endpoint polling internval (1ms)

                                                          // Interface 2 (HID KEYBOARD, Boot Protocol)
  USB_INTERFACE_DESC_SIZE,                                // Mouse Interface Descriptor (size in bytes)
  USB_INTERFACE_DESCRIPTOR_TYPE,                          // Interface descriptor ID
  USB_INTERFACE_2,                                        // Interface Number
  0x00,                                                   // Alternate Setting Number
  0x01,                                                   // # of endpoints in this interface
  USB_DEVICE_CLASS_HUMAN_INTERFACE,                       // Class code
  HID_SUBCLASS_NONE,                                      // Subclass code
  HID_PROTOCOL_KEYBOARD,                                  // Protocol code
  0x00,                                                   // Interface string index

  HID_DESC_SIZE,                                          // HID Class Descriptor (size in bytes)
  HID_HID_DESCRIPTOR_TYPE,                                // HID Class Descriptor ID
  WBVAL(0x0111),                                          // HID Spec Release Number in BCD format
  0x00,                                                   // Country Code (0x00 for Not supported)
  0x01,                                                   // Number of class descriptors, see usb_hid.h
  HID_REPORT_DESCRIPTOR_TYPE,                             // Report descriptor type
  WBVAL(sizeof(HIDKeyboardReport)),                       // Report descriptor size

  USB_ENDPOINT_DESC_SIZE,                                 // Endpoint Descriptor (size in bytes)
  USB_ENDPOINT_DESCRIPTOR_TYPE,                           // Endpoint Descriptor ID
  USB_ENDPOINT_IN(3),                                     // Endpoint number = 3
  USB_ENDPOINT_TYPE_INTERRUPT,                            // Attributes (Interrupt)
  WBVAL(0x0008),                                          // Max packet size (8 bytes)
  0x01,                                                   // Endpoint polling internval (1ms)
	
	/*
  USB_CONFIGUARTION_DESC_SIZE,                            // Configuration Descriptor (size in bytes)
  USB_CONFIGURATION_DESCRIPTOR_TYPE,                      // Configuration Descriptor ID
  WBVAL(USB_CONFIGUARTION_DESC_SIZE + (INTERFACE_NUM * USB_INTERFACE_DESC_SIZE) + (INTERFACE_NUM * HID_DESC_SIZE) + (INTERFACE_NUM * USB_ENDPOINT_DESC_SIZE)),  // Total size of Descriptors
  INTERFACE_NUM,                                          // # of interfaces in this configuration
  0x01,                                                   // Index value of this configuration
  0x00,                                                   // Configuration string index
  _DEFAULT | _REMOTEWAKEUP,                               // Attributes
  250,                                                    // Max power consumption (2X mA)
                                                          // Interface 0 (HID Mouse, Boot Protocol)
  USB_INTERFACE_DESC_SIZE,                                // Keyboard Descriptor (size in bytes)
  USB_INTERFACE_DESCRIPTOR_TYPE,                          // Interface Descriptor ID
  USB_INTERFACE_0,                                        // Interface Number
  0x00,                                                   // Alternate Setting Number
  0x01,                                                   // # of endpoints in this interface
  USB_DEVICE_CLASS_HUMAN_INTERFACE,                       // Class code
  HID_SUBCLASS_BOOT,                                      // Subclass code
  HID_PROTOCOL_MOUSE,                                     // Protocol code
  0x00,                                                   // Interface string index

  HID_DESC_SIZE,                                          // HID Class Descriptor (size in bytes)
  HID_HID_DESCRIPTOR_TYPE,                                // HID Class Descriptor ID
  WBVAL(0x0111),                                          // HID Spec Release Number in BCD format
  0x00,                                                   // Country Code (0x00 for Not supported)
  0x01,                                                   // Number of class descriptors
  HID_REPORT_DESCRIPTOR_TYPE,                             // Report descriptor type
  WBVAL(sizeof(HIDMouseReport)),                          // Report descriptor size

  USB_ENDPOINT_DESC_SIZE,                                 // Endpoint Descriptor (size in bytes)
  USB_ENDPOINT_DESCRIPTOR_TYPE,                           // Endpoint Descriptor ID
  USB_ENDPOINT_IN(1),                                     // Endpoint number = 1
  USB_ENDPOINT_TYPE_INTERRUPT,                            // Attributes (Interrupt)
  WBVAL(0x0008),                                          // Max packet size (8 bytes)
  0x01,                                                   // Endpoint polling internval (1ms)*/
};

const U8 USB_StringDescriptor[] = {
  4,                                                      // String Descriptor (size in bytes)
  0x03,                                                   // String Descriptor ID
  0x09, 0x04,                                             // English (American)

  12,                                                     // String Descriptor (size in bytes)
  0x03,                                                   // String Descriptor ID
  'R', 0x00,                                              // "Razer"
  'a', 0x00,
  'z', 0x00,
  'e', 0x00,
  'r', 0x00, 

  (1 + 34*2 + 1),                                       // String Descriptor (size in bytes)
  0x03,                                                   // String Descriptor ID
  'R', 0x00,                                              // "Razer Lancehead Tournament Edition"
  'a', 0x00,
  'z', 0x00,
  'e', 0x00,
  'r', 0x00,
  ' ', 0x00,
  'L', 0x00, 
  'a', 0x00,
  'n', 0x00,
  'c', 0x00,
  'e', 0x00,
	'h', 0x00, 
  'e', 0x00,
  'a', 0x00,
  'd', 0x00,
	' ', 0x00,
	'T', 0x00,
	'o', 0x00, 
  'u', 0x00,
  'r', 0x00,
  'n', 0x00,
  'a', 0x00,
	'm', 0x00, 
  'e', 0x00,
  'n', 0x00,
  't', 0x00,
	' ', 0x00,
	'E', 0x00,
	'd', 0x00, 
  'i', 0x00,
  't', 0x00,
  'i', 0x00,
  'o', 0x00,
	'n', 0x00, 
  0x00                                                    // Last length field = NULL
};



