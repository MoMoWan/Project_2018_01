#include "storage.h"
#include "type.h"
#include "macro.h"
#include "iap.h"
#include "sst26vf016b.h"
#include "protocol.h"
#include "FlashPrg.h"
BLOCK_ASSIGN blockMange __attribute__((aligned(4)));;
void setInterMemoryBusy(U32 bitOffset,U16 blockSize)
/*++
Function Description:
  This function set the key map of the exter flash busy.
Arguments:
  bitOffset    -- start block bit offset
  blockSize    -- total block bits
Returns:
  NONE
--*/
{
	U8 i,j;
	U32 k;
	for(k=bitOffset;k<bitOffset+blockSize;k++){
		i=k/8;
	  j=k%8;
		blockMange.interMap[i]|=(1<<j);// Set bit
	}
	requestVariableUpdate(BLOCK_ASSIGN_CHANGE,NULL);
}
void setExterMemoryBusy(U32 bitOffset,U16 blockSize)
/*++
Function Description:
  This function set the key map of the exter flash busy.
Arguments:
  bitOffset    -- start block bit offset
  blockSize    -- total block bits
Returns:
  NONE
--*/
{
	U8 i,j;
	U32 k;
	for(k=bitOffset;k<bitOffset+blockSize;k++){
		i=k/8;
	  j=k%8;
		blockMange.exterMap[i]|=(1<<j);     // Set bit
	}
	requestVariableUpdate(BLOCK_ASSIGN_CHANGE,NULL);
}
void setExterMemoryFree(U32 bitOffset,U16 blockSize)
	/*++
Function Description:
  This function set the key map of the exter flash free.
Arguments:
  bitOffset    -- start block bit offset
  blockSize    -- total block bits
Returns:
  NONE
--*/
{
	U8 i,j;
	U32 k;
	for(k=bitOffset;k<bitOffset+blockSize;k++){  
		i=k/8;
	  j=k%8;
		blockMange.exterMap[i]&=~(1<<j);   // Clear bits
	}
	requestVariableUpdate(BLOCK_ASSIGN_CHANGE,NULL);
}
U16 mallocBlock(U32 size)
	/*++
Function Description:
  This function malloc the block , when inter flash run out ,then trun to exter flash.
It need the continue blocks to fit the size.Search the memory map to find out the start 
blocks

Arguments:
  size    -- size to malloc
Returns:
  The start blocks

--*/
{
	U16 i;
	U8 j;
	U32 blockNum;
	U32 cnt=0;
	U16 blockAddr;
	S16 blockStart;
	#if INTER_FLASH_ENABLE
	blockStart=-1;                                                   // Init the start block
	blockNum=size/PAGE_SIZE+1;                                       // Calculate the total block numbers
	for(i=0;i<INTER_MAX_MAP_BYTES;i++){
		for(j=0;j<8;j++){
			if(((blockMange.interMap[i]>>j)&0X01)==BLOCK_FREE){    // If the bit is free
				cnt++;                                                     // Update available blocks
				if(blockStart==-1){                                        // The availble  Block , need to record the start address
					blockStart=i*8+j;                                        // Calculate the start block
				}
			}else{                                                       // If the block is not available
				cnt=0;                                                     // Clear the available blocks
				blockStart=-1;                                             // Init the start block
			}
			if(cnt==blockNum){                                           // Check if the available blocks need the requiret blocks
				setInterMemoryBusy(blockStart,blockNum);                   // Set the key map busy
				blockAddr=(blockStart+INTER_BASIC_BLOCK);                  // Add the basic block then get the absolute block address
				if(blockAddr<=INTER_END_BLOCK){                            // Check the valid block
					return blockAddr;
				}				
			}
		}
	}
	#endif
	#if EXTER_FLASH_ENABLE
	if(exterFlashConnect){                                            // Only if the extern flash device connected
		blockStart=-1;                                                  // Init the start block 
		cnt=0;                                                          // Init the availables blocks
		blockNum=size/SECTOR_SIZE+1;                                    // Calculate the total block numbers
		for(i=0;i<EXTER_MAX_MAP_BYTES;i++){
			for(j=0;j<8;j++){
				if(((blockMange.exterMap[i]>>j)&0X01)==BLOCK_FREE){   // If the bit is free
					cnt++;                                                     // Update available blocks  
					if(blockStart==-1){                                        // The availble  Block , need to record the start address
						blockStart=i*8+j;                                        // Calculate the start block
					}                                                         
				}else{                                                       // If the block is not available
					cnt=0;                                                     // Clear the available blocks
					blockStart=-1;                                             // Init the start block
				}                                                           
				if(cnt==blockNum){                                           // Check if the available blocks need the requiret blocks
					setExterMemoryBusy(blockStart,blockNum);                   // Set the key map busy
					blockAddr=blockStart+EXTER_BASIC_BLOCK;                    // Add the basic block  then get the absolute block address
					if(blockAddr<=EXTER_END_BLOCK){                            // Check the valid blockock
						return blockAddr|EXTER_BLOCK_FLAG;                       // Add the exter flag the get the exter block address
					}
				}
			}
		}
  }
	#endif
	return 0;   
}
U32 getFreeInterMemory(void)
	/*++
Function Description:
  This function free inter memory.the Map every bit if "1", Then is 
not free;

Arguments:
  NONE
Returns:
  NONE

--*/
{
	U32 cnt=0;
	U16 i;
	U8 j;
	for(i=0;i<INTER_MAX_MAP_BYTES;i++){                              //Search the whole key map
		for(j=0;j<8;j++){
			if(((blockMange.interMap[i]>>j)&0X01)==BLOCK_FREE){    // If the bit free
				cnt++;
			}
		}
	}
	return cnt*PAGE_SIZE;                                            // Return the sizes
}
U32 getFreeExterMemory(void)
	/*++
Function Description:
  This function free exter memory.the Map every bit if "1", Then is 
not free;

Arguments:
  NONE
Returns:
  NONE

--*/
{
	U32 cnt=0;
	U16 i;
	U8 j;
	for(i=0;i<EXTER_MAX_MAP_BYTES;i++){                            //Search the whole key map
		for(j=0;j<8;j++){
			if(((blockMange.exterMap[i]>>j)&0x01)==BLOCK_FREE){  // If the bit free
				cnt++;
			}
		}
	}
	return cnt*SECTOR_SIZE;                                       // Return the sizes
}
U32 getFreeMemory(void)
{
	U32 ret=0;
	#if INTER_FLASH_ENABLE
	ret=getFreeInterMemory();
	#endif
	#if EXTER_FLASH_ENABLE
	if(exterFlashConnect){  // Only if the exter flash device connected
	  ret+=getFreeExterMemory();
	}
	#endif
	return ret;
}
U32 getMaxMemory(void)
/*++
Function Description:
  This function Get the max memory
exter flash

Arguments:
  NONE
Returns:
  NONE

--*/
{
	U32 ret=0;
	#if INTER_FLASH_ENABLE
	ret=INTER_MAX_SIZE;
	#endif
	#if EXTER_FLASH_ENABLE
	if(exterFlashConnect){ // Only if the exter flash device connected
	  ret+=EXTER_MAX_SIZE;
	}
	#endif
	return ret;
}
U32 getRecycleSize(void)
{
	return macroTable.recycleSizes;
}
U16 getMaxMacroIDS(void)
{
	return MACRO_MAX_NUMBERS;
}
void formatMacroMemory(void)
/*++
Function Description:
  This function format the macro data , include inter flash and 
exter flash

Arguments:
  NONE
Returns:
  NONE

--*/
{
//	#if INTER_FLASH_ENABLE
//	U32 startSector,endSector;
//	if((INTER_BASIC_BLOCK<<8)<=0x17fff){                                     // When address is bigger than 0x17fff , every sector is 32k in U68
//		startSector=(INTER_BASIC_BLOCK<<8)/FLASH_SECTOR_SIZE;                  // Calculate the correct sector
//	}else{
//		startSector=((INTER_BASIC_BLOCK<<8)-0x18000)/FLASH_BIG_SECTOR_SIZE+24; // Calculate the correct sector
//	}
//	if((INTER_END_BLOCK<<8)<=0x17fff){                                       // When address is bigger than 0x17fff , every sector is 32k in U68
//		endSector=(INTER_END_BLOCK<<8)/FLASH_SECTOR_SIZE;                      // Calculate the correct sector  
//	}else{
//		endSector=((INTER_END_BLOCK<<8)-0x18000)/FLASH_BIG_SECTOR_SIZE+24;      // Calculate the correct sector    
//	}
//	 __disable_irq();                                      // Disable interrupts while flashing
//	iap_EraseSector(startSector,endSector);                                   // Erase the sector
//	 __enable_irq();
  
  
  //[
    FLASH_Erase(0xC000, 0xF000,0); 
  //]
	memset(blockMange.interMap,0,sizeof(blockMange.interMap));
//	#endif
//	#if EXTER_FLASH_ENABLE
//	if(exterFlashConnect){                                                   // Only if the exter flash device connected
//	 sstEraseChip();                                                         // Reset all memory for test
//	 memset(blockMange.exterMap,0,sizeof(blockMange.exterMap));
//	}
//	#endif
	requestVariableUpdate(BLOCK_ASSIGN_CHANGE,NULL);
}
void readFlash(U32 addr,U8 *pData,U32 size)
/*++
Function Description:
  This function for read the flash data.

Arguments:
  flag:  1: MCU internal flash data will be read, 2:extern flash will be read
  addr:  The start address of reading data
	pData:  The first data address to send back
	size:   Total required data size
Returns:
  1: Sucess  0:Fail

--*/
{
//	if(addr&EXTER_ADDR_FLAG){    // Extern flash
//		sstRead(addr,pData,size);
//	}else{
		memcpy(pData,(void *)addr,size);
//	}
}
