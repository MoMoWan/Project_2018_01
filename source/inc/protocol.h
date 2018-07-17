/*++

Copyright (c) 2012-2014 Razer Ltd.
All rights reserved. This program contains proprietary and confidential information.

Module Name:
  protocol.h

Compiler:
  Keil Software µVision v5.11, with NXP Semiconductor LPC11U2x proccessor support.

Abstract:
  This file contains the definitions for protocol.c, which provides the support for
  the driver to device command interface.

--*/

#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include "main.h"
#include "macro.h"
#include "led_driver.h"
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

//#define INFO_CMD                        0x00              // Command Class: Device Information
//#define OTF_SETTINGS                    0x01              // Command Class: On-the-fly Settings
//#define BUTTON_CMD                      0x02              // Command Class: Mouse Button command
//#define LED_CMD                         0x03              // Command Class: LED command
//#define DPI_CMD                         0x04              // Command Class: DPI command
////#define PROFILE_CMD                     0x05              // Command Class: Profile command
//#define MACRO_CMD                       0x06              // Command Class: Macro command
//#define POWER_CMD                       0x07              // Command Class: Power Mgmt command
//#define AUDIO_CMD                       0x08              // Command Class: Audio command
//#define FLASH_REGION_CMD                0x0A              // Command class: Flash region command
//#define SENSOR_CMD                      0x0B              // Command Class: Proximity command
//#define CHROMA_CMD                      0x0F              // Command Class: Chroma command
#define INFO_CMD                        0x00              // Command Class: On-the-fly Settings
#define LED_CMD                         0x01              // Command Class: On-the-fly Settings
#define BUTTON_CMD                      0x02              // Command Class: Mouse Button command
#define MACRO_CMD                       0x03              // Command Class: Macro command
#define SENSOR_CMD                      0x04              // Command Class: Proximity command


#define BUFFER_EMPTY                    0x00              // Status: Buffer Empty
#define COMMAND_BUSY                    0x01              // Status: Command in progress
#define RESPONSE_READY                  0x02              // Status: Response Ready
#define COMMAND_ERROR                   0x03              // Status: Command Error
#define COMMAND_NO_Response_Timeout     0x04              // Status: Comman No Response Timeout
#define COMMAND_NOTSUPPORT              0x05              // Status: Command Not Support

//#define CAPABILITIES                    0x00              // INFO_CMD: Device Capabilities
//#define FIRMWARE_VER                    0x01              // INFO_CMD: Firmware Version
//#define SERIAL_NUMBER                   0x02              // INFO_CMD: Serial Number
//#define PROTOCOL_VER                    0x03              // INFO_CMD: Protocol Version
//#define DEVICE_MODE                     0x04              // INFO_CMD: Device Mode
//#define POLLING_PERIOD                  0x05              // INFO_CMD: Device Polling Period
//#define EDITION_INFO                    0x06              // INFO_CMD: Device Edtion Informations
//#define EXT_FIRMWARE_VER                0x07              // INFO_CMD: Extended Firmware Version
//#define GAME_MODE                       0x08              // INFO_CMD: Game Mode state
//#define WAKEUP_EVENT                    0x09              // INFO_CMD: Device wakeup events
//#define DEVICE_RESET                    0x0B              // INFO_CMD: Device reset
//#define DOCK_FW_VERSION                 0x11              // INFO_CMD: Dongle Firmware Version
//#define DOCK_SERIAL_NO                  0x12              // INFO_CMD: Dongle Serial Number
//#define DOCK_EXT_FW_VER                 0x13              // INFO_CMD: Dongle Extended FW Version
//#define VARIABLE_INFO                   0x14              // INFO_CMD: Variable Storage Information
//#define DOCK_MID_CODE                   0x15              // INFO_CMD: Dock wireless module MID code
//#define EMILY_INHIBIT                   0x20              // INFO_CMD: 0=Enabled, 1=Disabled, 2=Limited
//#define AMBIDEXATROUS                   0x33              // INFO_CMD :0=right hand ,1=left hand
//#define BOOTLOADER_FW_VER               0x9F              // INFO_CMD: Bootloader Firmware Version    

//[device info CLASS 
#define DEVICE_MODE                     0x00              // LED_CMD: LED Effect Parameter
#define FW_VERSION                      0x01              // LED_CMD: LED RGB Parameters
#define MATRIX_INFO                     0x02              // LED_CMD: LED RGB Parameters
#define DEBOUNCE_INFO                   0x03              // LED_CMD: LED RGB Parameters
#define POLLING_INFO                    0x04              // LED_CMD: LED RGB Parameters
#define LAYOUT_INFO                     0x05              // INF_CMD: KEYBOARD LAYOUT Information
#define DEVICE_RESET                    0x06              // INF_CMD: KEYBOARD LAYOUT Information
#define PROFILE_DPI                     0x07              // INF_CMD: MOUSE DPI Information

//#define BUTTON_FUNCTION                 0x00              // BUTTON_CMD: Signal Button Function
//#define BUTTON_SWITCH                   0x01              // BUTTON_CMD: Hard Switch State
//#define BUTTON_DEBOUNCE                 0x02              // BUTTON_CMD: Button Debounce Time
//#define BUTTON_ID_LIST                  0x04              // BUTTON_CMD: Get button id list
//#define BTN_SINGLE_ASSIGN               0x0C              // BUTTON_CMD: Single button assign 
//#define BTN_ASSIGN_LIST                 0x0E              // BUTTON_CMD: Single button assign list

#define BUTTON_ID_LIST                  0x00              // BUTTON_CMD: Get button id list
#define BTN_SINGLE_ASSIGN               0x01              // BUTTON_CMD: Single button assign 


//#define LED_STATE                       0x00              // LED_CMD: LED State
//#define LED_RGB_PARAM                   0x01              // LED_CMD: LED RGB Parameters
//#define LED_EFFECT                      0x02              // LED_CMD: LED Effect Parameter
//#define LED_BRIGHTNESS                  0x03              // LED_CMD: LED Brightness
//#define LED_BLINK_PARAM                 0x04              // LED_CMD: LED Blinking Parameters
//#define LED_PULSATE_PARAM               0x05              // LED_CMD: LED Pulsating Parameters
//#define LED_RGB_BRIGHTNESS              0x06              // LED_CMD: LED RGB Brightness
//#define LED_TRANSITION                  0x07              // LED_CMD: LED Transition Parameters
//#define LED_RGB_MAXIMUM                 0x08              // LED_CMD: LED RGB Maximum Values
//#define LED_LST                         0x09              // LED_CMD: LED list
//#define LED_EXT_PULSATING_PARAM         0x0E              // To set led pulsating colors


#define LED_EFFECT                      0x00              // LED_CMD: LED State
#define LED_RGB_PARAM                   0x01              // LED_CMD: LED RGB Parameters
#define LED_BRIGHTNESS                  0x02              // LED_CMD: LED Effect Parameter
#define DPI_LED_COLOR                   0x03              // LED_CMD: LED Brightness

#define DPI_ACTIVE_STAGE                0x00              // DPI_CMD: DPI Active Stage index
#define DPI_ACTIVE_DPI                  0x01              // DPI_CMD: DPI Active DPI values
#define DPI_CLASS_STAGE                 0x02              // DPI_CMD: DPI Class Single Stage values
#define DPI_CLASS_TABLE                 0x03              // DPI_CMD: DPI Class Stage Table values
#define DPI_PROFILE_STAGE               0x04              // DPI_CMD: DPI Profile Active Stage index
#define DPI_PROFILE_DPI                 0x05              // DPI_CMD: DPI Profile Active DPI values
#define DPI_STAGE_TABLE                 0x06              // DPI_CMD: DPI stage profile setting
#define DPI_PROXIMITY                   0x09              // DPI_CMD: Proximity Sensor parameters
#define DPI_RAW_PROX                    0x0A              // DPI_CMD: Proximity Raw ADC value
#define DPI_PROX_CTRL                   0x0B              // DPI_CMD: Proximity Control
//#define DPI_MAX_SATGES                  0x0C              // DPI_CMD: DPI max stages


//#define PROXIMITY_SENSOR_LST            0x00              // PROXIMITY_SENSOR_CMD: Proximity snesor ID list
//#define PROXIMITY_SENSOR_CALIBRATION    0x01              // PROXIMITY_SENSOR_CMD: Proximity snesor calibation
//#define PROXIMITY_SENSOR_THRESHOLD      0x02              // PROXIMITY_SENSOR_CMD: Proximity sensor threshold
//#define PROXIMITY_SENSOR_STATE          0x03              // PROXIMITY_SENSOR_CMD: Proximity sensor state
//#define PROXIMITY_SENSOR_HARDWARE       0x04              // PROXIMITY_SENSOR_CMD: Proximity sensor hardware
//#define PROXIMITY_SENSOR_CONFIGURATION  0x05              // PROXIMITY_SENSOR_CMD: Proximity sensor configuration
//#define PROXIMITY_SENSOR_MANUAL         0x06              // PROXIMITY_SENSOR_CMD: Proximity sensor calibration data
//#define PROXIMITY_SENSOR_CTRL           0X09              // PROXIMITY_SENSOR_CMD: Proximity sensor control
//#define PXM_LIFT_LED_INDICATOR          0x0a              // lift-off LED dim 

#define SENSOR_STATUS                     0x00              // PROXIMITY_SENSOR_CMD: Proximity snesor calibation
#define SENSOR_THRESHOLD                  0x01              // PROXIMITY_SENSOR_CMD: Proximity sensor threshold
#define SENSOR_CONFIGURATION              0x02              // PROXIMITY_SENSOR_CMD: Proximity sensor configuration 
 
#define PROFILE_NUMBER                  0x00              // PROFILE_CMD: Get Number of Profiles
#define PROFILE_ID                      0x01              // PROFILE_CMD: Profile ID list
#define PROFILE_NEW                     0x02              // PROFILE_CMD: Set new Profile
#define PROFILE_DEL                     0x03              // PROFILE_CMD: Delete Profile
#define PROFILE_ACTIVE                  0x04              // PROFILE_CMD: Active Profile
#define PROFILE_NAME                    0x05              // PROFILE_CMD: Profile Name
#define PROFILE_NAME_MASS               0x08              // PROFILE_CMD: Profile Name  mroe than 90 bytes
#define PROFILE_MAX                     0x0A              // PROFILE_CMD: Max profile numbers

//#define MACRO_NUMBER                    0x00              // MACRO_CMD: Get Number of Macro
//#define MACRO_ID                        0x01              // MACRO_CMD: Macro ID list
//#define MACRO_NEW                       0x02              // MACRO_CMD: Set new Macro
//#define MACRO_DEL                       0x03              // MACRO_CMD: Delete Macro
//#define MACRO_NAME                      0x04              // MACRO_CMD: Macro Name
//#define MACRO_DATA                      0x05              // MACRO_CMD: Macro Data
//#define MACRO_STORAGE                   0x06              // MACRO_CMD: Macro Storage
//#define MACRO_MINIDELAY                 0x07              // MACRO_CMD: Macro Minimum Delay
//#define MACRO_ALLOCATE                  0x08              // MACRO_CMD: ALLOCATE Macro Data Size     
//#define MACRO_MEMORY                    0x09              // MACRO_CMD: Macro data setting 
//#define MACRO_MEM_MANAGE                0x0A              //
//#define MACRO_MASS_ID                   0x0B              // MACRO_CMD: macro Ia
//#define MACRO_MASS_NAME                 0x0C              // MACRO_CMD: Macro name will offset
//#define MACRO_RAW_DATA                  0x0D              // MACRO_CMD: Macro raw data dump
//#define MACRO_STORAGE_INFOR             0x0E


#define MACRO_INFO                      0x00              // MACRO_CMD: Get Number of Macro
#define MACRO_ID                        0x01              // MACRO_CMD: Macro ID list
#define MACRO_NEW                       0x02              // MACRO_CMD: Set new Macro
#define MACRO_DEL                       0x03              // MACRO_CMD: Delete Macro
#define MACRO_MINIDELAY                 0x04              // MACRO_CMD: Macro Minimum Delay
#define MACRO_DATA                      0x05              // MACRO_CMD: Macro Data
#define MACRO_MEM_MANAGE                0x06              //


#define POWER_BATTERY                   0x00              // POWER_CMD: Get Battery Level
#define POWER_CRITICAL                  0x01              // POWER_CMD: Battery Critical Level
#define POWER_MAX_LIGHT                 0x02              // POWER_CMD: Maximum Light Drive Level
#define POWER_SLEEP_TIME                0x03              // POWER_CMD: Time to Sleep
#define POWER_CHARGING                  0x04              // POWER_CMD: Get Charging Status

#define PXM_SENSOR_LST                  0x00              // PROXIMITY_CMD: Proximity sensor ID list
#define PXM_CALIBRATION                 0x01              // PROXIMITY_CMD: Proximity sensor Calibration Prepare
#define PXM_SENSOR_THRESHOLD            0x02              // PROXIMITY_CMD: Proximity sensor  Threshold
#define PXM_SENSOR_STATE                0x03              // PROXIMITY_CMD: Proximity sensor State, enable or disable
#define PXM_SENSOR_HW                   0x04              // PROXIMITY_CMD: Proximity sensor Hardware
#define LOW_SPEED_SENSE                 0x05              // Low speed sense setting for SW control, test for now
#define PXM_CAL_DATA                    0x08              // Sensor calibration data, test for now
#define PXM_CALIBRATION_CTRL            0x09              // PROXIMITY_CMD: Proximity sensor Calibration Control

#define CHROMA_REGION_ID                0x00              // CHROMA_CMD: Chroma Region ID list
#define CHROMA_EFFECT_ID                0x01              // CHROMA_CMD: Chroma Effect ID list
#define CHROMA_EFFECT                   0x02              // CHROMA_CMD: Chroma Effect
#define CHROMA_FRAME                    0x03              // CHROMA_CMD: Chroma Frame
#define CHROMA_BRIGHTNESS               0x04              // CHROMA_CMD: Chroma Brightness
#define CHROMA_CALIBRATION_DATA         0x05							// CHROMA_CMD: Chroma Calibration Data 

//#define LED_SCROLL_ID                   0x01                   // Chroma Region ID: scroll wheel LED
//#define LED_LOGO_ID                     0x02                   // Chroma Region ID: DPI LED
//#define LED_TAIL_ID                     0x03                   // Chroma Region ID: Battery LED

// [ Test cmd
#define PXM_ID_LIFTED                   0x01
#define PXM_ID_L2R                      0x02
#define PXM_ID_R2L                      0x03
#define PXM_ID_AVAGO                    0x04
#define PXM_ID_PHILIP                   0x05

#define CAL_CTRL                        0x05              // CAL_CMD: Sensor calibration Control: Enable or Disable
#define CAL_CTRL_TYPE                   0x06              // CAL_CMD: Sensor calibration Control Type: Auto or Manaul
#define CAL_HEIGHT                      0x07              // CAL_CMD: Sensor calibration lift off threshold setting
#define CAL_DATA                        0x08              // CAL_CMD: Sensor calibration lift off parameters setting
// ]

#define LED_GPIO                        0x01              // Led type: GPIO
#define LED_PWM                         0x02              // Led Type: PWM
#define LED_TRI_COLOR                   0x03              // Led Type: Tri-Color, RGB led

#define EMILY_FULLY                     0x00              // Emily cmd: fully funciton
#define EMILY_LIMITED                   0x01              // Emily cmd: limited function
#define EMILY_DISABLE                   0x02              // Emily cmd: disable device

#define RESET_DEVICE                    0x00
#define RESET_VARIABLES                 0x01
#define RESET_SECTION_1                 0x02

#define FLASH_INITIAL                   0x00             // Macro Memory Management , initia
#define FLASH_ONGOING                   0x01             // Macro Memory Management , doing
#define FLASH_COMPLETED                 0x02             // Macro Memory Management , completed
#define FLASH_ERROR                     0x03             // Macro Memory Management , error

#define ACTION_NONE                     0x00             // Do nothing
#define ACTION_CHECK_DATA               0x01             // Check all data ,include macro data and name and macro table
#define ACTION_FORMAT                   0x02             // Format all macro memory
#define ACTION_CHECK_IDDATA             0x03             // Check data by macro id

#define DATA_BUFFER_SIZE (0x40)   //(0x100)

typedef struct _MULTI_PACKET {                            // Multi-packet information structure
  U16 number;                                             // Current Packet Number
  U16 offset;                                             // Current Offset into Data
  U16 id;                                                 // Corresponding packet ID
} MULTI_PACKET;

typedef __packed struct _EEPROM_SAVE_{
	U8 profile;
	U8 macro[MAX_MACRO/8+1];
}EEPROM_SAVE;

extern U8 protocolBuffer[DATA_BUFFER_SIZE];
extern EEPROM_SAVE eepromSave;
extern volatile U8 usbBusyFlag;                            // To set the flag if any get/set command arrive
void processPendingCommand(void);
void processDeviceInfoCommand(void);
void processButtonCommand(void);
void processLedCommand(void);
void processDpiCommand(void);
void processProfileCommand(void);
void processProximityCommand(void);
void requestVariableUpdate(U8 source,U8 updatIndex);
void processVariableUpdate(void);
void processChromaCommand(void);
void updateLEDeffectParm (U8 led_id);
void CopyOthersProfileParm(U8 cnt);
void processMacroCommand(void);
void profileLedComand(U8 id)  ;
void updateProfileLed(CHROMA_PARM *ramLed,U8 profile) ;
#endif // _PROTOCOL_H_
