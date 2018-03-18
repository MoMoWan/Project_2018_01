/*++

Copyright (c) 20016 X-valley Ltd.
All rights reserved. This program contains proprietary and confidential information.

Module Name:
  usb_protocol.h

Compiler:
  Keil Software uVision v5.18, with ST Semiconductor STM32F072C8 proccessor support.

Abstract:
  This file contains the definitions for usb_protocol.c, which provides all of the
  keyboard functions support.

--*/

#ifndef _USB_PROTOCOL_H_
#define _USB_PROTOCOL_H_

#include "main.h"

#define RND_M 2147483647                                  // Random number generator terms
#define RND_A 16807
#define RND_Q (RND_M / RND_A)
#define RND_R (RND_M % RND_A)



#define GET                             0x80              // Get Command (Bit 7 = 1)
#define SET                             0x00              // Set Command (Bit 7 = 0)

#define STATUS                          0x00              // Protocol Header: Status byte
#define TRANS_ID                        0x01              // Protocol Header: Transaction ID
#define PACKET_H                        0x02              // Protocol Header: Packet Number word
#define PACKET_L                        0x03
#define RESERVED                        0x04              // Protocol Header: Reserved
#define DATA_SIZE                       0x03              // Protocol Header: Payload size
#define COMMAND_CLASS                   0x04              // Protocol Header: Command Class
#define COMMAND_ID                      0x05              // Protocol Header: Command ID
#define PAYLOAD                         0x06              // Protocol Payload Start
#define CHECKSUM                        0x3F              // Protocol Checksum location

#define BUFFER_EMPTY                    0x00              // Status: Buffer Empty
#define COMMAND_BUSY                    0x01              // Status: Command in progress
#define RESPONSE_READY                  0x02              // Status: Response Ready
#define COMMAND_ERROR                   0x03              // Status: Command Error
#define LOST_CONNECT_ERROR              0x04              // Status: Lost connect error for wireless project
#define COMMAND_NOTSUPPORT              0x05              // Status: Command Not Support

#define INFO_CMD                        0x00              // Command Class: On-the-fly Settings
#define LED_CMD                         0x01              // Command Class: On-the-fly Settings

//[device info CLASS 
#define DEVICE_MODE                     0x00              // LED_CMD: LED Effect Parameter
#define FW_VERSION                      0x01              // LED_CMD: LED RGB Parameters
#define MATRIX_INFO                     0x02              // LED_CMD: LED RGB Parameters
#define DEBOUNCE_INFO                   0x03              // LED_CMD: LED RGB Parameters
#define POLLING_INFO                    0x04              // LED_CMD: LED RGB Parameters
#define LAYOUT_INFO                     0x05              // INF_CMD: KEYBOARD LAYOUT Information
#define DEVICE_RESET                    0x06              // INF_CMD: KEYBOARD LAYOUT Information
//#define MOUSE_DPI                       0x07              // INF_CMD: MOUSE DPI Information
#define PROFILE_DPI                     0x07              // INF_CMD: MOUSE DPI Information

//[ LED CLASS 
#define LED_EFFECT                      0x00              // LED_CMD: LED Effect Parameter
#define LED_RGB_PARAM                   0x01              // LED_CMD: LED RGB Parameters
#define LED_BRIGHTNESS                  0x02              // LED_CMD: LED Effect Parameter
#define LED_SOFTWARE_CONTROL            0x03              // LED_CMD: LED RGB Maximum Values
#define LED_DYN_EFFECT                  0x04              // LED_CMD: LED RGB Parameters
#define INPUT_COLOR                     0x05              // LED_CMD: LED Effect Parameter
#define INPUT_BRIGHTNESS                0x06              // LED_CMD: LED Effect Parameter
#define LED_RGB_MAXIMUM                 0x08              // LED_CMD: LED RGB Maximum Values




//extern const U8 VERSIONNUMBER[3];


void processPendingCommand(void);
void requestVariableUpdate(uint8_t source);
uint8_t flashProgram(uint32_t addr, uint32_t *data, uint32_t size);
void processVariableUpdate(void);
uint16_t crc16(U8* pData, U16 len);
uint8_t crc8(const U8 *data, U8 len);
U32 randomGenerate(U32 lower, U32 upper);
void memoryCopy(void* dst, void* src, U16 count);
void memorySet(void* dst, U8 v, U16 count);
void processLedCommand(void);
void processDeviceInfoCommand(void);











#endif
