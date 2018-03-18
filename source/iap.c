/*++

Copyright (c) 2012-2014 Razer Ltd.
All rights reserved. This program contains proprietary and confidential information.

Module Name:
  _iap.c

Compiler:
  Keil Software µVision v4.72, with NXP Semiconductor LPC11Uxx proccessor support.

Abstract:
  This file provides the interface to the IAP (In Application Programming).

--*/

#include  "stdint.h"
#include  "iap.h"

U32 param_table[5];
U32 result_table[5];
//U32 flashBuffer[256 / 4] __attribute__((aligned(256)));   // Programming buffer

U8 flash_buffer[FLASH_PAGE_SIZE] __attribute__((aligned(4))); //

const IAP iap_entry = (IAP)IAP_ADDRESS;


void iap_PrepareSector(U32 sector_start, U32 sector_end)
/*++

Function Description:
  This function prepares a sector for write operation.

Arguments:
  sector_start - Starting sector.
  sector_end   - Ending sector.

Returns:
  NONE

--*/
{
  param_table[0] = PREPARE_SECTOR_FOR_WRITE;
  param_table[1] = sector_start;
  param_table[2] = sector_end;
  iap_entry(param_table, result_table);
}


U32 iap_EraseSector(U32 sector_start, U32 sector_end)
/*++

Function Description:
  This function prepares then erase a sector of flash.

Arguments:
  sector_start - Starting sector.
  sector_end   - Ending sector.

Returns:
  result_table

--*/
{
  iap_PrepareSector(sector_start, sector_end);
  if (result_table[0] == CMD_SUCCESS) {
    param_table[0] = ERASE_SECTOR;
    param_table[1] = sector_start;
    param_table[2] = sector_end;
    param_table[3] = IAP_CLOCK;
    iap_entry(param_table, result_table);
  }
  return result_table[0];
}


U32 iap_CopyToFlash(void *ram_addr,void *flash_addr, U32 count)
/*++

Function Description:
  This function prepares then writes a sector of flash.

Arguments:
  ram_addr   - Address of buffer containing 1K of data.
  flash_addr - Address of flash to be updated.
  count      - Data block size (1 kByte)

Returns:
  result_table

--*/
{
  U32 addr = (U32)flash_addr;
	if(addr<=0x17fff){                                                                             // When address is bigger than 0x17fff , every sector is 32k in U68
    iap_PrepareSector(addr/FLASH_SECTOR_SIZE, addr/FLASH_SECTOR_SIZE);
	}else{
		iap_PrepareSector((addr-0x18000)/FLASH_BIG_SECTOR_SIZE+24, (addr-0x18000)/FLASH_BIG_SECTOR_SIZE+24);
	}
	

  if (result_table[0] == CMD_SUCCESS) {
    param_table[0] = COPY_RAM_TO_FLASH;
    param_table[1] = (U32)flash_addr;
    param_table[2] = (U32)ram_addr;
    param_table[3] = count;
    param_table[4] = IAP_CLOCK;
    iap_entry(param_table, result_table);
  }
  return result_table[0];
}


U32 iap_ErasePage(U32 page_start, U32 page_end)
/*++

Function Description:
  This function prepares then erase a page of flash.

Arguments:
  sector_start - Starting sector.
  sector_end   - Ending sector.

Returns:
  result_table

--*/
{
	U32 start_sector,end_sector;
	U32 address;
	address=page_start<<8;
	if(address<=0x17fff){    // When address is bigger than 0x17fff , every sector is 32k in U68
		start_sector=address/FLASH_SECTOR_SIZE;  
	}else{
		start_sector=(address-0x18000)/FLASH_BIG_SECTOR_SIZE+24;
	}
	address=page_end<<8;
	if(address<=0x17fff){    // When address is bigger than 0x17fff , every sector is 32k in U68
		end_sector=address/FLASH_SECTOR_SIZE;  
	}else{
		end_sector=(address-0x18000)/FLASH_BIG_SECTOR_SIZE+24;
	}
	 iap_PrepareSector(start_sector, end_sector);
  if (result_table[0] == CMD_SUCCESS) {
    param_table[0] = ERASE_PAGE;
    param_table[1] = page_start;
    param_table[2] = page_end;
    param_table[3] = IAP_CLOCK;
    iap_entry(param_table, result_table);
  }
  return result_table[0];
}

U32 iap_WriteEEPROM(void *eeprom_addr,void *ram_addr, U32 count)
/*++

Function Description:
  This function writes data to EEPROM.

Arguments:
  eeprom_addr   - Address of eeprom to be writed.
  ram_addr - Address of ram data to be write to eeprom.
  count      - Data size to be write to eeprom

Returns:
  result_table

--*/
{
  param_table[0] = EEPROM_WRITE;
  param_table[1] = (U32)eeprom_addr;
  param_table[2] = (U32)ram_addr;
  param_table[3] = count;
  param_table[4] = IAP_CLOCK;
  iap_entry(param_table, result_table);
  return result_table[0];
}


U32 iap_ReadEEPROM(void *eeprom_addr,void *ram_addr, U32 count)
/*++

Function Description:
  This function read EEPROM data to ram.

Arguments:
  eeprom_addr   - Address of eeprom to be read.
  ram_addr - Address of ram to store the read data from eeprom.
  count      - Data size to be read from eeprom

Returns:
  result_table

--*/
{
  param_table[0] = EEPROM_READ;
  param_table[1] = (U32)eeprom_addr;
  param_table[2] = (U32)ram_addr;
  param_table[3] = count;
  param_table[4] = IAP_CLOCK;
  iap_entry(param_table, result_table);
  return result_table[0];
}


bool flash_write(const U8 *addr, U8 *data, U32 size)

/*++

Function Description:
  This function prepares then writes a sector of flash.

Arguments:
  *addr       - Point to the flash address to be write.
  *data       - Point to the data to be update to flash.
  size        - Data size

Returns:
  1 = sucess
  0 = fail

--*/

{
  U32 offset, page_start;
  U8  retry;
  U8 ret = true;
    
  __disable_irq();                                        // Disable interrupts while flashing, take about 12ms to write the eeprom

  while (size) {

    page_start = ((U32)addr) >> 8;                        // Get the start flash page 
    offset = ((U32)addr) & 0x00FF;                        // Get the start address offset of its sector
    
    memcpy((void *)flash_buffer, (void *)(page_start << 8), sizeof(flash_buffer)); 

    if ((size + offset) <= FLASH_PAGE_SIZE) {            // The last write byte is not the end of the sector
      // copy the data to be update to flash to the flash buffer
      memcpy((void *)(flash_buffer + offset), data, size);
      size = 0;                                           // update size to 0 and it indicator update finished
    } else {                                              // the last write byte of is equal or lager than the end of this sector
      memcpy((void *)(flash_buffer + offset), data, (FLASH_PAGE_SIZE - offset));
      size -= (FLASH_PAGE_SIZE - offset);
      addr += (FLASH_PAGE_SIZE - offset);
      data += (FLASH_PAGE_SIZE - offset);
    }

    retry = 0;
    while (iap_ErasePage(page_start, page_start) != CMD_SUCCESS) {
      if (++retry >= FLASH_OPERATE_TIMEOUT) {
        ret = false;
        break;
      }
    }
      
    if (ret != false) {                                       // page erase success
      retry = 0;
      while (iap_CopyToFlash(flash_buffer, (void *)(page_start << 8), FLASH_PAGE_SIZE) != CMD_SUCCESS) {
        if (++retry >= FLASH_OPERATE_TIMEOUT) {
          ret = false;
          break;
        }
      } 
    }
    
    if (ret == false) {
      break;
    }      
  }
  
  __enable_irq();                                         // Re-enable interrupts
  
  return ret;
}





