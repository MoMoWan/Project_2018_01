#include "gpio.h"
#include "main.h"
#include "spi1_driver.h"
#include <string.h>
#include "Type.h"
#include "gpio.h"
#include "wdt.h"
#include "IS25LQ016B.h"
 void setReg(U8 reg,U8 data)
 {
	 SPI1_ASSERT;	
	  SPI1_SET_MODE8;                                          // Enable 8-bit SPI mode
  while ((LPC_SSP1->SR & (SSP1SR_TFE | SSP1SR_BSY)) != SSP1SR_TFE); // Wait until not busy and TX FIFO empty
   LPC_SSP1->DR = reg;
	 while ((LPC_SSP1->SR & (SSP1SR_TFE | SSP1SR_BSY)) != SSP1SR_TFE); // Wait until not busy and TX FIFO empty
   LPC_SSP1->DR = data;
	 SPI1_DE_ASSERT;
 }
U8 sectorProgram(U32 addr,U8 *pdata,U32 length)
{
	U8 state;
	spi1ReadBytes(RDSR,0,&state,1); // Get the SR state
	if(!(state&WIP)){                // is not write in progress
		setReg(WRSR,0x02);// enable WEL
		delayMicroseconds(20);
		while(length--){
      spi1WriteBytes(PP,addr,pdata,length);		     			
		}
		return 1;                        // page program success 
	}else{
		return 0;
	}
}
U8 sectorErase(U32 addr)
{
	U8 state;
	spi1ReadBytes(RDSR,0,&state,1); // Get the SR state
	if(!(state&WIP)){
		setReg(WRSR,0x02);// enable WEL
		delayMicroseconds(20);
		spi1WriteBytes(SER,addr,&state,0);  // write erase sector register and addr ,data length is 0 ,so pdata will not be care
		return 1;
	}else{
		return 0;
	}
}
void flashICRest(void)
{
	spi1WriteCommand(RSTEN);
	spi1WriteCommand(RST);
}
U8 spiFlashRead(U32 addr,U8 *pData,U32 size)
{
	 U32 i;
	U8 Dummy=Dummy;		
	U8 reg=RD;
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
  for (i = 0; i < size; i++) {
    SSP1_Duplex8(pData);
    pData++;
    delayMicroseconds(2);
		wdt_resetTimer();
  }
  SPI1_DE_ASSERT;
	return 1;
}
void initFlashIC(void)
{
	 LPC_IOCON->PIO1_28 = 0x80;                             // Pin40, PIO0-16 sensor IRQ (GPIO, Input, internal pull-up)
	 gpio_Direction(1,  28, 1);                             // input
	 gpio_SetValue(1, 28, 1);                               // output low
	
	 LPC_IOCON->PIO1_16 = 0x80;                             // Pin40, PIO0-16 sensor IRQ (GPIO, Input, internal pull-up)
	 gpio_Direction(1,  16, 1);                             // input
	 gpio_SetValue(1, 16, 1);                               // output low
	 delayMilliseconds(20);
	 SSP1_Init();                                           // Initialize SSP0
	 SSP1_Enable();   
   flashICRest();    // Rest 	
	 spi1WriteCommand(WREN);      // Write enable
	
//	 delayMilliseconds(500);	 
//	 pageProgram(0x100,pageDate,256);
//	 //sectorErase(0);
//	 delayMilliseconds(500);	 
//	 delayMilliseconds(500);	 
//	 spi1ReadBytes(RD,0x100,getPageData,256);
}

