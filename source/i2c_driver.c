/*++

Copyright (c) 2012-2014 Razer Ltd.
All rights reserved. This program contains proprietary and confidential information.

Module Name:
  i2c_driver.c

Compiler:
  Keil Software uVision v4.72, with NXP Semiconductor LPC11U24 proccessor support.

Abstract:
  This file provides the I2C communication support code. include I2C initialization, 
  I2C interrupt handler, and APIs for I2C access.

--*/

#include "main.h"
#include "i2c_driver.h"
#include "wdt.h"
#include "led_driver.h"
#include "gpio.h"
#include "timer.h"


volatile U32 I2CMasterState = I2C_IDLE;
volatile U32 I2CSlaveState = I2C_IDLE;
volatile U32 timeout = 0;

volatile U32 I2CMode;

volatile U8  I2CMasterBuffer[BUFSIZE];
volatile U8  I2CSlaveBuffer[BUFSIZE];
volatile U32 I2CCount = 0;
volatile U32 I2CReadLength;
volatile U32 I2CWriteLength;

volatile U32 RdIndex = 0;
volatile U32 WrIndex = 0;


void I2C_IRQHandler (void) 
/*++

Function Description:
  I2C interrupt handler, deal with master mode only.
  From device to device, the I2C communication protocol may vary, 
  in the example below, the protocol uses repeated start to read data from or 
  write to the device:
  For master read: the sequence is: STA,Addr(W),offset,RE-STA,Addr(r),data...STO 
  for master write: the sequence is: STA,Addr(W),offset,RE-STA,Addr(w),data...STO
  Thus, in state 8, the address is always WRITE. in state 10, the address could 
  be READ or WRITE depending on the I2C command.

Arguments:
  NONE

Returns:
  NONE

--*/
{
  U8 StatValue;

  timeout = 0;
  StatValue = LPC_I2C->STAT;                              //this handler deals with master read and master write only

  switch (StatValue) {
  case 0x08:			                                        // A Start condition is issued.
    WrIndex = 0;
    RdIndex = 0;
    LPC_I2C->DAT = I2CMasterBuffer[WrIndex++];
    LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
    break;
	
  case 0x10:			                                        // A repeated started is issued 
    RdIndex = 0;
    LPC_I2C->DAT = I2CMasterBuffer[WrIndex++];            // Send SLA with R bit set
    LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
    break;
	
  case 0x18:			                                        // Regardless, it's a ACK
    if (I2CWriteLength == 1) {
      LPC_I2C->CONSET = I2CONSET_STO;                     // Set Stop flag
      I2CMasterState = I2C_NO_DATA;
    } else {
      LPC_I2C->DAT = I2CMasterBuffer[WrIndex++];
    }
    LPC_I2C->CONCLR = I2CONCLR_SIC;
    break;
	
  case 0x28:	                                            // Data byte has been transmitted, regardless ACK or NACK
    if (WrIndex < I2CWriteLength) {   
      LPC_I2C->DAT = I2CMasterBuffer[WrIndex++];          // this should be the last one
    } else {
      if (I2CReadLength != 0) {
        LPC_I2C->CONSET = I2CONSET_STA;	                  // Set Repeated-start flag
      } else {
        LPC_I2C->CONSET = I2CONSET_STO;                   // Set Stop flag
        I2CMasterState = I2C_OK;
      }
    }
    LPC_I2C->CONCLR = I2CONCLR_SIC;
    break;

  case 0x30:
    LPC_I2C->CONSET = I2CONSET_STO;                       // Set Stop flag 
    I2CMasterState = I2C_NACK_ON_DATA;
    LPC_I2C->CONCLR = I2CONCLR_SIC;
    break;
	
  case 0x40:	                                            // Master Receive, SLA_R has been sent
    if ((RdIndex + 1) < I2CReadLength) {                  // Will go to State 0x50
      LPC_I2C->CONSET = I2CONSET_AA;	                    // assert ACK after data is received
    } else {                                              // Will go to State 0x58
      LPC_I2C->CONCLR = I2CONCLR_AAC;	                    // assert NACK after data is received
    }
    LPC_I2C->CONCLR = I2CONCLR_SIC;
    break;
	
  case 0x50:	                                            // Data byte has been received, regardless following ACK or NACK
    I2CSlaveBuffer[RdIndex++] = LPC_I2C->DAT;
    if ( (RdIndex + 1) < I2CReadLength ) {   
      LPC_I2C->CONSET = I2CONSET_AA;	                    // assert ACK after data is received 
    } else {
      LPC_I2C->CONCLR = I2CONCLR_AAC;	                    // assert NACK on last byte
    }
    LPC_I2C->CONCLR = I2CONCLR_SIC;
    break;
	
  case 0x58:
    I2CSlaveBuffer[RdIndex++] = LPC_I2C->DAT;
    I2CMasterState = I2C_OK;
    LPC_I2C->CONSET = I2CONSET_STO;	                      // Set Stop flag  
    LPC_I2C->CONCLR = I2CONCLR_SIC;	                      // Clear SI flag 
    break;

  case 0x20:		                                          // regardless, it's a NACK
  case 0x48:
    LPC_I2C->CONSET = I2CONSET_STO;                       // Set Stop flag 
    I2CMasterState = I2C_NACK_ON_ADDRESS;
    LPC_I2C->CONCLR = I2CONCLR_SIC;
    break;
	
  case 0x38:		                                          // Arbitration lost
                                                          //we don't deal with multiple master situation
  default:
    I2CMasterState = I2C_ARBITRATION_LOST;
    LPC_I2C->CONCLR = I2CONCLR_SIC;	
    break;
  }
  return;
}


U32 I2CStart (void)
/*++

Function Description:
  Create I2C start condition, a timeout value is set if the I2C never 
  gets started, and timed out. It's a fatal error. 

Arguments:
  NONE

Returns:
  TURE or FALSE

--*/
{
  U32 timeout = 0;
  U32 retVal = FALSE;
 
  LPC_I2C->CONSET = I2CONSET_STA;	                        // Set Start flag to Issue a start condition
  
  while (1) {                                             // Wait until START transmitted
    if (I2CMasterState == I2C_STARTED) {
      retVal = TRUE;
      break;	
    }
    if (timeout >= MAX_TIMEOUT) {
      retVal = FALSE;
      break;
    }
    timeout++;
  }
  return (retVal);
}


U32 I2CStop (void)
/*++

Function Description:
  Set the I2C stop condition, if the routine never exit, it's a fatal bus error.

Arguments:
  NONE

Returns:
  true or never return

--*/
{
  LPC_I2C->CONSET = I2CONSET_STO;                         // Set Stop flag 
  LPC_I2C->CONCLR = I2CONCLR_SIC;                         // Clear SI flag  
            
  while( LPC_I2C->CONSET & I2CONSET_STO );                // Wait for STOP detected
  return TRUE;
}


U32 i2c_initialization (U32 I2cMode) 
/*++

Function Description:
  This function initializes the I2C communication by configuring the GPIO pins
  to be the SCL and SDA function. The I2C Interrupt handler is also enabled.

Arguments:
  mode - Either MASTER or SLAVE.

Returns:
  true, never false for LPC11Uxx

--*/
{
  LPC_SYSCON->PRESETCTRL |= (0x1<<1);

  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<5);
  LPC_IOCON->PIO0_4 &= ~0x3F;	                            //  I2C I/O config 
  LPC_IOCON->PIO0_4 |= 0x01;		                          // I2C SCL 
  LPC_IOCON->PIO0_5 &= ~0x3F;	
  LPC_IOCON->PIO0_5 |= 0x01;		                          // I2C SDA 
  
  LPC_I2C->CONCLR = I2CONCLR_AAC | I2CONCLR_SIC | I2CONCLR_STAC | I2CONCLR_I2ENC; // Clear flags   
  
#if FAST_MODE_PLUS
  LPC_IOCON->PIO0_4 |= (0x1<<9);                          // Reset registers
  LPC_IOCON->PIO0_5 |= (0x1<<9);
  LPC_I2C->SCLL   = I2SCLL_HS_SCLL;
  LPC_I2C->SCLH   = I2SCLH_HS_SCLH;
#else
  LPC_I2C->SCLL   = I2SCLL_SCLL;
  LPC_I2C->SCLH   = I2SCLH_SCLH;
#endif

  if (I2cMode == I2CSLAVE) {
    LPC_I2C->ADR0 = PCF8594_ADDR;
  }    

  NVIC_EnableIRQ(I2C_IRQn);

  LPC_I2C->CONSET = I2CONSET_I2EN;
  return( TRUE );
}

/*****************************************************************************
** Function name:		I2CEngine
**
** Descriptions:		The routine to complete a I2C transaction
**				from start to stop. All the intermitten
**				steps are handled in the interrupt handler.
**				Before this routine is called, the read
**				length, write length, I2C master buffer,
**				and I2C command fields need to be filled.
**				see i2cmst.c for more details. 
**
** parameters:			None
** Returned value:		true or false, return false only if the
**				start condition can never be generated and
**				timed out. 
** 
*****************************************************************************/
uint32_t I2CEngine( void ) 
/*++

Function Description:
  The routine to complete a I2C transaction from start to stop. 
  All the intermitten steps are handled in the interrupt handler.
  Before this routine is called, the read length, write length, 
  I2C master buffer, and I2C command fields need to be filled.

Arguments:
  NONE

Returns:
  true or false,  return false only if the start condition 
  can never be generated and timed out. 

--*/
{
  RdIndex = 0;
  WrIndex = 0;

  LPC_I2C->CONSET = I2CONSET_STA;	                        // Set Start flag to Issue a start condition

  I2CMasterState = I2C_BUSY;	

  while (I2CMasterState == I2C_BUSY) {    
    wdt_resetTimer();                                     // If Watchdog Timer expires the MCU resets
    if (timeout >= MAX_TIMEOUT) {      
      I2CMasterState = I2C_TIME_OUT;  
      break;
    }
    timeout++;
  }
  LPC_I2C->CONCLR = I2CONCLR_STAC;

  return (I2CMasterState);
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
  
  for ( i = 0; i < (length + 2); i++ )	/* clear buffer */
  {
  	I2CMasterBuffer[i] = 0;
  }
  I2CWriteLength = 2 + length;
  I2CReadLength = 0;
  I2CMasterBuffer[0] = device;
  I2CMasterBuffer[1] = reg;
  for (i = 0; i < length; i++) {
    I2CMasterBuffer[i+2] = data[i];
  }
  IICStates = I2CEngine();                   //ivan add a IIC state

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
  
  for ( i = 0; i < (length + 3); i++ )	/* clear buffer */
  {
  	I2CSlaveBuffer[i] = 0;
  }
  I2CWriteLength = 2;
  I2CReadLength = length;
  I2CMasterBuffer[0] = device;
  I2CMasterBuffer[1] = reg;
  I2CMasterBuffer[2] = device | RD_BIT;
  I2CEngine( ); 
  if (I2CMasterState == I2C_OK) {
    for (i = 0; i < length; i++) {
      *data++ = I2CSlaveBuffer[i];
    }
  }
}

U16 i2cRecoverTimer;


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
  //U8 tmp;
  
  if (i2cRecoverTimer >= 2000) {
    i2cRecoverTimer = 0;
  }
}



