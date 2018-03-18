#include "m25pe20.h"
#include "Type.h"
#include "spi_driver.h"
#include "gpio.h"
void externFlashInit(void)
{
	 LPC_IOCON->PIO1_28 = 0x80;                             // Pin40, PIO1-28 reset pin(GPIO, Input, internal pull-up)
	 gpio_Direction(1,  28, 1);                            // out put 
	 gpio_SetValue(1, 28, 0);                              // output hight to make it enter reset mode
	 delayMilliseconds(20);
	 gpio_SetValue(1, 28, 1);                              // output hight to make it enter normal mode
	
	
	 LPC_IOCON->PIO1_16 = 0x80;                             // Pin40, PIO0-16 sensor IRQ (GPIO, Input, internal pull-up)
	 gpio_Direction(1,  16, 1);                              // input
	 gpio_SetValue(1, 16, 1);                                // output low
	 delayMilliseconds(20);
	 SSP1_Init();                                            // Initialize SSP0
	 SSP1_Enable();   
	 delayMilliseconds(20);
}
void writeEnable(void)                                    // Enable write operation
{                                  
	U8 reg=WRENABLE;
	SPI1_ASSERT;
	SSP1_Duplex8(&reg);                                    // Transmit write addr
	SPI1_DE_ASSERT;
	delayMilliseconds(20);
} 
void writeDisable(void)                                  // Disable the write operation
{
	U8 reg=WRDISABLE;
	SPI1_ASSERT;
	SSP1_Duplex8(&reg);                                    // Transmit write addr
	SPI1_DE_ASSERT;
}
void readID(U8 * pData,U8 length)                        // Read the identification
{
	U8 reg=RDUID;
	SPI1_ASSERT;
	SSP1_Duplex8(&reg);                                    // Transmit write addr
	while(length--){
		SSP1_Duplex8(pData++);
	}
	SPI1_DE_ASSERT;
}
U8 readSR(void)                                   // Read the statues register
{
	U8 reg=RDSR;
	U8 ret=0;
	SPI1_ASSERT;
	SSP1_Duplex8(&reg);                              // Transmit write addr
	SSP1_Duplex8(&ret);                             // Get the ret value
	SPI1_DE_ASSERT;
	return ret;
}
void writeSR(U8 Data)                                   // Read the statues register
{
	U8 reg=WRSR;
	SPI1_ASSERT;
	SSP1_Duplex8(&reg);                                    // Transmit write addr
	SSP1_Duplex8(&Data);
	SPI1_DE_ASSERT;
	delayMilliseconds(20);
}
void readBytes(U8 *pData,U32 addr,U8 length)
{
	U8 reg=RDBYTES;
	SPI1_ASSERT;
	SSP1_Duplex8(&reg);                                    // Transmit write addr
	SSP1_Duplex8((U8*)&addr+2);                                    // Transmit write addr
	SSP1_Duplex8((U8*)&addr+1);                                    // Transmit write addr
	SSP1_Duplex8((U8*)&addr);                                    // Transmit write addr
	while(length--){
	  SSP1_Duplex8(pData++);
	}
	SPI1_DE_ASSERT;
}
U8 pageWrite(U32 addr,U8 *pData,U16 length)
{
	U8 reg=PW;
	U8 sr=readSR();
	U8 ret=0;
	if(((sr&WEL)==WEL)&&((sr&WIP)==0)){                      // If write enable latch ,and not in write progress
	  SPI1_ASSERT;
	  SSP1_Duplex8(&reg);                                    // Transmit write addr
	  SSP1_Duplex8((U8 *)&addr+2);                            // Hight byte
	  SSP1_Duplex8((U8 *)&addr+1);                            // Middle byte
	  SSP1_Duplex8((U8 *)&addr);                              // Low byte
	  while(length--){
		  SSP1_Duplex8(pData++);     
	  }
	  SPI1_DE_ASSERT;
		ret=1;
	}
	return ret;
}
U8 pageErase(U32 addr)
{
	U8 reg=PAGEERASE;
	U8 sr=readSR();
	U8 ret=0;
	if(((sr&WEL)==WEL)&&((sr&WIP)==0)){                      // If write enable latch ,and not in write progress
	  SPI1_ASSERT;
	  SSP1_Duplex8(&reg);                                    // Transmit write addr
	  SSP1_Duplex8((U8 *)&addr+2);                            // Hight byte
	  SSP1_Duplex8((U8 *)&addr+1);                            // Middle byte
	  SSP1_Duplex8((U8 *)&addr);                              // Low byte
	  SPI1_DE_ASSERT;
		ret=1;
	}
	return ret;
}
U8 pageProgram(U32 addr,U8 *pData,U16 length)
{
	U8 reg=PAGEPROGRAM;
	U8 sr=readSR();
	U8 ret=0;
	if(((sr&WEL)==WEL)&&((sr&WIP)==0)){                      // If write enable latch ,and not in write progress
	  SPI1_ASSERT;
	  SSP1_Duplex8(&reg);                                    // Transmit write addr
	  SSP1_Duplex8((U8 *)&addr+2);                            // Hight byte
	  SSP1_Duplex8((U8 *)&addr+1);                            // Middle byte
	  SSP1_Duplex8((U8 *)&addr);                              // Low byte
	  while(length--){
		  SSP1_Duplex8(pData++);     
	  }
	  SPI1_DE_ASSERT;
	  ret=1;
 }
	return ret;
}
U8 subSectorErase(U32 addr)
{
	U8 reg=SUB_SECTORERASE;
	U8 sr=readSR();
	U8 ret=0;
	if(((sr&WEL)==WEL)&&((sr&WIP)==0)){                      // If write enable latch ,and not in write progress
	  SPI1_ASSERT;
	  SSP1_Duplex8(&reg);                                    // Transmit write addr
	  SSP1_Duplex8((U8 *)&addr+2);                            // Hight byte
	  SSP1_Duplex8((U8 *)&addr+1);                            // Middle byte
	  SSP1_Duplex8((U8 *)&addr);                              // Low byte
	  SPI1_DE_ASSERT;
		ret=1;
	}
	return ret;
}
U8 sectorErase(U32 addr)
{
	U8 reg=SECTORERASE;
	U8 sr=readSR();
	U8 ret=0;
	if(((sr&WEL)==WEL)&&((sr&WIP)==0)){                      // If write enable latch ,and not in write progress
	  SPI1_ASSERT;	
	  SSP1_Duplex8(&reg);                                    // Transmit write addr
	  SSP1_Duplex8((U8 *)&addr+2);                            // Hight byte
	  SSP1_Duplex8((U8 *)&addr+1);                            // Middle byte
	  SSP1_Duplex8((U8 *)&addr);                              // Low byte
	  SPI1_DE_ASSERT;
		ret=1;
	}
  return ret;
}
U8 bulkErase(void)
{
	U8 reg=BULKERASE;
	U8 sr=readSR();
	U8 ret=0;
	if(((sr&WEL)==WEL)&&((sr&WIP)==0)){                      // If write enable latch ,and not in write progress
	  SPI1_ASSERT;
	  SSP1_Duplex8(&reg);                                    // Transmit write addr                              // Low byte
	  SPI1_DE_ASSERT;
	  ret=1;
	}
	return ret;
}
