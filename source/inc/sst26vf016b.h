#ifndef _SST26VF016B_H_
#define _SST26VF016B_H_
#include "type.h"
#define SST_RSTEN_REG      (0x66)
#define SST_RST_RGE        (0x99)
#define SST_RDSTATUE_REG   (0x05)
#define SST_WREN_REG       (0x06)
#define SST_WRDIS_REG      (0x04)
#define SST_READ_REG       (0x03)
#define SST_SECTORERASE_REG (0x20)
#define SST_ERASECHIP_REG   (0xC7)
#define SST_PAGEPROGRAM_REG (0x02)
#define SST_WRBLOCKPROTECT_REG (0x42)
#define SST_JEDECID_REG      (0x9F)
#define SST_SECTOR_SIZE    (0x1000)


extern U8 exterFlashConnect;
void sst26Init(void);
void sstRead(U32 addr,U8* pData,U32 size);
void sstEraseSector(U32 addr);
void sstSectorProgram(U32 addr,U8 *pData);
void sstEraseChip(void);
void sstPageProgram(U32 page,U8* pData,U16 size)  ;
BOOL sstIsBusy(void);
U32 sstReadJedecId(void);
void setExterMemoryFree(U32 bitOffset,U16 blockSize);
#endif
