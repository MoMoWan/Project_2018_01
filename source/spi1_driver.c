#include "spi1_driver.h"
#include "Type.h"
#include "lpc11U6x.h"

#include "gpio.h"
#include "wdt.h"


void SSP1_Init( void )
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

  LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 18);                 //set bit 18 to enable SSP1 in the SYSAHBCLKCTRL register
  LPC_SYSCON->SSP1CLKDIV = 0x01;                          //SPI  PCLK is 48/2  == 24MHz
  LPC_SYSCON->PRESETCTRL |= (1 << 2);                     //SSP1 reset SSP reset de-asserted

  for (i = 0; i < FIFOSIZE; i++) {
    Dummy = LPC_SSP1->DR;                                 // clear the RxFIFO 
  }

  //LPC_SSP0->CR0 = SSP0CR0_DDS_8BIT | SSP0CR0_FRF_SPI | SSP0CR0_CPOL_HIGH | SSP0CR0_CPHA_FIRST_EDGE | SSP0CR0_SCR_6;
  LPC_SSP1->CR0 = SSP1CR0_DDS_8BIT | SSP1CR0_FRF_SPI | SSP1CR0_CPOL_HIGH | SSP1CR0_CPHA_SECOND_EDGE | SSP1CR0_SCR_6;
  //LPC_SSP0->CPSR = 0x02;									              //SSPCPSR clock prescale register, master mode, minimum divisor is 0x02 =2M
  LPC_SSP1->CPSR = 0x04;									              //SSPCPSR clock prescale register, master mode, minimum divisor is 0x02 = 1M (24/(4*[6+1]))
  LPC_SSP1->CR1 = ~SSP1CR1_SSE;							            // SSP Disable,  Master mode 
  LPC_SSP1->IMSC = SSP1IMSC_RORIM | SSP1IMSC_RTIM;      // Set SSPINMS registers to enable error interrupts	
}
void SSP1_Enable (void)
/*++

Function Description:
  configure pins as SPI in the IOCONFIG register block and enable SSP

Arguments:
  NONE

Returns: 
   NONE
	 
--*/
{ 
  LPC_IOCON->PIO1_21 &= ~0x07;                             //selects PIO1_21 as MISO1
  LPC_IOCON->PIO1_21 |= 0x02;		
  LPC_IOCON->PIO0_21 &= ~0x07;                             //selects PIO1_22 as MOSI1
  LPC_IOCON->PIO0_21 |= 0x02;		
  LPC_IOCON->PIO1_20 &= ~0x07;                            //PIO1_20 as SSP1 SCLK1 
  LPC_IOCON->PIO1_20 |= 0x02;
  LPC_IOCON->PIO1_23 &= ~0x07;                             //PIO1_23 as SSL1 
  gpio_Direction(1, 23, 1);                                //set the PIO0_2 as output,high level 	
	gpio_SetValue(1,23,1);
	SPI1_CHIP_SELECT_HIGH;                                   // De-assert chip-select (abort pending transfer)
  __NOP();
  __NOP();
  __NOP();
  SPI1_CHIP_SELECT_LOW; 
  LPC_SSP1->CR1 = SSP1CR1_SSE;                            // SSP0 Enabled as Master mode
}
void SSP1_Disable (void)
/*++

Function Description:
  configure pins of SPI port as GPIO to disable SPI interface

Arguments:
  NONE

Returns: 
   NONE
	 
--*/
{
  LPC_SSP1->CR1 &= ~SSP1CR1_SSE;                            // Disable the SPI controller 
  LPC_IOCON->PIO1_21 = 0x80 ;                               // MISO, PIO1_21 as GPIO, input, no pull up
  gpio_Direction(1, 21, 0); 
  LPC_IOCON->PIO0_21 = 0x80;                               // MOSI, PIO1_22 as GPIO, input, no pull up
  gpio_Direction(0, 21, 0);
  LPC_IOCON->PIO1_20  = 0x80;                             // SCK, PIO1_29 as GPIO, input, no pull up
  gpio_Direction(1, 20, 0);
  LPC_IOCON->PIO1_23  = 0x80;                              // SSL, PIO0_2 as GPIO, input, no pull up
  gpio_Direction(1, 23, 0);
}

U8 spi1IsEnable(void)
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
  return (LPC_SSP1->CR1 & SSP1CR1_SSE);
}

void SSP1_Duplex8(U8 *pData)
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
  
  SPI1_SET_MODE8;                                          // Enable 8-bit SPI mode
  // TFE: Transmit FIFO Empty.
  //      1 is the Transmit FIFO is empty
  //      0 if not
  // BSY: Busy.
  //      0 if the SSP0 controller is idle
  //      1 if it is currently sending/receiving a frame and/or the Tx FIFO is not empty
  while ((LPC_SSP1->SR & (SSP1SR_TFE | SSP1SR_BSY)) != SSP1SR_TFE); // Wait until not busy and TX FIFO empty
  LPC_SSP1->DR = *pData;
  // RNE: Receive FIFO Not Empty.
  //      0 if the Receive FIFO is empty
  //      1 if not
  while ((LPC_SSP1->SR & (SSP1SR_BSY | SSP1SR_RNE)) != SSP1SR_RNE); // Wait until not Busy and RX FIFO not empty
  temp = (unsigned char)LPC_SSP1->DR;
  if (pData >= (unsigned char*)LPC_RAM_BASE) {            // Cannot write to LPC13XX FLASH (results hardware error)
   *pData = temp;
  }
}
void spi1WriteCommand(U8 command)
{
	 SPI1_ASSERT;
	 SSP1_Duplex8(&command);                                     // Transmit write addr
	 SPI1_DE_ASSERT;
}
void spi1WriteByte(U8 addr, U8 pdata)
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
  SPI1_ASSERT;
  SSP1_Duplex8(&addr);                                     // Transmit write addr
  //delayMicroseconds(50);
  //delayMicroseconds(20);
  SSP1_Duplex8(&pdata);                                     // Transmit write data
  delayMicroseconds(3);                                  // 30us between write cmd request
  SPI1_DE_ASSERT;
  // [ test
  delayMicroseconds(180);                                  // 20us between read and write cmd request
  // ]
}


U8 spi1ReadByte(U8 addr)
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

  SPI1_ASSERT;
 // addr = (0x7F & addr);                                   // Assert the addr for spi read
  SSP1_Duplex8(&addr);                                    // Transmit write addr
  //delayMicroseconds(110);                                  // 4us request
  //delayMicroseconds(50);                                  // 4us request
	 delayMicroseconds(160);                                  // 4us request
  SSP1_Duplex8(&ret);
  //delayMicroseconds(100);                                  // 20us between read and write cmd request
  delayMicroseconds(3);                                  // 20us between read and write cmd request
  SPI1_DE_ASSERT;
  // [ test
  delayMicroseconds(180);                                  // 20us between read and write cmd request
  // ]
  return ret;
}

void spi1WriteBytes(U8 reg,U32 addr, U8 *pData, U16 length)
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
  U8 addrH=(U8)((addr>>16)&0xff);
	U8 addrM=(U8)((addr>>8)&0xff);
	U8 addrL=(U8)(addr&0xff);
  SPI1_ASSERT;  
  SSP1_Duplex8(&reg);                                    // Transmit write addr
	SSP1_Duplex8(&addrH);                                    // Transmit write addr
	SSP1_Duplex8(&addrM);                                    // Transmit write addr
	SSP1_Duplex8(&addrL);                                    // Transmit write addr
  delayMicroseconds(120);
  for (i = 0; i < length; i++) {                          // Transmit write data
    SSP1_Duplex8(pData);
    pData++;
    delayMicroseconds(3);                                // 30us request
    wdt_resetTimer();
  }
  SPI1_DE_ASSERT;
  delayMicroseconds(15);                                // 30us request
}

void spi1ReadBytes(U8 reg,U32 addr, U8 *pData, U32 length) 
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
	U8 Dummy=Dummy;		
  U8 addrH=(U8)((addr>>16)&0xff);
	U8 addrM=(U8)((addr>>8)&0xff);
	U8 addrL=(U8)(addr&0xff);
	while (LPC_SSP1->SR & SSP1SR_RNE) {                     // if recieve is not empy
    Dummy = LPC_SSP1->DR;                                 // clear the RxFIFO 
  }
  SPI1_ASSERT;  
	SSP1_Duplex8(&reg);                                     // Transmit register
  SSP1_Duplex8(&addrH);                                    // Transmit write addr
	SSP1_Duplex8(&addrM);                                    // Transmit write addr
	SSP1_Duplex8(&addrL);                                    // Transmit write addr
  delayMicroseconds(3);
  for (i = 0; i < length; i++) {
    SSP1_Duplex8(pData);
    pData++;
    delayMicroseconds(2);
		wdt_resetTimer();
  }
  SPI1_DE_ASSERT;
}
