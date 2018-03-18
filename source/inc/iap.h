/*++

Copyright (c) 2012-2014 Razer Ltd.
All rights reserved. This program contains proprietary and confidential information.

Module Name:
  iap.h

Compiler:
  Keil Software µVision v4.72, with NXP Semiconductor LPC11Uxx proccessor support.

Abstract:
  This file contains the definitions for _iap.c, which provides the interface to
  the IAP (In Application Programming).

--*/

#ifndef _IAP_H_
#define _IAP_H_

#include "main.h"
#include "type.h"

#define CMD_SUCCESS                   0
#define IAP_ADDRESS                   0x1FFF1FF1
#define IAP_CLOCK                     48000               // Clock in kilo-hertz


#define FLASH_PAGE_SIZE              (0x100)
#define FLASH_SECTOR_SIZE            (0x1000)
#define FLASH_BIG_SECTOR_SIZE        (0x8000)
#define FLASH_PAGES_PER_SECTOR       (FLASH_SECTOR_SIZE/FLASH_PAGE_SIZE)

#define FLASH_OPERATE_TIMEOUT         (4)


typedef enum {
  PREPARE_SECTOR_FOR_WRITE = 50,
  COPY_RAM_TO_FLASH        = 51,
  ERASE_SECTOR             = 52,
  BLANK_CHECK_SECTOR       = 53,
  READ_PART_ID             = 54,
  READ_BOOT_VER            = 55,
  COMPARE                  = 56,
  REINVOKE_ISP             = 57,
  READ_UID                 = 58,
  ERASE_PAGE               = 59,
  EEPROM_WRITE             = 61,
  EEPROM_READ              = 62
} IAP_Command_Code;

typedef void (*IAP)(U32[], U32 []);


void iap_PrepareSector    (U32 sector_start, U32 sector_end);
U32  iap_EraseSector      (U32 sector_start, U32 sector_end);
U32  iap_CopyToFlash      (void *ram_addr,void *flash_addr, U32 count);

U32 iap_ErasePage(U32 page_start, U32 page_end);

void dfu_CopyMemory       (void *dst, void *src, U16 count);
U16  dfu_CompareMemory    (void* dst, void* src, U16 count);
void dfu_InitializeBuffer (void);

extern U8 flash_buffer[FLASH_PAGE_SIZE];

U32 iap_WriteEEPROM(void *eeprom_addr,void *ram_addr, U32 count);

U32 iap_ReadEEPROM(void *eeprom_addr,void *ram_addr, U32 count);

bool flash_write(const U8 *addr, U8 *data, U32 size);

#endif // _IAP_H_
