#include "sst26vf016b.h"
#include "spi1_driver.h"
#include "gpio.h"
U8 exterFlashConnect=0;
void sstReset(void)
{
	U8 reg;
	reg=SST_RSTEN_REG;
	SPI1_ASSERT;
	SSP1_Duplex8(&reg);                                     // Transmit write addr
	SPI1_DE_ASSERT;
	//delayMicroseconds(1);                                   // Wait
	__NOP();
	__NOP();
	reg=SST_RST_RGE;
	SPI1_ASSERT;
	SSP1_Duplex8(&reg);                                     // Transmit write addr
	SPI1_DE_ASSERT;
	delayMicroseconds(10);                                   // Wait
}
void sstWrEnable(void)
{
	U8 reg=SST_WREN_REG;
	SPI1_ASSERT;
	SSP1_Duplex8(&reg);                                     // Transmit write addr
	SPI1_DE_ASSERT;
	delayMicroseconds(10);
}
void sstWrBlockProtect(void)
{
	U8 reg=SST_WRBLOCKPROTECT_REG;
	U8 val[18]={0};
	U8 i;
	sstWrEnable();
	SPI1_ASSERT;
	SSP1_Duplex8(&reg);                                     // Transmit write addr
	for(i=0;i<sizeof(val);i++){
		SSP1_Duplex8((&val[i]));
	}
	SPI1_DE_ASSERT;
	delayMicroseconds(10);
}
U8 sstRdStatus(void)
{
	U8 reg=SST_RDSTATUE_REG;
	U8 ret=0xff;
	SPI1_ASSERT;
	SSP1_Duplex8(&reg);                                     // Transmit write addr
	SSP1_Duplex8(&ret);                                     // Transmit write addr
	SPI1_DE_ASSERT;
	delayMicroseconds(10);
	return ret;
}
void sstEraseChip(void)
{
	U8 reg=SST_ERASECHIP_REG;
	U8 getStatue;
	U16 timeOut=0;
	while(1){
		sstWrEnable();
		getStatue=sstRdStatus();
		if((getStatue&0x01)||(getStatue&0x02)==0){                               // Busy
			delayMicroseconds(10);                              // Wait
			if(timeOut++>10000){                                // If the 100ms also busy , return fail
				return ;
			}
		}else{                                                // Free
			break;                                              // Continue
		}			
	}
	SPI1_ASSERT;
	SSP1_Duplex8(&reg);                                     // Transmit write addr
	SPI1_DE_ASSERT;
	delayMicroseconds(10);
}
void sst26Init(void)
{
	U32 deviceInfor;
	SSP1_Init();
	SSP1_Enable();
	LPC_IOCON->PIO1_24=0x80;  // Reset pin
	gpio_Direction(1,24,1);   // Out put
	gpio_SetValue(1,24,0);
	delayMilliseconds(10);
	gpio_SetValue(1,24,1);    // Finishi reset
	LPC_IOCON->PIO2_7=0x80;  // WP pin 
	gpio_Direction(2,7,1);   // Out put
	gpio_SetValue(2,7,1);     // For temp ,not protect
	sstReset();
	delayMilliseconds(10);
	sstWrBlockProtect();
	deviceInfor=sstReadJedecId();
	if((deviceInfor>>8)==0xBF26){  // ID Match , Just match Manufacturer ID and device type
		exterFlashConnect=1;        // Statue connected
	}else{
		exterFlashConnect=0;
	}
}
void sstWrDisable(void)
{
	U8 reg=SST_WRDIS_REG;
	SPI1_ASSERT;
	SSP1_Duplex8(&reg);                                     // Transmit write addr
	SPI1_DE_ASSERT;
}
void sstRead(U32 addr,U8* pData,U32 size)
{
	U8 reg=SST_READ_REG;
	U8 addrBuffer[3];
  addrBuffer[0]=(addr>>16)&0xff;
	addrBuffer[1]=(addr>>8)&0xff;
	addrBuffer[2]=addr&0xff;
//	while(1){
//		sstWrEnable();
//		getStatue=sstRdStatus();
//		if(getStatue&0x01){                               // Busy
//			delayMicroseconds(10);                              // Wait
//		}else{                                                // Free
//			break;                                              // Continue
//		}			
//	}
	SPI1_ASSERT;
	SSP1_Duplex8(&reg);                                     // Transmit write addr
	SSP1_Duplex8(&addrBuffer[0]);                           // Transmit write addr
	SSP1_Duplex8(&addrBuffer[1]);                           // Transmit write addr
	SSP1_Duplex8(&addrBuffer[2]);                           // Transmit write addr
	while(size--)                                           // Read 
		SSP1_Duplex8(pData++);
	SPI1_DE_ASSERT;
	delayMicroseconds(10);
}
void sstEraseSector(U32 addr)
{
	U8 reg=SST_SECTORERASE_REG;
	U8 addrBuffer[3];
	U8 getStatue;
	U16 timeOut=0;
	addrBuffer[0]=(addr>>16)&0xff;
	addrBuffer[1]=(addr>>8)&0xff;
	addrBuffer[2]=addr&0xff;
	while(1){
		sstWrEnable();
		getStatue=sstRdStatus();
		if((getStatue&0x01)||(getStatue&0x02)==0){                               // Busy
			delayMicroseconds(10);                              // Wait
			if(timeOut++>10000){                                // If the 100ms also busy , return fail
				return ;
			}
		}else{                                                // Free
			break;                                              // Continue
		}			
	}
	SPI1_ASSERT;
	SSP1_Duplex8(&reg);                                     // Transmit write addr
	SSP1_Duplex8(&addrBuffer[0]);                           // Transmit write addr
	SSP1_Duplex8(&addrBuffer[1]);                           // Transmit write addr
	SSP1_Duplex8(&addrBuffer[2]);                           // Transmit write addr
	SPI1_DE_ASSERT;
	delayMicroseconds(10);
}
void sstPageProgram(U32 page,U8* pData,U16 size)          // Program 256 bytes
{
	U32 addr=page<<8;
	U8 reg=SST_PAGEPROGRAM_REG;
	U8 addrBuffer[3];
	U8 getStatue;
	U16 timeOut=0;
	addrBuffer[0]=(addr>>16)&0xff;
	addrBuffer[1]=(addr>>8)&0xff;
	addrBuffer[2]=addr&0xff;
	while(1){
		sstWrEnable();
		getStatue=sstRdStatus();
		if((getStatue&0x01)||(getStatue&0x02)==0){                               // Busy
			delayMicroseconds(10);                              // Wait
			if(timeOut++>10000){                                // If the 100ms also busy , return fail
				return ;
			}
		}else{                                                // Free
			break;                                              // Continue
		}			
	}
	SPI1_ASSERT;
	SSP1_Duplex8(&reg);                                     // Transmit write addr
	SSP1_Duplex8(&addrBuffer[0]);                           // Transmit write addr
	SSP1_Duplex8(&addrBuffer[1]);                           // Transmit write addr
	SSP1_Duplex8(&addrBuffer[2]);                           // Transmit write addr
	while(size--){
		SSP1_Duplex8(pData++);                           // Transmit write addr
	}
	SPI1_DE_ASSERT;
	delayMicroseconds(10);
}
void sstSectorProgram(U32 addr,U8 *pData)
{
	U8 i;
	U32 page=addr>>8;;
	for(i=0;i<16;i++){                                     // 16 pages a sector
		sstPageProgram(page+i,(pData+0x100*i),0x100);
	}
	delayMicroseconds(10);
}
BOOL sstIsBusy(void)
{
	if(sstRdStatus()&0x81){
		return TRUE;
	}
	return FALSE;
}
U32 sstReadJedecId(void)
{
	U8 reg=SST_JEDECID_REG;
	U8 buffer[3];
	U32 ret;
	SPI1_ASSERT;
	SSP1_Duplex8(&reg);                                    // Transmit write addr
	SSP1_Duplex8(&buffer[0]);                           // Manufacturer ID
	SSP1_Duplex8(&buffer[1]);                           // Device Type
	SSP1_Duplex8(&buffer[2]);                           // Device ID 
	SPI1_DE_ASSERT;
	ret=(buffer[0]<<16)|(buffer[1]<<8)|(buffer[1]);    
	return ret;             
}
