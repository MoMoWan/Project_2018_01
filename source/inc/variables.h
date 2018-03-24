/*++

Copyright (c) 2012-2014 Razer Ltd.
All rights reserved. This program contains proprietary and confidential information.

Module Name:
  variables.h

Compiler:
  Keil Software µVision v5.11, with NXP Semiconductor LPC11U2x proccessor support.

Abstract:
  This file contains the extern references for variables.c, which provides the global
  variable definitions.

--*/

#ifndef _VARIABLES_H_
#define _VARIABLES_H_

#include "main.h"
#include "protocol.h"
#include "led_driver.h"

#define HW_INIT                   0x01                    // deviceState: Hardware initialized
#define VAR_INIT                  0x02                    // deviceState: Variables initialized
#define USB_INIT                  0x03                    // deviceState: USB initialized & configured

// [storage1
typedef __packed struct _STORAGE1 {                                // Special Variable Storage structure
//  __packed struct {
    U16 storageSize;                                      // Size of special Variable Storage
    U8  editionInfo[2];                                   // Edition information
    U8  devSerialNumber[22];                              // Device Serial number
//    LED_MAXM maxLight[NUMBER_OF_LEDS];                    // LED Calibration values
//  } cont;
  U16 crc;
} STORAGE1;
// ]

typedef __packed struct _BREATH_PARM {                       // LED Special Effect structure
  U16 pulseOn;                                             // Length of "On Period" for Pulsating (1ms)
  U16 pulseOff;                                            // Length of "Off Period" for Pulsating (1ms) 
	U16 pulseRiseTime;                                       // Length of "Step Rising step Time" for Pulsating (1ms)
  U16 pulseRise1Step;                                      // Size of "Rise 1 Step" in magnitude for Pulsating
  U16 pulseRise2Step;                                      // Size of "Rise 2 Step" in magnitude for Pulsating
  U16 pulseFallTime;                                       // Length of "Fall Step Time" for Pulsating (1ms)
  U16 pulseFall1Step;                                      // Size of "Fall 1 Step" in magnitude for Pulsating
  U16 pulseFall2Step;                                      // Size of "Fall 2 Step" in magnitude for Pulsating
  U16 pulseNeckLine;                                       // Size of "Neck line" of Rise or Fall 1 and 2
} BREATH_PARM;
// [storage2
typedef __packed struct _STORAGE2 {                       // Generic Variable Storage structure
  U16 storageSize;                                        // Size of generic Variable Storage
  U8  emily;                                              // Device emily Inhibit level value
  U8  makeDebounce;                                       // Mouse buttons debounce time
  U8  makeMsk;
  U8  breakDebounce;
  U8  breakMsk;
	U8  devicestatus;
	U8  ambidextrous;                                       // left hand:0x01 right hand:0x02
  U8  pollingrate; 
//	BREATH_PARM breathParam;                                // breath parameter
  U16  crc;                                               // Variable storage checksum value
} STORAGE2;
// ]

typedef __packed struct _CPI_PARM {
  U16 x;
  U16 y;
} DPI_PAR;

typedef __packed struct _STAGE_DPI_{
	U16 x;
	U16 y;
	U16 z;
}DPI_STAGE;
// [ storage3
typedef __packed struct _LIFTOFF_SENSOR_PARM {           // Totoal 19 bytes
// U8  pollingRate;                                        // Device polling rate (ms)
 U8  maxStages;                                          // Max stages of the dpi
 U8  dpiStage;                                           // DPI stages on Mouse
 U8  angleSnap; 
 U8  angleTune;
 U8  liftDis;
 DPI_STAGE stageDPI[NUMBER_OF_STAGES];                   // DPI stages value
} SENSOR_PAR;

typedef __packed struct _BTN_PARM {                       // Button Assignment structure
  U8  id;                                                 // Button ID value
	U8  mode;                                               // Button Mode :bit7-2 reserved ,bit 1:Turbo, bit 0:Hypershift
  U8  type[2];                                            // Button Function Type
  U8  size[2];                                            // Number of bytes of data
  U8  d1[2];                                              // Data byte 1[0]:normal ,1[1]:HyperShift
  U8  d2[2];                                              // Data byte 2[0]:normal ,2[1]:HyperShift
	U8  d3[2];                                              // Data byte 3[0]:normal ,3[1]:HyperShift
	U8  d4[2];                                              // Data byte 4[0]:normal ,4[1]:HyperShift
	U8  d5[2];                                              // Data byte 5[0]:normal ,5[1]:HyperShift
} BTN_PAR;

typedef __packed struct _RGB {           // Totoal 19 bytes
 U8  r;                                        // Device polling rate (ms)
 U8  g;                                          // Max stages of the dpi
 U8  b;                                           // DPI stages on Mouse
} RGBDEF;

typedef __packed struct _LED_PAR {           // Totoal 19 bytes
 U8  id;
 U8  effect;                                        // Device polling rate (ms)
 RGBDEF  color;                                          // Max stages of the dpi
 U8  speed;                                           // DPI stages on Mouse
 U8  mode; 
 U8  bright;
} LED_PAR;

typedef __packed struct _PROFILE_PAR {
  U8  profile_ID;                                         // ID
//	U32 nameAdd;                                            // Profile name address
//	U32 nameSize;                                           // Profile name size
//  U16 nameCrc;                                            // Profile name checksum 
//  U8  nameStage;                                          // Profile name setting stage, 1 is finish, 0 is not finish or error 
//	U8  Runing_light_LED_REFRSH_Time;                       // running light refrsh time(ms)
//  CHROMA_PARM savedLight[NUMBER_OF_LEDS];                 // LED Lighting parameters saved thru power cycle  ;
  LED_PAR led[NUMBER_OF_LEDS];
	DPI_PAR activeDpi;                                      // DPI setting
	SENSOR_PAR sensor;                           // Sensor Lift off parameter 
  BTN_PAR btnAssign[NUMBER_OF_BUTTONS];                 // Button assignment (Profile 0 = Default Assignment)
} PROFILE_PAR;


typedef __packed struct _STORAGE3 {                       // Generic Variable Storage structure
	U16 storageSize;                                        // Size of generic Variable Storage
  U8  makeDebounce;                                       // Mouse buttons debounce time
  U8  makeMsk;
  U8  breakDebounce;
  U8  breakMsk;
  U8  pollingrate;   
	U8  PClass;
	U8  ProNO;
	U8  aliveProfile;                                       // show the alive profile
	PROFILE_PAR profile[profileNO];
	U16  crc;                                               // Variable storage checksum value
} STORAGE3;
// ]


extern U8  deviceMode;
extern U8  deviceState;
extern U8  wakeupEvents;
extern U16 milliseconds;

extern U8  ntfType;
extern U8  ntfValue;

extern U8  calStatus;
extern U8  calEnable;

extern U8  updateStorage;
extern U16 updateOneshot;
extern U8  usbReceiveFlag;
extern U8  usbReceivelen;
extern U8  protocolTransmit[PROTOCOL_DATA_SIZE];
extern const U8 firmwareVersion[4];


extern STORAGE1 sys;
//extern STORAGE2 user;
extern STORAGE3 user;//user;

extern const STORAGE1 dft_special;
extern const STORAGE2 dft_generic1;
extern const STORAGE3 dft_generic2;
extern U8  updateLeftProfile;                                  
extern U8  updateLeftMacro;
extern const COLOR DPICOLOR[5];
  
extern U8 pollingChange;
extern U8 blinkCount;
extern U8 pollingTimer;  
extern volatile uint16_t	dbMS_LED_ReflashTimeFrame[5];  
  
#if (EP_OVERWRITE_PROTECT)
extern U8 Endpoint_BusyFlag[4];

extern U8 SofCnt[4] ; 
#define isEpBusy(n) (Endpoint_BusyFlag[n])
#define setEpBusy(n)   {Endpoint_BusyFlag[n]  = 1;SofCnt[n] =2;}
#define freeEp(n)     {Endpoint_BusyFlag[n]  = 0;SofCnt[n] =0;}
#endif
extern  uint8_t  USB_Configuration;

#endif // _VARIABLES_H_

