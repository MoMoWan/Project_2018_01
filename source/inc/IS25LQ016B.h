#ifndef _IS25LQ016B_H_
#define _IS25LQ016B_H_
#include "main.h"
#define WIP   (1<<0)
#define WEL   (1<<1)
#define WRSR  0x01
#define PP    0x02
#define RD    0x03
#define RDSR  0x05
#define WREN  0x06
#define RSTEN 0x66
#define RST   0x99
#define RDUID 0x4B
#define RDID  0xab
#define SER   0xd7
extern U8 flashUID[20];
void initFlashIC(void);
U8 sectorProgram(U32 addr,U8 *pdata,U32 length);
U8 sectorErase(U32 addr);
U8 spiFlashRead(U32 addr,U8 *pData,U32 size);
#endif
