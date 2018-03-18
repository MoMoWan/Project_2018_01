/*++

Copyright (c) 2012-2014 Razer Ltd.
All rights reserved. This program contains proprietary and confidential information.

Module Name:
  main.c

Compiler:
  Keil Software uVision v4.50, with NXP Semiconductor LPC11U23 proccessor support.

Abstract:
  This file contains the definitions for i2c_driver.c, which provides the I2C
  communication support.

--*/

#ifndef _I2C_DRIVER_H_
#define _I2C_DRIVER_H_


#define FAST_MODE_PLUS      0

#define BUFSIZE             200
#define MAX_TIMEOUT         0x00008000//0x00004000//0x00FFFFFF

#define I2CMASTER           0x01
#define I2CSLAVE            0x02
#define MASTER              0x01
#define SLAVE               0x02

#define PCF8594_ADDR        0xA0
#define READ_WRITE          0x01

#define RD_BIT              0x01

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

#define I2CONSET_I2EN       (0x1<<6)                      // I2C Control Set Register 
#define I2CONSET_AA         (0x1<<2)
#define I2CONSET_SI         (0x1<<3)
#define I2CONSET_STO        (0x1<<4)
#define I2CONSET_STA        (0x1<<5)

#define I2CONCLR_AAC        (0x1<<2)                      // I2C Control clear Register
#define I2CONCLR_SIC        (0x1<<3)
#define I2CONCLR_STAC       (0x1<<5)
#define I2CONCLR_I2ENC      (0x1<<6)

#define I2DAT_I2C           0x00000000                    // I2C Data Reg 
#define I2ADR_I2C           0x00000000                    // I2C Slave Address Reg 

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

//#define I2SCLH_SCLH         0x00000180                    // I2C SCL Duty Cycle High Reg 
//#define I2SCLL_SCLL         0x00000180                    // I2C SCL Duty Cycle Low Reg 
#define I2SCLH_HS_SCLH		  0x00000015                    // Fast Plus I2C SCL Duty Cycle High Reg 
#define I2SCLL_HS_SCLL		  0x00000015                    // Fast Plus I2C SCL Duty Cycle Low Reg


extern U16 i2cRecoverTimer;

extern void I2C_IRQHandler (void);
extern U32  i2c_initialization  (U32 mode);
extern U32  I2CStart (void);
extern U32  I2CStop (void);
extern U32  I2CEngine (void);
extern void i2cRead(U8 device, U8 reg, U8 *data, U8 length);
extern void i2cWrite(U8 device, U8 reg, U8 *data, U8 length);


#endif
