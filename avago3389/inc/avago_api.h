/*++

Copyright (c) 2009-2010 Razer Ltd.
All rights reserved. This program contains proprietary and confidential information.

Module Name:
  PMW3389_api.h

Compiler:
  Freescale CodeWarrior IDE v6.0, with support for MC9S08JM60 proccessor.

Abstract:
  This file contains the definitions for the PMW3389_api.c, which provides the 
  interface and support routines for the Avago SDNS-3668 Sensor.

--*/

#ifndef _PWM3389_API_H_
#define _PWM3389_API_H_

#include "Type.h"

#define	WRITE_BIT		(0x01 << 7)
#define	READ_BIT		(0x00 << 7)	

#define REG_PRODU CT_ID                 0x00    // Address for Product ID register
#define REG_REVISION_ID                 0x01    // Address for Revision ID register
#define REG_MOTION                      0x02    // Address for Motion register
#define REG_DELTA_X_L                   0x03    // Address for Delta_X low byte register
#define REG_DELTA_X_H                   0x04    // Address for Delta_X high byte register
#define REG_DELTA_Y_L                   0x05    // Address for Delta_Y register
#define REG_DELTA_Y_H                   0x06    // Address for Delta_x register
#define REG_SQUAL                       0x07    // Address for Surface Quality
#define REG_PIXEL_SUM                   0x08    // Address for pixel sum
#define REG_MAXIMUM_PIXEL               0x09    // Address for maximum pixel
#define REG_MINIMUM_PIXEL               0x0A    // Address for minimum pixel
#define REG_SHUTTER_LOWER               0x0B    // Address for shutter lower
#define REG_SHUTTER_UPPER               0x0C    // Address for shutter upper
#define REG_CONTROL1                    0x0D    // Address for control1
#define REG_RESOLUTION_L                0x0E    // Address for resolution lower byte
#define REG_RESOLUTION_H                0x0F    // Address for resolution high byte
#define REG_CONFIGURATION_II            0x10    // Address for the X and Y CPI reporting mode
#define REG_ANGLE_TUNE                  0x11    // Address for angle tune
#define REG_FRAME_CAPTURE               0x12    // Address for frame capture
#define REG_SROM_ENABLE                 0x13    // Address for srom enable Must write key here before download srom

#define REG_OBSERVATION                 0x24    // Address for indicates whether SROM is running
#define REG_DATA_OUT_LOWER              0x25    // Address for lower byte of the date_out register
#define REG_DATA_OUT_UPPER              0x26    // Address for SROM CRC test register
#define REG_SROM_ID                     0x2A    // Address for SROM ID
#define REG_MIN_SQUAL_RUN               0x2B    // Address for minimum aquual threshold
#define REG_PIXEL_THRESHOLD             0x2C    // Address for SQUAL value register
#define REG_CONTROL2                    0x2D    // Address control 2
#define REG_CONFIG5_L                   0x2E    // Address lower byte of the config5 register
#define REG_CONFIG5_H                   0x2F    // Address high byte of the config5 register

#define REG_POWER_UP_RESET              0x3A    // Address power up reset
#define REG_SHUTDOWN                    0x3B    // Address shoutdown
#define REG_INVERSER_PRODUCT_ID         0x3F    // Address inverse product ID
#define REG_LIFTCUTOFF_CAL3						  0x41    // Address lift cutoff calibration 3
#define REG_ANGLE_SNAP                  0x42    // Address angle snap
#define REG_LIFTCUTOFF_CAL1             0x4A    // Address lift cutoff calibration 1
#define REG_MOTION_BURST                0x50    // Address motion burst
#define REG_SROM_LOAD_BURST             0x62    // Address SROM load burst
#define REG_LIFT_CONFIG                 0x63    // Address lift detection height threshold
#define REG_PIXART_BURST                0x64    // Address pixel burst
#define REG_LIFTCUTOFF_CAL2             0x65    // Address lift cutoff calibration 2
#define REG_LIFTCUTOFF_CAL_TIMEOUT      0x71    // Address lift cutoff calibration timeout
#define REG_LIFTCUTOFF_CAL_MIN_LENGTH   0x72    // Address lift cutoff calibration length threshold
#define REG_PWM_PERIOD_CNT              0x73    // Address PWM period cnt
#define REG_PWM_WIDTH_CNT               0x74    // Address PWM width cnt
#define REG_SQUAL_THRESHOLD             0x76    // Address PWM AQUAL threshold
#define REG_LIFTCUTOFF_CAL4             0x77
#define	SENSOR_POWER_DOWN				        0xB6
#define MOT_BIT                         0x80    // REG_MOTION: Motion
#define LP_VALID_BIT                    0x20    // REG_MOTION: Check for valid LP_CFG0 and LP_CFG1_registers
#define OVF_BIT                         0x10    // REG_MOTION: Overflow

#define VALID_LOWER_CRC                 0xEF    // REG_DATA_OUT_LOWER: Correct CRC lower byte
#define VALID_UPPER_CRC                 0xBE    // REG_DATA_OUT_UPPER: Correct CRC upper byte

#define SROM_KEY1                       0x1D    // REG_SROM_ENABLE: Key to enable SROM download
#define SROM_KEY2                       0x18    // REG_SROM_ENABLE: Key to enable SROM download
#define SROM_CHECK                      0x15    // REG_SROM_ENABLE: Value to setup SROM check

#define LP_CFG0_REG_DEFAULT             0x7F    // REG_LP_CFG0: Laser current default contents
#define MATCH_BIT                       0x80    // REG_LP_CFG0: Laser bin matching parameter
#define LP_CFG1_REG_DEFAULT             0x80    // REG_LP_CFG1: Laser current default contents
#define Non_adaptive                    0x80
#define Adaptive                        0x00  

#define IR_CC1(x)        (gpio_SetValue(1, 19, x))    
#define IR_CC2(x)        (gpio_SetValue(0, 20, x))         

extern S16 sensor_X;
extern S16 sensor_Y;
extern U16 sensorIntegrity;
extern U8 motion;
extern U8 surfaceQual;
extern U8 Observation;
extern U8 pixelAvg;
extern U8 pixelSum;
extern U32 pixelShutter;
extern U8 pixelMin;
extern U8 pixelMax;
extern U8 shutterValueUpper;
extern U8 shutterValueLower;
extern U16 ShutterValue;
extern U8 SROMchecktimer;
extern U8 WriterMotionBurstflag;

void sensorProgramAPI     (void);
void updateSensorFirmware (void);
void setResolution        (U16 x,U16 y);
void sensorPowerUp        (void);
void sensorShutDown       (void);
void checkSROM(void);
void sensorLEDCurrent(void);

#endif //_AVAGO3668_API_H_


