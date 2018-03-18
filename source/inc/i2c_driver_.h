/*++

Copyright (c) 2012-2014 Razer Ltd.
All rights reserved. This program contains proprietary and confidential information.

Module Name:
  main.c

Compiler:
  Keil Software µVision v4.50, with NXP Semiconductor LPC11U14 proccessor support.

Abstract:
  This file contains the definitions for i2c_driver.c, which provides the I2C
  communication support.

--*/

#ifndef _I2C_DRIVER_H_
#define _I2C_DRIVER_H_

#define FAST_MODE_PLUS        0                           // 1: 1MHz clock rate, I2SCLH_HS_SCLH & I2SCLL_HS_SCL hand
                                                          // adjusted to give 1.023MHz at 12.0MHz oscillator
#define I2C_SIZE              (64<<1)                     // Used as a Circular Buffer, so must be power of 2
#define MAX_TIMEOUT           0x00004000                  // Timeout = 1ms

#define MASTER                0x01
#define SLAVE                 0x02

#define PCF8594_ADDR          0xA0
#define READ_WRITE            0x01

#define RD_BIT                0x01

#define I2C_IDLE              0
#define I2C_STARTED           1
#define I2C_RESTARTED         2
#define I2C_REPEATED_START    3
#define DATA_ACK              4
#define DATA_NACK             5
#define I2C_BUSY              6
#define I2C_NO_DATA           7
#define I2C_NACK_ON_ADDRESS   8
#define I2C_NACK_ON_DATA      9
#define I2C_ARBITRATION_LOST  10
#define I2C_TIME_OUT          11
#define I2C_OK                12

#define I2C_ONSET_I2EN       (1 << 6)                     // I2C Control Set Register
#define I2C_ONSET_AA         (1 << 2)
#define I2C_ONSET_SI         (1 << 3)
#define I2C_ONSET_STO        (1 << 4)
#define I2C_ONSET_STA        (1 << 5)

#define I2C_ONCLR_AAC        (1 << 2)                     // I2C Control clear Register
#define I2C_ONCLR_SIC        (1 << 3)
#define I2C_ONCLR_STAC       (1 << 5)
#define I2C_ONCLR_I2ENC      (1 << 6)

#define I2DAT_I2C             0x00000000                  // I2C Data Reg
#define I2ADR_I2C             0x00000000                  // I2C Slave Address Reg

// 800K
//#define I2SCLH_SCLH           30        // I2C SCL Duty Cycle High Reg
//#define I2SCLL_SCLL           30        // I2C SCL Duty Cycle Low Reg
// 400K
#define I2SCLH_SCLH           55                          // I2C SCL Duty Cycle High Reg
#define I2SCLL_SCLL           65                          // I2C SCL Duty Cycle Low Reg
// 300K
//#define I2SCLH_SCLH           75        // I2C SCL Duty Cycle High Reg
//#define I2SCLL_SCLL           85        // I2C SCL Duty Cycle Low Reg
// 200K
//#define I2SCLH_SCLH           120        // I2C SCL Duty Cycle High Reg
//#define I2SCLL_SCLL           120        // I2C SCL Duty Cycle Low Reg
// 100KHz(240),400k (62)
//#define I2SCLH_SCLH           240        // I2C SCL Duty Cycle High Reg
//#define I2SCLL_SCLL           240        // I2C SCL Duty Cycle Low Reg
// 50KHz(240),400k (62)
//#define I2SCLH_SCLH           480        // I2C SCL Duty Cycle High Reg
//#define I2SCLL_SCLL           480        // I2C SCL Duty Cycle Low Reg
#define I2SCLH_HS_SCLH        0x00000018                  // Fast Plus I2C SCL Duty Cycle High Reg
#define I2SCLL_HS_SCLL        0x00000018                  // Fast Plus I2C SCL Duty Cycle Low Reg

extern volatile U8 rc_Length;
extern volatile U8 tx_Length;
extern volatile U8 rc_Buffer[];
extern volatile U8 tx_Buffer[];
extern volatile U8 tx_Head;
extern volatile U8 tx_Tail;

U8   i2c_initialization  (U8 mode);
void I2C_IRQHandler      (void);
void i2c_StartCondition  (void);
U8   i2c_Wait4Completion (void);
void i2c_SetTransmitData (U8 data);
U8   i2c_GetTransmitData (void);
void i2cRead(U8 device, U8 reg, U8 *data, U8 length);
void i2cWrite(U8 device, U8 reg, U8 *data, U8 length);

void i2cRecoverTrigger(void);
extern U16 i2cRecoverTimer;

#endif // _I2C_DRIVER_H_

