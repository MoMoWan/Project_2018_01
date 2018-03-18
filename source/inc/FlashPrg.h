#include "SN32F240B.h"

#ifndef __FLASHPRG_H__
#define __FLASHPRG_H__



#define	FLASH_WAIT_FOR_DONE	  __nop();	while (SN_FLASH->STATUS & FLASH_BUSY) {__nop();}

#define	FLASH_PROGRAM_ONE_WORD 1		//** 1word
#define	FLASH_PROGRAM_ONE_PAGE 16 		//** 16words

/*_____ DECLARATIONS ____________________________________________*/

/*_____ FUNCTIONS __________________________________________________*/


extern int FLASH_ProgramPage (uint32_t adr, uint32_t sz, uint32_t* buf);
extern int FLASH_Program(uint32_t adr, uint32_t sz, uint32_t* buf); 
extern int FLASH_Erase(uint32_t adr, uint32_t end, uint32_t data);
extern int FLASH_ErasePage (uint32_t adr, uint32_t sz, uint32_t data); 
//extern void FLASH_InitInfoData(void);
//extern void FLASH_GetInfoData(unsigned int *buf);

#endif /* _FLASHPRG_H__ */
