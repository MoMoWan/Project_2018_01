/*++

Copyright (c) 2013-2014 Razer Ltd.
All rights reserved. This program contains proprietary and confidential information.

Module Name:
  spi_driver.c

Compiler:
  Keil Software µVision v5.11, with NXP Semiconductor LPC11U2x proccessor support.

Abstract:
  This file provides the SPI interface procedures.

--*/


#include <string.h>
#include "Type.h"
#include "gpio.h"
#include "spi0_driver.h"
#include "wdt.h"
#include "variables.h"
#include "avago_api.h"
#include "avago3389_srom04.h"

void SSP0_Init( void )
/*++

Function Description:
  This function initializes the processor's SPI support.
  bit frequency = PCLK/(CPSDVSR*[SCR+1]) where CPSDVSR is the prescale devider, SPI Speed is 2MHz

Arguments:
   NONE

Returns: 
   NONE
   
--*/
{
  U8 i, Dummy = Dummy;

  LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 11);                 //set bit 11 to enable SSP0 in the SYSAHBCLKCTRL register
  LPC_SYSCON->SSP0CLKDIV = 0x02;                          //SPI  PCLK is 48/2  == 24MHz
  LPC_SYSCON->PRESETCTRL |= (1 << 0);                     //SSP0 reset SSP reset de-asserted

  for (i = 0; i < FIFOSIZE; i++) {
    Dummy = LPC_SSP0->DR;                                 // clear the RxFIFO 
  }

  //LPC_SSP0->CR0 = SSP0CR0_DDS_8BIT | SSP0CR0_FRF_SPI | SSP0CR0_CPOL_HIGH | SSP0CR0_CPHA_FIRST_EDGE | SSP0CR0_SCR_6;
  LPC_SSP0->CR0 = SSP0CR0_DDS_8BIT | SSP0CR0_FRF_SPI | SSP0CR0_CPOL_HIGH | SSP0CR0_CPHA_SECOND_EDGE | SSP0CR0_SCR_6;
  //LPC_SSP0->CPSR = 0x02;									              //SSPCPSR clock prescale register, master mode, minimum divisor is 0x02 =2M
  LPC_SSP0->CPSR = 0x04;									              //SSPCPSR clock prescale register, master mode, minimum divisor is 0x02 = 1M (24/(4*[6+1]))
  LPC_SSP0->CR1 = ~SSP0CR1_SSE;							            // SSP Disable,  Master mode 
  LPC_SSP0->IMSC = SSP0IMSC_RORIM | SSP0IMSC_RTIM;      // Set SSPINMS registers to enable error interrupts	
}

void SSP0_Enable (void)
/*++

Function Description:
  configure pins as SPI in the IOCONFIG register block and enable SSP

Arguments:
  NONE

Returns: 
   NONE
	 
--*/
{ 
  LPC_IOCON->PIO0_8 &= ~0x07;                             //selects PIO0_8 as MISO0
  LPC_IOCON->PIO0_8 |= 0x01;		
  LPC_IOCON->PIO0_9 &= ~0x07;                             //selects PIO0_9 as MOSI0
  LPC_IOCON->PIO0_9 |= 0x01;		
  LPC_IOCON->PIO0_6 &= ~0x07;                            //PIO0_6 as SSP0 SCLK0 
  LPC_IOCON->PIO0_6 |= 0x02;
  LPC_IOCON->PIO0_2 &= ~0x07;                             //PIO0_2 as GPIO
  gpio_Direction(0, 2, 1);                                //set the PIO0_2 as output,high level  

  // [ Why need De-assert SPI here?
  SPI_CHIP_SELECT_HIGH;                                   // De-assert chip-select (abort pending transfer)
  __NOP();
  __NOP();
  __NOP();
  SPI_CHIP_SELECT_LOW; 
  // ]
  
  LPC_SSP0->CR1 = SSP0CR1_SSE;                            // SSP0 Enabled as Master mode
}

void SSP0_Disable (void)
/*++

Function Description:
  configure pins of SPI port as GPIO to disable SPI interface

Arguments:
  NONE

Returns: 
   NONE
	 
--*/
{
  LPC_SSP0->CR1 &= ~SSP0CR1_SSE;                            // Disable the SPI controller 
  LPC_IOCON->PIO0_8 = 0x80 ;                               // MISO, PIO0_8 as GPIO, input, no pull up
  gpio_Direction(0, 8, 1); 
  LPC_IOCON->PIO0_9 = 0x80;                               // MOSI, PIO0_9 as GPIO, input, no pull up
  gpio_Direction(0, 9, 0);
  LPC_IOCON->PIO0_6  = 0x80;                             // SCK, PIO1_29 as GPIO, input, no pull up
  gpio_Direction(0, 6, 0);
  LPC_IOCON->PIO0_2  = 0x80;                              // SSL, PIO0_2 as GPIO, input, no pull up
  gpio_Direction(0, 2, 0);
}

U8 spiIsEnable(void)
/*++

Function Description:
  To get the spi interface was Enable of not

Arguments:
  NONE

Returns: 
   0 - spi was disabled
   others - spi was enabled
	 
--*/
{
  return (LPC_SSP0->CR1 & SSP0CR1_SSE);
}

void SSP0_Duplex8(U8 *pData)
/*++

Function Description:
  Full-duplex a single byte from/to pData

Arguments:
  pData: Point to byte

Returns: 
   NONE
	 
--*/
{
  U8 temp;
  
  SPI_SET_MODE8;                                          // Enable 8-bit SPI mode
  // TFE: Transmit FIFO Empty.
  //      1 is the Transmit FIFO is empty
  //      0 if not
  // BSY: Busy.
  //      0 if the SSP0 controller is idle
  //      1 if it is currently sending/receiving a frame and/or the Tx FIFO is not empty
  while ((LPC_SSP0->SR & (SSP0SR_TFE | SSP0SR_BSY)) != SSP0SR_TFE); // Wait until not busy and TX FIFO empty
  LPC_SSP0->DR = *pData;
  // RNE: Receive FIFO Not Empty.
  //      0 if the Receive FIFO is empty
  //      1 if not
   while ((LPC_SSP0->SR & (SSP0SR_BSY | SSP0SR_RNE)) != SSP0SR_RNE); // Wait until not Busy and RX FIFO not empty
  temp = (unsigned char)LPC_SSP0->DR;
  if (pData >= (unsigned char*)LPC_RAM_BASE) {            // Cannot write to LPC13XX FLASH (results hardware error)
   *pData = temp;
  }
}

void spiWriteByte(U8 addr, U8 data)
/*++

Function Description:
  This function writes the specified register. 
  First any transmissions in progress are aborted;
  Next the most significant bit of the sensor register address defines
  the direction of the command (0 = Read, 1 = Write). 
  The register address is transmitted, when the transmit buffer is empty again 
  the data is written. 
 
Arguments:
  addr - SPI Register address to write.
  data - SPI Register data value.

Returns: 
  NONE

--*/
{
  SPI_ASSERT;
  addr = (0x80 | addr);                                    // Assert the addr for spi write
  SSP0_Duplex8(&addr);                                     // Transmit write addr
  //delayMicroseconds(50);
  //delayMicroseconds(20);
  SSP0_Duplex8(&data);                                     // Transmit write data
  delayMicroseconds(3);                                  // 30us between write cmd request
  SPI_DE_ASSERT;
  // [ test
  delayMicroseconds(180);                                  // 20us between read and write cmd request
  // ]
}

U8 spiReadByte(U8 addr)
/*++

Function Description:
  This function reads the specified register. 
  First any transmissions in progress are aborted;
  Next the most significant bit of the sensor register address defines
  the direction of the command (0 = Read, 1 = Write). 
  The second write provides the clock for the read of the data from the slave. 
  Before each write, we wait for the transmit buffer is empty. 
  The data of the second write is a don't care. The wait for receive buffer full, 
  signals the sensor data is ready.
 
Arguments:
  addr - SPI Register address to read.

Returns: 
  data - SPI Register data value.

--*/
{
  U8 ret = 0xFF;

  SPI_ASSERT;
  addr = (0x7F & addr);                                   // Assert the addr for spi read
  SSP0_Duplex8(&addr);                                    // Transmit write addr
  //delayMicroseconds(110);                                  // 4us request
  //delayMicroseconds(50);                                  // 4us request
	 delayMicroseconds(160);                                  // 4us request
  SSP0_Duplex8(&ret);
  //delayMicroseconds(100);                                  // 20us between read and write cmd request
  delayMicroseconds(3);                                  // 20us between read and write cmd request
  SPI_DE_ASSERT;
  // [ test
  delayMicroseconds(180);                                  // 20us between read and write cmd request
  // ]
  return ret;
}

void spiWriteBytes(U8 addr, U8 *pData, U32 length)
/*++

Function Description:
  This function write serial bytes data to  the specified register.   
 
Arguments:
  addr  - SPI Register address to write.
  pdata - Point to the SPI Register data values.
  length - data length to write

Returns: 
  NONE

--*/
{
  U32 i;

  SPI_ASSERT;  
  addr = (0x80 | addr);                                   // Assert the addr for spi write
  SSP0_Duplex8(&addr);                                    // Transmit write addr
  delayMicroseconds(120);
  for (i = 0; i < length; i++) {                          // Transmit write data
    SSP0_Duplex8(pData);
    pData++;
    delayMicroseconds(3);                                // 30us request

    wdt_resetTimer();
  }
  SPI_DE_ASSERT;
  delayMicroseconds(15);                                // 30us request
}

void spiReadBytes(U8 addr, U8 *pData, U32 length) 
/*++

Function Description:
  This function read serial bytes data to  the specified register.   
 
Arguments:
  addr  - SPI Register address to write.
  pdata - Point to the buffer to save the read values of SPI Register.
  length - data length to write

Returns: 
  NONE

--*/
{
  U32 i;

  SPI_ASSERT;  
  addr = (0x7F & addr);                                   // Assert the addr for spi read
  SSP0_Duplex8(&addr);                                    // Transmit write addr
  delayMicroseconds(50);
  for (i = 0; i < length; i++) {
    SSP0_Duplex8(pData);
    pData++;
    delayMicroseconds(2);
  }
  SPI_DE_ASSERT;
}

void spiDisplacement(void)
/*++

Function Description:
  This function reads the X and Y displacement values. The displacement values are
  read in burst mode to speed up the process. Reading the Motion register freezes the
  Delta data registers and should be read sequentially (registers 0x02, 0x03, and 0x04).
  Note that this function is written using the principles outlined in the read function.
 
Arguments:
  NONE

Returns: 
  NONE

--*/
{
  U8 buf[15] = {0x80};
	motion = MOT_BIT;
	if (WriterMotionBurstflag == 0) {
	  spiWriteByte(REG_MOTION_BURST,0xff);   // Need write any value to Motion_Burst register,othwerwise can't read x,y data,request for pwm3389DM Mark 2016.3.23
    WriterMotionBurstflag  = 1;
  }
  spiReadBytes(REG_MOTION_BURST, buf, 12);                // Force Delta_X and Delta_Y to be read
	motion = buf[0];
  Observation = buf[1]; 
  surfaceQual = buf[6];                                   // Reading surface Quality
	pixelSum = buf[7];                                      // Reading Sum pixel value
  pixelMax = buf[8];                                      // Reading Max pixel value
  pixelMin = buf[9];                                      // Reading Min pixel value
	shutterValueUpper = buf[10];                            // Reading Shutter Value Upper
  shutterValueLower = buf[11];                            // Reading Shutter Value Lower

	if((buf[0] & 0x80)) {
      sensor_X = (S16)(buf[2] |(buf[3] << 8)) ;           // Reading Delta X
      sensor_Y = (S16)(buf[4] |(buf[5] << 8)) ;           // Reading Delta Y
  }
}

void spiDownload_SROM(void)
/*++

Function Description:
  This function downloads the Avago Sensor firmware into the SROM. This is done by
  using burst mode write 1986 bytes to the SROM. After the sensor is put into the
  "download srom" mode, the download address is written out the bus followed by the
  data bytes coming at 10us intervals. Note that this function is written using the
  principles outlined in the write function. There are two different sensor firmware
  versions that can be downloaded, currently only the Standard displacement support
  is downloaded.

Arguments:
  NONE

Returns: 
  NONE

--*/
{  
	//[PMW3389 SROM download
	spiWriteByte(REG_CONFIGURATION_II,0x4);                 // Normal CPI setting affects both delta X and Y
	spiWriteByte(REG_SROM_ENABLE, 0x1D);                    // SROM_Enable registe for initializing
  delayMilliseconds(10);                                  // Delay 10ms 
  spiWriteByte(REG_SROM_ENABLE, 0x18);                    // start SROM download
  delayMicroseconds(200);							                    // Measured at 0.2ms, 

  spiWriteBytes(0x80 | REG_SROM_LOAD_BURST, (U8 *)srom3389param, sizeof(srom3389param)); // write SROM file into SROM_Load_Burst register
  delayMicroseconds(200);                                 // Delay 200us before the next command

}

BOOL spiChecksum_SROM(void)
/*++

Function Description:
  This function causes the Avago Sensor firmware to perform a CRC test on the SROM.
  After placing the sensor in the 'CRC test' mode, this function gives 7.5ms to allow
  the sensor to complete this task.
 
Arguments:
  NONE

Returns: 
  result - TRUE  = SROM is valid.
           FALSE = SROM is invalid.

--*/
{
  BOOL result = FALSE;
 
  if (spiReadByte(REG_SROM_ID) != 0) {                   //0x0d
    spiWriteByte(REG_SROM_ENABLE, SROM_CHECK);           // Put Avago Sensor into SROM CRC test mode
    delayMilliseconds(13);                                // Measured at 10 ms, allow CRC to be generated
    
    if (spiReadByte(REG_DATA_OUT_UPPER) == VALID_UPPER_CRC) {  
      if (spiReadByte(REG_DATA_OUT_LOWER) == VALID_LOWER_CRC) {
        result = TRUE;                                    // If CRC is valid, return TRUE
      }
    }
  }
  	// [PMW3389 power up step 7~10
	spiWriteByte(0x3d,0x80);
  SROMchecktimer = 0;
  do {
    wdt_resetTimer();                                     // If Watchdog Timer expires (1.024s), MCU resets 
    delayMilliseconds(1);
    if (SROMchecktimer > 55) {
      result = FALSE;
    }
  } while ((spiReadByte(0x3d) != 0xc0) && (result !=  FALSE));
	
	spiWriteByte(0x3d,0x0);
	spiWriteByte(REG_CONFIGURATION_II,0x4);
	// ]
  
  return (result);
}
