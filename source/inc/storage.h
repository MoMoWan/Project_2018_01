#ifndef _STROAGE_H_
#define _STROAGE_H_
#include "type.h"
#define INTER_FLASH_ENABLE 1
#define EXTER_FLASH_ENABLE 0
#define BLOCK_FREE        (0)
#define INTER_BASIC_BLOCK (0x300)//(0x200)  //0xC000/0x40
#define INTER_END_BLOCK   (0x3C0)//(0X3FF)  //0xF000/0x40
#define INTER_MAX_SIZE    ((INTER_END_BLOCK-INTER_BASIC_BLOCK+1)*PAGE_SIZE)
#define EXTER_BASIC_BLOCK (0x00000000)
#define EXTER_END_BLOCK   (0x000001FF)
#define EXTER_MAX_SIZE    ((EXTER_END_BLOCK-EXTER_BASIC_BLOCK+1)*SECTOR_SIZE)
#define EXTER_BLOCK_FLAG  (0x8000)
#define EXTER_ADDR_FLAG   (0x8000000)
#define PAGE_SIZE         (0x40)   //0x100
#define SECTOR_SIZE       (0x1000)
#define CRC_SIZE          (2)                 // Size of crc value

#define INTER_MAX_MAP_BYTES  (INTER_MAX_SIZE/(PAGE_SIZE*8))
#define EXTER_MAX_MAP_BYTES  (EXTER_MAX_SIZE/(SECTOR_SIZE*8))

typedef __packed struct _BLOCK_ASSIGN_{
	U8 interMap[INTER_MAX_MAP_BYTES];       // Map by bit 
	U8 exterMap[EXTER_MAX_MAP_BYTES];       // Map by bit
	U16 crc;
}BLOCK_ASSIGN;

extern BLOCK_ASSIGN blockMange;
U16 mallocBlock(U32 size);
U32 getFreeInterMemory(void);
U32 getFreeExterMemory(void);
void formatMacroMemory(void);
void readFlash(U32 addr,U8 *pData,U32 size);
U32 getMaxMemory(void);
U32 getFreeMemory(void);
U16 getMaxMacroIDS(void);
U32 getRecycleSize(void);
#endif
