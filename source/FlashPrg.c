/***********************************************************************/
/*  This file is part of the ARM Toolchain package                     */
/*  Copyright (c) 2010 Keil - An ARM Company. All rights reserved.     */
/***********************************************************************/
/*                                                                     */
/*  FlashDev.C:  Flash Programming Functions adapted                   */
/*               for SN32F700 Flash                            		   */
/*                                                                     */
/***********************************************************************/

#include "FlashPrg.h"
#include "wdt.h"
/*_____ DECLARATIONS ____________________________________________*/
const uint32_t tbFLASH_ModeAddress = 0x77FC;//30K Size 		Max Address


/*_____ DEFINITIONS ______________________________________________*/
/*****************************************************************************
* Description	: Flash Control Register definitions
*****************************************************************************/
#define FLASH_PG		0x00000001
#define FLASH_PER		0x00000002
#define FLASH_START		0x00000040

/*_____ D E F I N I T I O N S ______________________________________________*/

/*****************************************************************************
* Description	: Flash Status Register definitions
*****************************************************************************/
#define FLASH_BUSY							0x00000001
#define FLASH_ERR							0x00000004
#define	CODE_SECURITY_ADDR					0x1FFF2000 

#define FLASH_INFORMATION_ADDR				0x7000
/*_____ MACROS ________________________________________________________*/
/*_____ FUNCTIONS _____________________________________________________*/

//void FLASH_InitInfoData(void);
//void FLASH_GetInfoData(unsigned int *buf);


/*****************************************************************************
** Descriptions:		FLASH_ProgramPage
**
** parameters:			starting rom address to program , program size, source user pointer to program
** Returned value:	0: Pass, 1: Fail
** Descriptor:			
***************************************************************************/
int FLASH_ProgramPage (uint32_t adr, uint32_t sz, uint32_t* buf) 
{ 

	SN_FLASH->STATUS  &= ~(FLASH_ERR);									// Reset Error Flags

	SN_FLASH->CTRL  =  FLASH_PG;											// Programming Enabled
	SN_FLASH->ADDR  =  adr;
	FLASH_WAIT_FOR_DONE
	
	while (sz) 
	{
		SN_FLASH->DATA  = (*buf);
		__nop();
		 buf++;
		 sz--;
	}
	
	SN_FLASH->CTRL   |= FLASH_START;                       

	FLASH_WAIT_FOR_DONE

	if (SN_FLASH->STATUS & FLASH_ERR)
	{   
		return (1);																			// Failed
	}
	else
	{
		return (0);																			// Done
	}
}
/*****************************************************************************
** Descriptions:		FLASH_ErasePage
**
** parameters:			starting rom address to program , program size, source user pointer to program
** Returned value:	0: Pass, 1: Fail
** Descriptor:			
***************************************************************************/

int FLASH_ErasePage (uint32_t adr, uint32_t sz, uint32_t data) 
{ 

	SN_FLASH->STATUS  &= ~(FLASH_ERR);									// Reset Error Flags

	SN_FLASH->CTRL  =  FLASH_PER;											// Programming Enabled
	SN_FLASH->ADDR  =  adr;
//	FLASH_WAIT_FOR_DONE
	
//	while (sz) 
//	{
//		SN_FLASH->DATA  = data;
//		__nop();
//		 sz--;
//	}
	
	SN_FLASH->CTRL   |= FLASH_START;                       

	FLASH_WAIT_FOR_DONE

	if (SN_FLASH->STATUS & FLASH_ERR)
	{   
		return (1);																			// Failed
	}
	else
	{
		return (0);																			// Done
	}
}
/*****************************************************************************
** Descriptions:		FLASH_Program
**
** parameters:			None
** Returned value:	None
** Descriptor:			Init bufFLASH_InfoData content
***************************************************************************/
int FLASH_Erase(uint32_t adr, uint32_t end, uint32_t data) 
{
  uint32_t counter = 0;
  uint8_t result = 1;
  counter = adr;
	while (counter < end) 
	{
    wdt_resetTimer(); 
//    if (sz > 16) {
//      result = FLASH_ProgramPage((adr + (counter*64)), 16,(buf + (counter*16)));
//      sz -=16;
//    } else {
//      result = FLASH_ProgramPage((adr + (counter*64)), sz,(buf + (counter*16)));
//      sz = 0;
//    }
//    counter++;
//    result = FLASH_ErasePage((adr+(counter*64)),16,data);
    result = FLASH_ErasePage(counter,16,data);
    counter+=64;
    if (result) {
      return result;
    }      
  }
  return result;  
}

/*****************************************************************************
** Descriptions:		FLASH_Program
**
** parameters:			None
** Returned value:	None
** Descriptor:			Init bufFLASH_InfoData content
***************************************************************************/
int FLASH_Program(uint32_t adr, uint32_t sz, uint32_t* buf) 
{
  uint8_t counter = 0;
  uint8_t result = 1;
	while (sz) 
	{
//    if (sz > 16) {
//      result = FLASH_ProgramPage((adr + (counter*64)), 16,(buf + (counter*16)));
//      sz -=16;
//    } else {
//      result = FLASH_ProgramPage((adr + (counter*64)), sz,(buf + (counter*16)));
//      sz = 0;
//    }
//    counter++;
    result = FLASH_ProgramPage((adr+(counter*64)),16,(buf + (counter*16)));
    sz--;
    counter++;
    if (result) {
      return result;
    }      
  }
  return result;  
}
/*****************************************************************************
** Descriptions:		FLASH_InitInfoData
**
** parameters:			None
** Returned value:	None
** Descriptor:			Init bufFLASH_InfoData content
***************************************************************************/
//void FLASH_InitInfoData(void)
//{
//	bufFLASH_InfoData[0] = *(volatile uint32_t *)(FLASH_INFORMATION_ADDR+4);		
//}





