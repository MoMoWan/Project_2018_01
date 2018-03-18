/*++

Copyright (c) 2012-2014 Razer Ltd.
All rights reserved. This program contains proprietary and confidential information.

Module Name:
  main.c

Compiler:
  Keil Software µVision v4.50, with NXP Semiconductor LPC11U14 proccessor support.

Abstract:
  This file provides the I²C communication support code.

--*/

#include "main.h"
#include "i2c_driver.h"
#include "wdt.h"
#include "mouse.h"
#include "led_driver.h"
#include "gpio.h"

volatile U8  rc_Index = 0;
volatile U8  rc_Length;
volatile U8  rc_Buffer[I2C_SIZE];                         // I²C Receive uses a Linear Buffer

volatile U8  tx_Head = 0;
volatile U8  tx_Tail = 0;
volatile U8  tx_Index = 0;
volatile U8  tx_Length;
volatile U8  tx_Buffer[I2C_SIZE];                         // I²C Transmit uses a Circular Buffer

volatile U8  i2c_Status = I2C_IDLE;
U32 timeout = 0;
U16 i2cRecoverTimer = 0;

U8 i2c_initialization(U8 mode)
/*++

Function Description:
  This function initializes the I²C communication by configuring the GPIO pins
  to be the SCL and SDA function. The I²C Interrupt handler is also enabled.

Arguments:
  mode - Either MASTER or SLAVE.

Returns:
  result - SUCCESS or FAILURE.

--*/
{
  tx_Head = 0;
  tx_Tail = 0;
  LPC_SYSCON->PRESETCTRL |= (1 << 1);
  LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 5);
  LPC_IOCON->PIO0_4 &= ~(0x0007 | 0x0300);                // Clear bit0,1,2,8,9
  LPC_IOCON->PIO0_4 |= 0x01;                              // SCL (PIO0_4, SCL, Standard mode)
  LPC_IOCON->PIO0_5 &= ~(0x0007 | 0x0300);                // Clear bit0,1,2,8,9
  LPC_IOCON->PIO0_5 |= 0x01;                              // SDA (PIO0_5, SDA, Standard mode)
  LPC_I2C->CONCLR = (I2C_ONCLR_AAC | I2C_ONCLR_SIC | I2C_ONCLR_STAC | I2C_ONCLR_I2ENC);
#if FAST_MODE_PLUS
  LPC_IOCON->PIO0_4 |= (1 << 9);                          // Reset registers (Fast Mode)
  LPC_IOCON->PIO0_5 |= (1 << 9);
  LPC_I2C->SCLL = I2SCLL_HS_SCLL;
  LPC_I2C->SCLH = I2SCLH_HS_SCLH;
#else
  LPC_I2C->SCLL = I2SCLL_SCLL;                            // Reset registers (Standard)
  LPC_I2C->SCLH = I2SCLH_SCLH;
#endif
  if (mode == SLAVE) {
    LPC_I2C->ADR0 = PCF8594_ADDR;
  }    

  NVIC_EnableIRQ(I2C_IRQn);                               // Enable the I²C Interrupt
  LPC_I2C->CONSET = I2C_ONSET_I2EN;
  return (SUCCESS);
}


void I2C_IRQHandler(void)
/*++

Function Description:
  This function initializes all of the processor port pins. From device to device,
  the I²C communication protocol may vary, in the example below, the protocol uses
  repeated start to read data from or write to the device: For master read: the 
  sequence is: STA,Addr(W),offset,RE-STA,Addr(r),data...STO. For Master write: the 
  sequence is: STA,Addr(W),offset,RE-STA,Addr(w),data...STO. Thus, in state 8, the
  address is always WRITE, in state 10, the address could be READ or WRITE depending
  on the I²C command.

Arguments:
  NONE

Returns:
  NONE

--*/
{
  U8 value;

  value = LPC_I2C->STAT;                                  // Deals with Master reads & writes only

  switch (value) {
  case 0x08:                                              // Start condition is issued
    tx_Index = 0;
    LPC_I2C->DAT = i2c_GetTransmitData();                 // Get data to transmit
    LPC_I2C->CONCLR = (I2C_ONCLR_SIC | I2C_ONCLR_STAC);
    break;

  case 0x10:                                              // Repeated-start is issued
    rc_Index = 0;
    LPC_I2C->DAT = i2c_GetTransmitData();                 // Get data and send SLA with R bit set
    LPC_I2C->CONCLR = (I2C_ONCLR_SIC | I2C_ONCLR_STAC);
    break;

  case 0x18:                                              // Regardless, it's a ACK
    if (tx_Length == 1) {
      LPC_I2C->CONSET = I2C_ONSET_STO;                    // Set Stop flag
      i2c_Status = I2C_NO_DATA;
    } else {
      LPC_I2C->DAT = i2c_GetTransmitData();               // Get data to transmit
    }
    LPC_I2C->CONCLR = I2C_ONCLR_SIC;
    break;

  case 0x28:                                              // Data = transmitted, regardless ACK or NACK
    if (tx_Index < tx_Length) {
      LPC_I2C->DAT = i2c_GetTransmitData();               // Get transmit data, this should be the last byte
    } else {
      if (rc_Length != 0) {
        LPC_I2C->CONSET = I2C_ONSET_STA;                  // Set Repeated-start flag
      } else {
        LPC_I2C->CONSET = I2C_ONSET_STO;                  // Set Stop flag
        i2c_Status = I2C_OK;
        if (tx_Head != tx_Tail) {                         // If there data to transmit, then
          i2c_StartCondition();                           // Force another start condition
        }
      }
    }
    LPC_I2C->CONCLR = I2C_ONCLR_SIC;
    break;

  case 0x30:
    LPC_I2C->CONSET = I2C_ONSET_STO;                      // Set Stop flag
    i2c_Status = I2C_NACK_ON_DATA;
    LPC_I2C->CONCLR = I2C_ONCLR_SIC;
    break;

  case 0x40:                                              // Master Receive, SLA_R has been sent
    if ((rc_Index + 1) < rc_Length) {
      LPC_I2C->CONSET = I2C_ONSET_AA;                     // Assert ACK after data is received (goto case 0x50)
    } else {
      LPC_I2C->CONCLR = I2C_ONCLR_AAC;                    // Assert NACK after data is received (goto case 0x58)
    }
    LPC_I2C->CONCLR = I2C_ONCLR_SIC;
    break;

  case 0x50:                                              // Data = received, regardless following ACK or NACK
    rc_Buffer[rc_Index++] = LPC_I2C->DAT;
    if ((rc_Index + 1) < rc_Length) {
      LPC_I2C->CONSET = I2C_ONSET_AA;                     // Assert ACK after data is received
    } else {
      LPC_I2C->CONCLR = I2C_ONCLR_AAC;                    // Assert NACK on last byte
    }
    LPC_I2C->CONCLR = I2C_ONCLR_SIC;
    break;

  case 0x58:
    rc_Buffer[rc_Index++] = LPC_I2C->DAT;
    i2c_Status = I2C_OK;
    LPC_I2C->CONSET = I2C_ONSET_STO;                      // Set Stop flag
    LPC_I2C->CONCLR = I2C_ONCLR_SIC;                      // Clear SI flag
    if (tx_Head != tx_Tail) {                             // If there data to transmit, then
      i2c_StartCondition();                               // Force another start condition
    }
    break;

  case 0x20:                                              // Regardless, it's a NACK
  case 0x48:
    LPC_I2C->CONSET = I2C_ONSET_STO;                      // Set Stop flag
    i2c_Status = I2C_NACK_ON_ADDRESS;
    LPC_I2C->CONCLR = I2C_ONCLR_SIC;
    break;

  case 0xF8:																							// Indicates that no relevant info is available for SI not set.
    LPC_I2C->CONCLR = I2C_ONCLR_SIC;
    break;

  case 0x00:																							// Bus error. when it happen, SI is set. to recover it, set STO flag and clear SI. otherwise, I2C will enter no address mode.
    LPC_I2C->CONSET = I2C_ONSET_STO|I2C_ONSET_AA;         // Set Stop flag and ack bit
    LPC_I2C->CONCLR = I2C_ONCLR_SIC;
	  break;

  case 0x38:                                              // Arbitration lost (not handled)
  default:
    i2c_Status = I2C_ARBITRATION_LOST;
    LPC_I2C->CONCLR = I2C_ONCLR_SIC;
    break;
  }
  return;
}


void i2c_StartCondition(void)
/*++

Function Description:
  This function first checks if there is an active transmission in progress. If so,
  this task will be pending until the current transmission completes. The first two
  entries are always the number of bytes to Transmit and the number of bytes to
  Receive for this specific command.

Arguments:
  NONE

Returns:
  NONE

--*/
{
  if ((i2c_Status == I2C_IDLE) || (i2c_Status == I2C_OK) || (i2c_Status == I2C_TIME_OUT)) {
    tx_Length = i2c_GetTransmitData();                    // Get number of bytes to transmit
    rc_Length = i2c_GetTransmitData();                    // Get number of bytes to receive
    rc_Index = tx_Index = 0;                              // Initialize Buffer indexes
    LPC_I2C->CONSET = I2C_ONSET_STA;                      // Issue a start condition
    i2c_Status = I2C_BUSY;
  }
}


U8 i2c_Wait4Completion(void)
/*++

Function Description:
  This function waits for all pending commands to be complete and then returns with
  the current status.

Arguments:
  NONE

Returns:
  result - I²C Status.

--*/
{
  timeout = 0;

  while (i2c_Status != I2C_OK) {
    if (timeout >= MAX_TIMEOUT) {
      i2c_Status = I2C_TIME_OUT;
      break;
    }
    timeout++;
  }
  LPC_I2C->CONCLR = I2C_ONCLR_STAC;                       // Clear I²C Interrupt flag
  return (i2c_Status);
}


void i2c_SetTransmitData(U8 data)
/*++

Function Description:
  This function first checks if there is room for the new data. Since interrupts
  are active if there is not room currently there will be soon. Then the function
  fills the circular buffer and updates the Buffer's Tail pointer. When adding a
  new command to the buffer the first entry is always the number of bytes to
  Write followed by the number of bytes to read. Next follows the actual data that
  will be transmitted.

Arguments:
  data - Data to transmit.

Returns:
  NONE

--*/
{
  timeout = 0;
  while (((tx_Tail + 1) & (I2C_SIZE - 1)) == tx_Head) {   // Wait until there is room for new data
    wdt_resetTimer(); 																		// If Watchdog Timer expires (1.024s), MCU resets
    if (++timeout > MAX_TIMEOUT) {
      return;//break;
    }
  } 
  tx_Buffer[tx_Tail] = data;                              // Insert data at Tail location
  tx_Tail = (tx_Tail + 1) & (I2C_SIZE - 1);               // Update Tail, adjust if it wrapped
}


U8 i2c_GetTransmitData(void)
/*++

Function Description:
  This function get data from the circular buffer and updates the Buffer's Head
  pointer as well as the number of bytes read from the transmit buffer.

Arguments:
  NONE

Returns:
  data - Data to transmit.

--*/
{
  U8 data;

  tx_Index++;                                             // Update # of bytes read from buffer
  data = tx_Buffer[tx_Head];                              // Get data at the head of the buffer 
  tx_Head = (tx_Head + 1) & (I2C_SIZE - 1);               // Update Head, adjust if it wrapped
  return (data);
}

void i2cWrite(U8 device, U8 reg, U8 *data, U8 length)
/*++

Function Description:
  This function writes the specified Driver's register and does not wait for the transmission
  to complete. This allows the setting of states to occur in the background without taking up
  time in the foreground (or interrupt handler).
 
Arguments:
  device - device address to write
  reg    - Register to Write.
  *data  - ptr to the write datas
	length - data length to write

Returns: 
  NONE

--*/
{
  U8 i;
  
  i2c_SetTransmitData(2 + length);                        // Number of bytes to Write
  i2c_SetTransmitData(0);                                 // Number of bytes to Read
  i2c_SetTransmitData(device);                            // Address all PCA9633's together
  i2c_SetTransmitData(reg);                               // PCA9633 Register index
  for (i = 0; i < length; i++) {
    i2c_SetTransmitData(data[i]);                         // Value to write register
  }
//  while (length--) {
//    i2c_SetTransmitData(*data++);                         // Value to write register
//  }
  i2c_StartCondition();                                   // Force start condition if I²C Idle
}


void i2cRead(U8 device, U8 reg, U8 *data, U8 length)
/*++

Function Description:
  This function reads the specified Driver's register and waits until the
  communication is complete. Reading ).
 
Arguments:
  device - device address to write
  reg    - Register to read.
  *data  - ptr to the buffer for read datas
  length - data length to read

Returns: 
  NONE.

--*/
{
  U8 i;
  
  for (i = 0; i < length; i++) {                        // Clear i2c receive buffer
    rc_Buffer[i] = 0;
  }
  i2c_SetTransmitData(2);                               // Number of bytes to Write
  i2c_SetTransmitData(length);                          // Number of bytes to Read
  i2c_SetTransmitData(device);                          // device address for write
  i2c_SetTransmitData(reg);                             // PCA9633 Register index
  i2c_SetTransmitData(0x01 | device);                   // device address for read
  i2c_StartCondition();                                 // Force start condition if I²C Idle
  i2c_Wait4Completion();                                // Wait for communication to complete
  for (i = 0; i < length; i++) {
    *data++ = rc_Buffer[i];
  }
}


void i2cRecoverTrigger(void)
/*++

Function Description:
  This function are for eft protect. it will detect the extern I2C device by read one regist about every short time.
  If error find, start a debounce, and it will reset I2C, I2C device and try to recover it when debounce time out.

Arguments:
  NONE

Returns:
  NONE

--*/
{
  if (i2cRecoverTimer > 2000) {
    i2cRecoverTimer = 0;
  }
}
    
