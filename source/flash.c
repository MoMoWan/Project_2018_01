#include "flash.h"
#include "iap.h"
#include "sst26vf016b.h"

#define BUFFER_SIZE (0x100)
U8 flashBuffer[BUFFER_SIZE] __attribute__((aligned(4))); 
BOOL flashPageWrite(U32 addr,U8* pData)
{
	bool ret;
	memcpy(flashBuffer,pData,BUFFER_SIZE);	
	__disable_irq();   
	ret=!iap_CopyToFlash(flashBuffer,(void*)addr,BUFFER_SIZE);
	 __enable_irq();  
	return ret;
}



