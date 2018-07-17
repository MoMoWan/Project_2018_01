/*++

Copyright (c) 2012-2014 Razer Ltd.
All rights reserved. This program contains proprietary and confidential information.

Module Name:
  Mouse.h

Compiler:
  Keil Software µVision v5.11, with NXP Semiconductor LPC11U2x proccessor support.

Abstract:
  This file provides the header of mouse related function, types

--*/


#ifndef _MOUSE_H_
#define _MOUSE_H_

#include "main.h"
#include "variables.h"
#define MAX_EP_SIZE                   (16)                // 16 bytes normal key packet
#define REPORT_SIZE_MOUSE             (8)                 // 8 bytes mouse data report size
#define REPORT_SIZE_KB                (8+1)                 // 8 bytes boot keyboard report size
#define REPORT_SIZE_RAZER             (16)                // 16 bytes razer ep report size

#define NOKEY                          (0)

#define REPORT_ID_KEY                 (1)                 // Extended standary key report ID
#define REPORT_ID_MEDIA               (2)                 // Media key report ID
#define REPORT_ID_POWER               (3)                 // Power key report ID
#define REPORT_ID_RAZER               (4)                 // Razer key report ID
#define REPORT_ID_NTF                 (5)                 // Notification key report ID

#define NTF_PROFILE                   (1)                 // Profile change notification event
#define NTF_DPI                       (2)                 // DPI change notification event
#define NTF_DETACHMENT                (3)                 // Detachment notification event
#define NTF_ATTACHMENT                (4)                 // Attachment notification event
#define NTF_LIFTED                    (5)                 // Lifted notification event
#define NTF_DOWNED                    (6)                 // Downed notification event
#define NTF_GESTURE_DETECTED          (7)                 // Gesture detected notification event
#define NTF_BRIGHTNESS                (8)                 // Brightness change notification event
#define NTF_RF_RECONNECT              (9)                 // Wireless reconnct event notification event
#define NTF_CAL_ID                    (0x0A)              // Calibration event change notification event type

#define EVENT_BUTTON                  (1 << 0)
#define EVENT_SCROLLWHEEL             (1 << 1)
#define EVENT_POSITION                (1 << 2)
#define EVENT_KEY                     (1 << 3)             
#define EVENT_MEDIA                   (1 << 4)
#define EVENT_POWER                   (1 << 5)
#define EVENT_RZ_KEY                  (1 << 6)            // Razer key event
#define EVENT_NTF                     (1 << 7)            // Razer notification key event

#define NTF_CAL_START           0x00                      // Calibration start, Such as: Press left button and start moving to cover the mat 
#define NTF_CAL_END             0x01                      // Calibration end, Such as: Release left button after moving to cover the mat
#define NTF_CAL_HALT            0x02                      // Calibration halted, Such as: User never press left button to start calibration process
#define NTF_CAL_RESET           0x03                      // Calibration reset, such as: Press L+M+R and hold 5s to reset all calibration setting
#define NTF_CAL_SPEED_NORMAL    0x04                      // Calibration under normal speed
#define NTF_CAL_SPEED_OVER      0x05                      // Calibration over speed, SW may need feedback user to slow down the moving
#define NTF_CALIBRATION         (0x0A)                    // Calibration change notification event

#define TIMEOUT_MINI_CAL        35000                     // 35sec = 35000ms
#define TIMEOUT_MANUAL_CAL      60000                     // 60sec = 60000ms
#define TIMEOUT_PREPARE         35000                     // 35sec  

#define TURBO_NONE	            0x00                      // Turbo mode, disable
#define TURBO_MAKE              0x01                      // Turbo mode , make stage
#define TURBO_BREAK             0x02                      // Turbo mode , break stage
#define TURBO_END               0x03                      // Turbo mode , end stage
typedef enum {
  CAL_S_OFF = 0,                      // Calibration stage OFF
  CAL_S_PREPARE = 1,                  // Calibration stage Prepare
  CAL_S_READY = 2,                    // Calibration stage Ready
  CAL_S_SET = 3,                      // Calibration stage Set
  CAL_S_START = 4,                    // Calibration stage Start
  CAL_S_RESET = 5,                    // Calibration stage Reset
  CAL_S_ZDATA = 6,                    // Calibration stage to report Z data to SW
  CAL_S_WAIT = 7,                     // Calibration stage wating, After SW read back the cal data, the FW need waiting untill SW set down new cal data to mouse
  CAL_S_ABORT = 8,                    // Calibration stage Abort
  CAL_S_END = 9,
  CAL_S_REBOOT = 10,
} CalEnum;            // Calibration stage Erase FLASH


typedef __packed struct _SCROLL_{
	U8 upTimer;
	U8 upFlag;
	U8 dnTimer;
	U8 dnFlag;
}SCROLL;

#define BTNS                    0                         // Mouse Report: Buttons
#define X_SM                    1                         // Mouse Report: Delta X (byte)
#define Y_SM                    2                         // Mouse Report: Delta Y (byte)
#define Z_SM                    3                         // Mouse Report: Delta Z (byte)
#define X_LO                    4                         // Mouse Report: Delta X (lower byte)
#define X_HI                    5                         // Mouse Report: Delta X (upper byte)
#define Y_LO                    6                         // Mouse Report: Delta Y (lower byte)
#define Y_HI                    7                         // Mouse Report: Delta Y (upper byte)


//Scroll Wheel HW Map
#define   ENCODER_DN  ((uint32_t)(SN_GPIO2->DATA  >> 4) & 0x01)     // P0.19 Scroll Wheel encoder Up signal
#define   ENCODER_UP  ((uint32_t)(SN_GPIO2->DATA  >> 3) & 0x01)     // P1.18 Scroll Wheel encoder Down signal

//=============================//
//      Wheel case       			 //
//=============================//
#define		S_WHEEL_BA_LL  		0x00
#define		S_WHEEL_BA_LH  		0x01
#define		S_WHEEL_BA_HL 		0x02
#define		S_WHEEL_BA_HH 		0x03


#define		WHEEL_BA_LL  					0x00
#define		WHEEL_BA_LH  					0x01
#define		WHEEL_BA_HL 					0x02
#define		WHEEL_BA_HH 					0x03
#define		WHEEL_BA_STABLE_LL 		(0x01<<2)
#define		WHEEL_BA_STABLE_HH 		(0x02<<2)

#define		mskWheel_CurrentStatus		(0x03 << 0)
#define		mskWheel_StableStatus		(0x0C << 0)
#define		mskWheel_Clockwise 			(0x01<<7)
#define		mskWheel_CounterClockwise 	(0x01<<6)

#define		mskWheel_JustStable 		(0x01<<5)

#define		WHEEL_DEBOUNCE_COUNT				8 //** 2ms
#define		WHEEL_NEW_DEBOUNCE_VALUE		(WHEEL_DEBOUNCE_COUNT)	

#define		mskWHEEL_A_BIT0		(0x01 << 0)
#define		mskWHEEL_B_BIT1		(0x01 << 1)

#define 	ZERO 	0x00
#define 	mskLOW_NIBBLE  0x0F
#define 	HIGH_NIBBLE    0xF0
#define 	COUNT_FINISH   0x00

extern U8  optical;

extern S16 xCount;
extern S16 yCount;
extern S8  zCount;
extern U8  mouseEvent;

extern U8 Hid_Ep1_Report_In[MAX_EP_SIZE];
extern U8 Hid_Ep3_Report_In[MAX_EP_SIZE];


extern U8  mouseEvent;
extern U8  mouseLifted;
extern U8  FilterState;
extern U8 CAL_LiftConfig;
extern U8 CAL_LiftCal2;
extern U8 CAL_LiftCal3;
extern U8 CAL_Threshold;
extern U8 CAL_LiftCal4;
extern U8 DisableCalibraiton;
extern U8 sampleCountSqual;
extern U8 AutoCalibrationFalg;
extern U16 HaltTimer1;
extern U32 HaltTimer2;
extern U8 changeDPI;
extern U8 startFlag;

//extern U16 liftOffThreshold;
//extern U16 MEASURERESULT;
//extern U8 minValueSqual;
extern U8 maxShutterHi;
extern U8 minShutterLo;
extern U8 AutoCalibrationFalg;
extern U16 CalibrationTimer;
extern U8 ledLock;
extern U8 maxShutterHi;
extern U8 minShutterLo;
extern U8 manualState;
extern U8 clearFlag;

//extern LIFTOFF_EXT_PARM liftoff;
extern SENSOR_PAR IRSensor;
extern DPI_PAR dpiCurrent;
extern U8 ResetHalFlag;

extern SCROLL scrollTimer;                                       // Use for scroll release even triger
void initializeMouseVarialbes  (void);
void opticalSensorPolling (void);
void processPendingTasks  (void);	
void scrollWheelHandling(void);
//void calResetChking(void);
void UniversityLiftoff(void);
void DoCalibration (void);
void manualCalibration(void);
void AutoCalibrationReset(void);
void copyLiftoff2Active(U8 function);
extern void CalibrationDate(U8 cal2,U8 cal3,U8 config, U8 cal4);
extern void requestNtfEvent(U8 type, U8 value);
extern void MS_WheelInit(void);

void setResolutionTemp(U16 x,U16 y);
#define ABSD(x) ((x > 0x80) ? ((0xFF - x) + 1) : x)


#if FW_DPI_EN                                                     // Firmware doing DPI scale
U8 zoomInDPI(S16 *x, S16 *y, U16 dpiX, U16 dpiY);
#endif

#endif // _MOUSE_H_
