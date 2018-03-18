#ifndef _M25PE20_H_
#define _M25PE20_H_
#include "Type.h"
#define WRENABLE   0x06          // Write enable
#define WRDISABLE  0x04          // Write disable
#define RDUID      0x9F          // Read indetification
#define RDSR       0x05          // Read status register
#define WRSR       0x01           // Wirte status register
#define RDBYTES    0x03         // Read bytes
#define PW         0x0A         // Page write
#define PAGEERASE  0xDB       // Page erase
#define PAGEPROGRAM 0x02     // Page program
#define SUB_SECTORERASE 0x20  // Subsector erase
#define SECTORERASE   0xD8    // Sector erase
#define BULKERASE   0xC7      // Bulk erase

#define WIP       (1<<0)           // Write in producess
#define WEL       (1<<1)           // Write enable latch
#define SRWD      (1<<7)           // Status register write protect
void readID(U8 * pData,U8 length);
void externFlashInit(void);
U8 readSR(void);
void writeEnable(void);
void writeSR(U8 Data);                                   // Read the statues register
void readBytes(U8 *pData,U32 addr,U8 length);                     // Read 
U8 pageWrite(U32 addr,U8 *pData,U16 length);
U8 pageErase(U32 addr);                                  // Erase the page data
U8 pageProgram(U32 addr,U8 *pData,U16 length);           // Page program
U8 subSectorErase(U32 addr);                             // Sub sector erase
U8 sectorErase(U32 addr);                               // Erase sector
U8 bulkErase(void);                                     // Erase bulk
#endif 
