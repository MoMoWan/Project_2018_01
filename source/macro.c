#include "macro.h"
#include "type.h"
#include "iap.h"
#include "mouse.h"
#include "button.h"
#include "flash.h"
//#include "sst26vf016b.h"
#include "storage.h"
#include "keyboard.h"
#include "FlashPrg.h"

MACRO_TABLE macroTable __attribute__((aligned(4)));;
ACTIVE_MACRO macroActive[MAX_ACTIVE_MACRO];
U8 macroEven=0;
U8 macroBuffer[MAX_ACTIVE_MACRO][MACRO_BUFFER_SIZE];
U8 macroKeyQueue[MAX_KEY];
U32 delayTime=0;

U8 macroButton=0;                                      //Backup the Mouse button state from macro data
U8 macroScroll=0;                                      // Backup the scroll data from macro data
S16 macroXCursor=0;
S16 macroYCursor=0;

U8  Macro_Ep1_In[MACRO_EP1_SIZE] __attribute__((aligned(4)));                        // Macro end point 1 in 
U8  Macro_Ep2_In[MACRO_EP2_SIZE] __attribute__((aligned(4)));                       // Macro end point 2 in 
U8  Macro_Ep3_In[MACRO_EP3_SIZE] __attribute__((aligned(4)));                        // Macro end point 3 in 

U8 memoryManageStaue;                                 // 	Macro Memory Management 
U8 macroMemoryCheck;                                  // Check the macro memory 
U16 erroMacroId;                                      // 
bool protocolWait=FALSE;                          //   
ERASE_FLASH exterFlashErase;                          // Manage the exter flash erase operation


bool macroIsExist(U16 id)
/*++

Function Description:
  This function check the macro is exist or not.
 
Arguments:
    id    -- The target id to check

Returns: 
    true  -- The macro exist
    false -- The macro not exist

--*/
{
	U16 i;
	bool ret=FALSE;
	U16 j=0;
	for(i=0;i<macroTable.macroNumbers+j;i++){         // Check every macro table 
		if(macroTable.macro[i].id.id==id){                 // Id match
			ret=TRUE;                                     // Return true , macro exist
			break;                                        // Break the cycle
		}
		if(macroTable.macro[i].id.id==ID_NA){              // If the macro id is NA , need  to check one more
      j++;
		}
	}
	return ret;                                       // Return 
}
bool setMallocMacro(U16 id, U32 size)
/*++

Function Description:
  This function apply the memory space for new macro id.
The address will start of every bolck ,block size will be 0x100(inter flash)
or 0x1000(external flash).
Arguments:
    id    -- The target id to creat
    size  -- Macro data size
Returns: 
    true  -- The macro creat success
    false -- The macro creat fail

--*/
{
	U16 i;
	U16 blockNum;
	bool ret=FALSE;
	if((id>0)&&(id<=0x7fff)){                                             // Macro Id range 0x0001-0x7fff
		if(!macroIsExist(id)&&macroTable.macroNumbers<MACRO_MAX_NUMBERS){  // Macro id is not exist
			for(i=0;i<MACRO_MAX_NUMBERS;i++){                                // Find the macro from start                     
				if(macroTable.macro[i].id.id==ID_NA){                             // Fill the macro macro id is NA
					blockNum=mallocBlock(size+CRC_SIZE);                         // Need more crc16 bytes(2)
					if(blockNum){                                                // Get the enough space for require size
						macroTable.macro[i].id.id=id;                                 // Set ID
            macroTable.macro[i].id.error=false;            
						macroTable.macro[i].dataBlockAddr=blockNum;                // Save the block number
						macroTable.macro[i].dataSize=size;                         // Save the data size
						macroTable.macroNumbers++;                                 // Update the macro numbers
						requestVariableUpdate(MACRO_CHANGE,i);		                 // Update the macro table
						ret=TRUE;                                                  // Return 
					}
					break;
				}
			}
		}
 }
	return ret;
}
bool getMallocMacro(U16 id,U8* pData)
/*++

Function Description:
  This function get the macro data size
Arguments:
    id     -- The target id 
    pData  -- The return size  to the pointer address
Returns: 
    true  -- The macro is exist
    false -- The macro is not exist

--*/
{
	U16 i;
	U16 j=0;
	bool ret=FALSE;
	for(i=0;i<macroTable.macroNumbers+j;i++){                  // Search the whole macro table
		if(macroTable.macro[i].id.id==id){                          // Match ID
			*(pData++)=(macroTable.macro[i].dataSize>>24)&0xff;    // Get the byte for MSB
			*(pData++)=(macroTable.macro[i].dataSize>>16)&0xff;    // Get the byte for MSB
			*(pData++)=(macroTable.macro[i].dataSize>>8)&0xff;     // Get the byte for MSB
			*(pData++)=macroTable.macro[i].dataSize&0xff;          // Get the byte for MSB  
			ret=TRUE;                        
			break;
		}
		if(macroTable.macro[i].id.id==ID_NA){                       // 
      j++;
		}
	}
	return ret;
}
U32 getMacroDataAddr(U16 id)
/*++

Function Description:
  This function get the macro id 's data address.
Will  distinguish  initer flash or exter flash.
(0x8000000) address will exter flash.
Arguments:
    id     -- The target id 
Returns: 
   0       -- Fail , macro id is not exist
   address -- The macro data's address.

--*/
{
	U16 i;
	U16 j=0;
	U32 ret=0;
	for(i=0;i<macroTable.macroNumbers+j;i++){   // Search all macro table
		if(macroTable.macro[i].id.id==id){           // Id match
			ret=macroTable.macro[i].dataBlockAddr;  // Get address
			if(ret&EXTER_BLOCK_FLAG){               // Extern flash
				ret=ret*SECTOR_SIZE;                  // Extern flash block size is 0x1000
			}else{                                  // Inter flash
				ret=ret*PAGE_SIZE;                    // Inter flash block size is 0x100
			}
			break;
		}
		if(macroTable.macro[i].id.id==ID_NA){
      j++;
		}
	}
	return ret;
}
U32 getMacroNameAddr(U16 id)
/*++

Function Description:
  This function get the macro id 's name address.
Will  distinguish  initer flash or exter flash.
(0x8000000) address will exter flash.
Arguments:
    id     -- The target id 
Returns: 
   0       -- Fail , macro id is not exist
   address -- The macro data's address.

--*/
{
	U16 i;
	U16 j=0;
	U32 ret=0;
	for(i=0;i<macroTable.macroNumbers+j;i++){   // Search all macro table
		if(macroTable.macro[i].id.id==id){           // Id match
			ret=macroTable.macro[i].nameBlockAddr;  // Get address
			if(ret&EXTER_BLOCK_FLAG){               // Extern flash
				ret=ret*SECTOR_SIZE;                  // Extern flash block size is 0x1000
			}else{                                  // Inter flash
				ret=ret*PAGE_SIZE;                    // Inter flash block size is 0x100
			}
			break;
		}
		if(macroTable.macro[i].id.id==ID_NA){
      j++;
		}
	}
	return ret;
}
U32 getMacroDataSize(U16 id)
/*++

Function Description:
  This function get the data size.
Arguments:
    id     -- The target id 
Returns: 
   0       -- Fail , macro id is not exist
   address -- The macro data's address.

--*/
{
	U16 i;
	U32 ret=0;
	U16 j=0;
	for(i=0;i<macroTable.macroNumbers+j;i++){
		if(macroTable.macro[i].id.id==id){
			ret=macroTable.macro[i].dataSize;
			break;
		}
		if(macroTable.macro[i].id.id==ID_NA){
      j++;
		}
	}
	return ret;
}
U32 getMacroNameSize(U16 id)
/*++

Function Description:
  This function get the name size.
Arguments:
    id     -- The target id 
Returns: 
   0       -- Fail , macro id is not exist
   address -- The macro name's address.

--*/
{
	U16 i;
	U16 j=0;
	U32 ret=0;
	for(i=0;i<macroTable.macroNumbers+j;i++){
		if(macroTable.macro[i].id.id==id){
			ret=macroTable.macro[i].nameSize;
			break;
		}
		if(macroTable.macro[i].id.id==ID_NA){
      j++;
		}
	}
	return ret;
}
U8 setMacroData(U16 id,U32 offset,U8*pData,U8 size)
/*++
Function Description:
  This function will set macro data , it will write to flash after getting 
0x100 bytes data.after getting all sizes of macro data, it will wirte crc in the 
end.When exter flash , it will write to device ic if device is free , or it will wait 
next cycle to recheck the device 's statue.
Arguments:
  id  -- target macro id
  offset -- the data offset of macro id
  pData  -- data address to get
  size   -- the size of these command
Returns: 
  0 --Fail ,Id is not exit
  1 --Waiting ,need to enter next loop
  2 --Succee
--*/
{
	U32 addr;
	U32 dataSize;
	
	static U32 cnt=0;
	static U32 pageIndex=0;
//	static U8 wait=0;
	static U16 crc=0xffff;
	//static bool crcWait=FALSE;
	static U8 crcLeftSize=0;
	static U32 offsetCheck=0;
	U16 flashCRCVal;
	U8 ret=0;
	if(offsetCheck==offset){                                     // Check offset	
		addr=getMacroDataAddr(id);                                 // Get macro address
		dataSize=getMacroDataSize(id);                             // Get the macro data size
		if(offset+size>dataSize){                                  // Ignored the over part
			size=dataSize-offset;
		}
		readFlash(addr+dataSize,(U8*)&flashCRCVal,2);	             // Read the crc , if the crc is not default value ,that means macro has written data ,doesn't support data writing twice
		if((addr)&&(dataSize)&&((flashCRCVal==0xffff) || (flashCRCVal==0))){             // Data ,Name in not 0 , crc is default value
			crc=crc16_parm(pData,size,crc);                          // Calculate the crc of very data
//			if(addr&EXTER_ADDR_FLAG){                                // External flash
//				addr&=~(EXTER_ADDR_FLAG);                              // Remove exter flag
//				if(wait!=WAIT_THREE){                                  // Not for writing the last crc bytes 
//					 if(wait!=WAIT_TWO){                                 // Not the final data state
//							if(size+cnt<DATA_BUFFER_SIZE){                   // Buffer is not full , keep filled buffer
//								memcpy(&protocolBuffer[cnt],pData,size);       // Copy the data to buffer
//								cnt+=size;                                     // Add the cnt 
//								ret=RETURN_SUCCESS;                            // Return success
//							}else{  // Datas is over buffer(0x100) , need to write flash first
//								if(wait==WAIT_NONE){                           // Data has been all recieve , wati device free then write
//									memcpy(&protocolBuffer[cnt],pData,DATA_BUFFER_SIZE-cnt); // Copy 
//								}
//								wait=WAIT_ONE;                                 // Wait statue one(buffer full writting wait for device free) 
//								ret=RETURN_WAIT;                               // Notify not clear the protocol flag, then will enter here next cycle
//								if(!sstIsBusy()){                              // Flash is not busy
//									sstPageProgram((addr>>8)+pageIndex,protocolBuffer,DATA_BUFFER_SIZE); // Write device
//									pageIndex++;                                 // Index page increat
//									memcpy(&protocolBuffer,pData+(DATA_BUFFER_SIZE-cnt),size-(DATA_BUFFER_SIZE-cnt)); // Copy the left data of this command size
//									cnt=size-(DATA_BUFFER_SIZE-cnt);             // New cnt for buffer index
//									wait=WAIT_NONE;                              // Data recieved over , no need wait
//									ret=RETURN_SUCCESS;                          // Return success
//							}
//						}
//				  }
//					if(offset+size>=dataSize&&(wait!=WAIT_ONE)){         // Reach the end of the data,make sure the all date befor is written or recieved to the buffer,need to wirte 
//						wait=WAIT_TWO;                                     // Wait the statue  Two, which is data is recieved over ,need to wain device free to write the lash data
//						ret=RETURN_WAIT;                                   // Notify the protocol function need process next cycle
//						if(!sstIsBusy()){                                  // Flash is not busy
//							if(cnt+CRC_SIZE<=DATA_BUFFER_SIZE){              // Add crc bytes not full yet ,can write to flash once and finish
//								memcpy(&protocolBuffer[cnt],&crc,CRC_SIZE);    // Copy
//								sstPageProgram((addr>>8)+pageIndex,protocolBuffer,cnt+CRC_SIZE);
//								cnt=0;                                         // Clear 
//								pageIndex=0;                                   // Clear
//								ret=RETURN_SUCCESS;                            // Notify success
//								wait=0;                                        // Clear
//								crc=0xffff;                                    // Reset crc
//								offsetCheck=0;                                 // Reset offset check
//								crcLeftSize=0;                                 // Clear
//							}else{   // It's over the buffer is add the crc , need to write the first block , then enter statue  wait three , wait to write the crc in next block
//								memcpy(&protocolBuffer[cnt],&crc,DATA_BUFFER_SIZE-cnt); // Copy the crc byte till full
//								sstPageProgram((addr>>8)+pageIndex,protocolBuffer,DATA_BUFFER_SIZE); // write deivce
//								pageIndex++;                                   // Update page index
//								wait=WAIT_THREE;                               // Enter wait three statues
//								ret=RETURN_WAIT;                               // Nofify protocoll function to wait next cycle
//								crcLeftSize=2-(DATA_BUFFER_SIZE-cnt);          // Update the crc left sizes   
//							}
//						}
//					}
//			 }else{ // Wait Three , update the crc byte
//				 if(!sstIsBusy()){                                     // Flash is not busy
//					 memcpy(&protocolBuffer,&crc+(DATA_BUFFER_SIZE-cnt),crcLeftSize); //Copy
//					 sstPageProgram((addr>>8)+pageIndex,protocolBuffer,crcLeftSize); //Write
//					 ret=RETURN_SUCCESS;                                 // Retuen 
//					 cnt=0;                                              // Clear
//					 pageIndex=0;                                        // Clear
//					 wait=0;                                             // Clear
//					 crcLeftSize=0;                                      // Clear
//					 crc=0xffff;                                         // Reset the crc 
//					 offsetCheck=0;                                      // Clear
//				 }
//			 }
//			}else{ // Internal flash
				if(size+cnt<DATA_BUFFER_SIZE){                         // Buffer is not full , keep filled buffer
					memcpy(&protocolBuffer[cnt],pData,size);             // COpy
					cnt+=size;                                           // Update cnt
					ret=RETURN_SUCCESS;                                  // Return 
				}else{                                                 // Buffer is full , need to write first , then copy the left data 
					memcpy(&protocolBuffer[cnt],pData,DATA_BUFFER_SIZE-cnt);     // Copy till full					
					ret=(FLASH_ProgramPage(addr+pageIndex*PAGE_SIZE,16,(U32*)protocolBuffer)== 0); // Write flash
					pageIndex++;                                         // Udpate index   
//          size -=(DATA_BUFFER_SIZE-cnt);          
					if ((size -(DATA_BUFFER_SIZE-cnt)) > DATA_BUFFER_SIZE) {
            memcpy(&protocolBuffer,pData+(DATA_BUFFER_SIZE-cnt),DATA_BUFFER_SIZE); // COpy the left size
					  ret=(FLASH_ProgramPage(addr+pageIndex*PAGE_SIZE,16,(U32*)protocolBuffer)== 0); // Write flash
					  pageIndex++; 
            memcpy(&protocolBuffer,pData+(DATA_BUFFER_SIZE-cnt+DATA_BUFFER_SIZE),size-DATA_BUFFER_SIZE-(DATA_BUFFER_SIZE-cnt)); // COpy the left size
            cnt=size-DATA_BUFFER_SIZE-(DATA_BUFFER_SIZE-cnt);             
          } else {
          memcpy(&protocolBuffer,pData+(DATA_BUFFER_SIZE-cnt),size-(DATA_BUFFER_SIZE-cnt)); // COpy the left size
					cnt=size-(DATA_BUFFER_SIZE-cnt);                     // Update the cnt index
          }
				}

				if(offset+size>=dataSize){                             // Reach the the end bytes
          __nop();
          __nop();
          __nop();
					if(cnt+CRC_SIZE<=DATA_BUFFER_SIZE){                  // Add the crc bytes (2) still not full
						memcpy(&protocolBuffer[cnt],&crc,CRC_SIZE);        // Copy 
						memset(protocolBuffer+cnt+CRC_SIZE,0,DATA_BUFFER_SIZE-cnt-CRC_SIZE); // Set the left size (to page) 0
						ret=(FLASH_ProgramPage(addr+pageIndex*PAGE_SIZE,16,(U32*)protocolBuffer) == 0);  // Write Inter flash
						pageIndex=0;                                       // Clear
						cnt=0;	                                           // Clear
//						wait=0;		                                         // CLear
						crc=0xffff;		                                     // Reset
            offsetCheck=0;						                         // Clear
					}else{   // Need another block to writ crc 
						memcpy(&protocolBuffer[cnt],&crc,DATA_BUFFER_SIZE-cnt);  // Copy 
 						FLASH_ProgramPage(addr+pageIndex*PAGE_SIZE,16,(U32*)protocolBuffer); // Write the flash 
						crcLeftSize=CRC_SIZE-(DATA_BUFFER_SIZE-cnt);             // Update the left crc size   
						pageIndex++;                                             // Update the index
						memcpy(protocolBuffer,&crc+(CRC_SIZE-crcLeftSize),CRC_SIZE-crcLeftSize); // Copy the left crc
						memset(&protocolBuffer[CRC_SIZE-crcLeftSize],0,DATA_BUFFER_SIZE-CRC_SIZE-crcLeftSize); // Set the left data 0 to the end of the page
						ret=(FLASH_ProgramPage(addr+pageIndex*PAGE_SIZE,16,(U32*)protocolBuffer)==0); /// Write flash
						pageIndex=0;                                        // Clear 
						cnt=0;	                                            // Clear
//						wait=0;	                                            // Clear
						crc=0xffff;                                         // Reset
						offsetCheck=0;                                      // Clear
					}					
				} 
//			}
		}   
 }else{              // Offset is not correct , clear all data and return error
	 pageIndex=0;                                       // Clear 
	 cnt=0;	                                            // Clear
//	 wait=0;	                                          // Clear
	 crc=0xffff;                                        // Reset
	 offsetCheck=0;                                     // Clear
	 crcLeftSize=0;                                     // Clear
	 ret=RETURN_FAIL;                                              // 
 }
  if((ret==RETURN_SUCCESS)&&(offset+size<dataSize)){   // Need to update offset
		offsetCheck+=size;
  }	
	return ret;
}
U8 setMacroName(U16 id,U16 totalSize,U16 offset,U8* pData,U8 size)
/*++
Function Description:
  This function will set macro name , it will write to flash after getting 
0x100 bytes data.after getting all sizes of macro data, it will wirte crc in the 
end.When exter flash , it will write to device ic if device is free , or it will wait 
next cycle to recheck the device 's statue.All process is the same as "setMacroData"
Arguments:
  id  -- target macro id
  offset -- the data offset of macro id
  pData  -- data address to get
  size   -- the size of these command
Returns: 
  0 --Fail ,Id is not exit
  1 --Waiting ,need to enter next loop
  2 --Succee
--*/
{
	U32 addr;
	U32 nameSize;
	U32 i;
	U16 j=0;
	U32 blockNum;
//	static U8 wait=0;
	static U32 cnt;
	static U32 pageIndex;
	static U16 crc=0xffff;
	static U8 crcLeftSize=0;
//	static U32 offsetCheck=0;

	U8 ret=0;
	if(offset+size>totalSize){    // Ignore the over part
		size=totalSize-offset;
	}
	for(i=0;i<macroTable.macroNumbers+j;i++){
		if(macroTable.macro[i].id.id==id){
			if(offset==0){          // Change name ,need to malloc another block
				blockNum=mallocBlock(totalSize+CRC_SIZE);
				if(blockNum){
          if(macroTable.macro[i].nameBlockAddr!=0){ // Already has macro name ,need to release the memory space
						if(macroTable.macro[i].nameBlockAddr&EXTER_BLOCK_FLAG){ // extern flash
							macroTable.recycleSizes+=((macroTable.macro[i].nameSize+2)/SECTOR_SIZE+1)*SECTOR_SIZE;
						}else{
							macroTable.recycleSizes+=((macroTable.macro[i].nameSize+2)/PAGE_SIZE+1)*PAGE_SIZE;
						}						
					}						
				  macroTable.macro[i].nameSize=totalSize;
				  macroTable.macro[i].nameBlockAddr=blockNum;
				  requestVariableUpdate(MACRO_CHANGE,i);		
				}else{               // Malloc faild , return 
					return ret;
				}
			}
			addr=getMacroNameAddr(id);
		  nameSize=getMacroNameSize(id);
		  crc=crc16_parm(pData,size,crc);
//      if(addr&EXTER_ADDR_FLAG){        // External flash
//				addr&=~(EXTER_ADDR_FLAG);      // Remove exter flag
//				if(wait!=WAIT_THREE){            // Not for writing the last crc bytes
//					 if(wait!=WAIT_TWO){          // Not the final data state
//							if(size+cnt<DATA_BUFFER_SIZE){ // Buffer is not full , keep filled buffer
//								memcpy(&protocolBuffer[cnt],pData,size);
//								cnt+=size;
//								ret=RETURN_SUCCESS;
//							}else{               // 
//								if(wait==WAIT_NONE){                     // Data has been all recieve , wati to write
//									memcpy(&protocolBuffer[cnt],pData,DATA_BUFFER_SIZE-cnt);
//								}
//								wait=WAIT_ONE;
//								ret=RETURN_WAIT;
//								if(!sstIsBusy()){            // Flash is not busy
//									sstPageProgram((addr>>8)+pageIndex,protocolBuffer,DATA_BUFFER_SIZE);
//									pageIndex++;
//									memcpy(&protocolBuffer,pData+(DATA_BUFFER_SIZE-cnt),size-(DATA_BUFFER_SIZE-cnt));
//									cnt=size-(DATA_BUFFER_SIZE-cnt);
//									wait=WAIT_NONE;
//									ret=RETURN_SUCCESS;
//							}
//						}
//				  }
//					if(offset+size>=nameSize&&(wait!=WAIT_ONE)){    // Reach the end of the data ,need to wirte
//						wait=WAIT_TWO;
//						ret=RETURN_WAIT;
//						if(!sstIsBusy()){            // Flash is not busy
//							if(cnt+CRC_SIZE<=DATA_BUFFER_SIZE){                   // Add crc size and not full yet
//								memcpy(&protocolBuffer[cnt],&crc,CRC_SIZE);
//								sstPageProgram((addr>>8)+pageIndex,protocolBuffer,cnt+CRC_SIZE);
//								wait=0;
//								cnt=0;
//								pageIndex=0;
//								ret=RETURN_SUCCESS;
//								wait=0;
//								crc=0xffff;
////								offsetCheck=0;
//							}else{               // Add crc over one byte ,need another block
//								memcpy(&protocolBuffer[cnt],&crc,DATA_BUFFER_SIZE-cnt);
//								sstPageProgram((addr>>8)+pageIndex,protocolBuffer,DATA_BUFFER_SIZE);
//								pageIndex++;
//								wait=0;
//								wait=WAIT_THREE;
//								ret=RETURN_WAIT;
//								crcLeftSize=2-(DATA_BUFFER_SIZE-cnt);
//							}
//						}
//					}
//			 }else{                          // Write the left crc byte
//				 if(!sstIsBusy()){            // Flash is not busy
//					 memcpy(&protocolBuffer,&crc+(DATA_BUFFER_SIZE-cnt),crcLeftSize);
//					 sstPageProgram((addr>>8)+pageIndex,protocolBuffer,crcLeftSize);
//					 ret=RETURN_SUCCESS;
//					 cnt=0;
//					 pageIndex=0;
//					 wait=0;
//					 wait=0;
//					 crcLeftSize=0;
//					 crc=0xffff;
////					 offsetCheck=0;
//				 }
//			 }
//			}else{                           // Innternal flash
				if(size+cnt<DATA_BUFFER_SIZE){ // Buffer is not full , keep filled buffer
					memcpy(&protocolBuffer[cnt],pData,size);
					cnt+=size;
					ret=RETURN_SUCCESS;
				}else{
					memcpy(&protocolBuffer[cnt],pData,DATA_BUFFER_SIZE-cnt);					
					ret=(FLASH_ProgramPage(addr+pageIndex*PAGE_SIZE,16,(U32*)protocolBuffer)==0);
					pageIndex++;
					memcpy(&protocolBuffer,pData+(DATA_BUFFER_SIZE-cnt),size-(DATA_BUFFER_SIZE-cnt));
					cnt=size-(DATA_BUFFER_SIZE-cnt);
				}
				
				if(offset+size>=nameSize){
					if(cnt+CRC_SIZE<=DATA_BUFFER_SIZE){
						memcpy(&protocolBuffer[cnt],&crc,CRC_SIZE);
						memset(protocolBuffer+cnt+CRC_SIZE,0,DATA_BUFFER_SIZE-cnt-CRC_SIZE);
						ret=(FLASH_ProgramPage(addr+pageIndex*PAGE_SIZE,16,(U32*)protocolBuffer)==0);
						pageIndex=0;
						cnt=0;	
//						wait=0;		
						crc=0xffff;		
//            offsetCheck=0;						
					}else{           // Need another block to writ crc 
						memcpy(&protocolBuffer[cnt],&crc,DATA_BUFFER_SIZE-cnt);
						FLASH_ProgramPage(addr+pageIndex*PAGE_SIZE,16,(U32*)protocolBuffer);
						crcLeftSize=CRC_SIZE-(DATA_BUFFER_SIZE-cnt);
						pageIndex++;
						memcpy(protocolBuffer,&crc+(CRC_SIZE-crcLeftSize),CRC_SIZE-crcLeftSize);
						memset(&protocolBuffer[CRC_SIZE-crcLeftSize],0,DATA_BUFFER_SIZE-CRC_SIZE-crcLeftSize);
						ret=(FLASH_ProgramPage(addr+pageIndex*PAGE_SIZE,16,(U32*)protocolBuffer) ==0);
						pageIndex=0;
						cnt=0;	
//						wait=0;	
						crc=0xffff;
//						offsetCheck=0;
					}					
				}
//			}
			break;
		}
		if(macroTable.macro[i].id.id==ID_NA){
      j++;
		}
	}
	return ret;
}
bool getMacroData(U16 id,U32 offset,U8* pData,U8 size)
/*++
Function Description:
  This function get the macro data will offset.
Arguments:
  id  -- target macro id
  offset -- the data offset of macro id
  pData  -- data address to get
  size   -- the size of these command
Returns: 
  0 --Fail ,Id is not exit
--*/
{
	U32 addr,dataSize;
	bool ret=FALSE;
	addr=getMacroDataAddr(id);       // Get the data address
	dataSize=getMacroDataSize(id);   //¡¡Get the data size
	if((addr)&&(dataSize)){          // addr and size is not  0
		if(offset+size<=dataSize){     // In the legal sizes
			readFlash(addr+offset,pData,size); // Read
		}else if(dataSize>offset){          // Over the total size
		  readFlash(addr+offset,pData,dataSize-offset); //Return the data to the end
		}
		ret=TRUE;
	}
	return ret;
}
bool getMacroName(U16 id,U8* totalSize,U32 offset,U8* pData,U8 size)
	/*++
Function Description:
  This function get the macro name will offset.
Arguments:
  id  -- target macro id
  offset -- the name offset of macro id
  pData  -- name address to get
  size   -- the size of these command
Returns: 
  0 --Fail ,Id is not exit
--*/
{
	U32 addr,dataSize;
	bool ret=FALSE;
	addr=getMacroNameAddr(id);                     // Get the data address
	dataSize=getMacroNameSize(id);                 //¡¡Get the data size
	*(totalSize++)=(dataSize>>8)&0xff;             // Return the total size
	*totalSize=dataSize&0xff;                      // Return the total size
	if((addr)&&(dataSize)){                         // addr and size is not  0
		if(offset+size<=dataSize){                    // In the legal sizes
			readFlash(addr+offset,pData,size);
		}else if(dataSize>offset){
			readFlash(addr+offset,pData,dataSize-offset); //Return the data to the end
		}
		ret=TRUE;
	}
	return ret;
}
void GetRamDataDump(U32 addr,U8* pData,U8 size)
/*++
Function Description:
  This function get the flash of address
Arguments:
  addr  -- address of flash
  pData  -- return data pointer
  size   -- the size of the requrite size
Returns: 
  NONE
--*/	
{
	if(size<0x5A){     // Size need to small than 90 bytes 
	 readFlash(addr,pData,size);
	}
}
U8 macroDelete(U16 id)
/*++
Function Description:
  This function delete the macro , do nothing of the flash if is inter flash.
If is the extern flash ,need to erase the flash .
Arguments:
  id  -- The target the id
Returns: 
  0 --Fail ,Id is not exit
  1 --Waiting ,need to enter next loop
  2 --Succee
--*/	
{
	U16 i;
	U16 j=0;
	U8 ret=RETURN_FAIL;
	for(i=0;i<(macroTable.macroNumbers+j)&&(i<MAX_MACRO);i++){
	if(macroTable.macro[i].id.id==id){                           // Fill the macro to table
		if(macroTable.macro[i].dataBlockAddr&EXTER_BLOCK_FLAG){ // External Flash, need to erase block
			if(exterFlashErase.dataEraseFlag!=1){                 // The previous erase is done
			  exterFlashErase.dataBlockStart=macroTable.macro[i].dataBlockAddr&(~EXTER_BLOCK_FLAG); // Get the  start address will be erase
			  exterFlashErase.dataBlockNumbers=(macroTable.macro[i].dataSize+CRC_SIZE)/SECTOR_SIZE+1; // Get the numbers of the blocks to erase			 
			  exterFlashErase.dataEraseFlag=1;                                                        // Erase flash set
			  macroTable.recycleSizes+=(((macroTable.macro[i].dataSize+2)/SECTOR_SIZE+1)*SECTOR_SIZE);// recycle size calc  
				ret=RETURN_SUCCESS;           
			}else{                                                // The previous erase is not finish
				ret=RETURN_WAIT;                                    // Erase is busy ,Need to wait next cycle
			}
		}else{                                                  // Inter flash , Just calculate the cycle size
			if(macroTable.macro[i].dataBlockAddr!=0){             
			  macroTable.recycleSizes+=(((macroTable.macro[i].dataSize+2)/PAGE_SIZE+1)*PAGE_SIZE);
			}
			ret=RETURN_SUCCESS;
		}
		if(ret!=RETURN_WAIT){                                     // If the previous is wait ,need to wait next cycle
	  	if(macroTable.macro[i].nameBlockAddr&EXTER_BLOCK_FLAG){   // Extern flash
			  if(exterFlashErase.nameEraseFlag!=1){                   // The previous erase is done
			    exterFlashErase.nameBlockStart=macroTable.macro[i].nameBlockAddr&(~EXTER_BLOCK_FLAG);
			    exterFlashErase.nameBlockNumbers=(macroTable.macro[i].nameSize+CRC_SIZE)/SECTOR_SIZE+1;		
			    exterFlashErase.nameEraseFlag=1;
			    macroTable.recycleSizes+=(((macroTable.macro[i].nameSize+2)/SECTOR_SIZE+1)*SECTOR_SIZE);
				  ret=RETURN_SUCCESS;
			  }else{
				  ret=RETURN_WAIT;                                      // Erase is busy ,Need to wait next cycle
			  }
		  }else{                                                    // Inter flash ,Just calculate the cycle size
			  if(macroTable.macro[i].nameBlockAddr!=0){
			    macroTable.recycleSizes+=(((macroTable.macro[i].nameSize+2)/PAGE_SIZE+1)*PAGE_SIZE);
			  }
			  ret=RETURN_SUCCESS;
		  }
	  }
		if(ret==RETURN_SUCCESS){                                   // Delete finish , update he macro table
		  macroTable.macro[i].id.id =ID_NA;
		  macroTable.macro[i].dataBlockAddr=0;
	    macroTable.macro[i].dataSize=0;
		  macroTable.macro[i].nameBlockAddr=0;
		  macroTable.macro[i].nameSize=0;
		  macroTable.macroNumbers--;
			requestVariableUpdate(MACRO_CHANGE,i);	
		}
		break;
	}
	if(macroTable.macro[i].id.id==ID_NA){
		j++;
	}
 }
	return ret;
}

void setMacroMinDelay(U8 delay)
{
	macroTable.macroMiniDelay=delay;
}
U8 getMacroMinDelay(void)
{
	return macroTable.macroMiniDelay;
}
void getMacroList(U16 offset,U8* totalSize,U8 *pData,U8 size)
	/*++
Function Description:
  This function get the macro list and the total size
Arguments:
  offset  -- the macro offset
  totalSize -- macro numbers
  pData     -- Data to return back
  size      -- Number sizes 
Returns: 
 NONE
--*/
{
	U16 i;
	U16 j=0;
	*(totalSize++)=(macroTable.macroNumbers>>8)&0xff;   // Macro numbers
	*totalSize=macroTable.macroNumbers&0xff;            // Macro numbers
	for(i=offset;(i<macroTable.macroNumbers+j)&&(i<offset+size);i++){ // Return every macro id
		if(macroTable.macro[i].id.id!=ID_NA){
		*(pData++)=(macroTable.macro[i].id.id>>8&0xff);
		*(pData++)=(macroTable.macro[i].id.id&0xff);
		}else{
			j++;
		}
	}
}
U16 getMacroNumber(void)
{
	return macroTable.macroNumbers;
}
void earseExFlash(void)
	/*++
Function Description:
  This function erase the extern flash.It will erase data first , then to name.
Function is process in main loop the check device free or not every cycle.
Arguments:
  NONE
Returns: 
 NONE
--*/
{
//	static U16 dataEraseCnt=0;                                       // Count the blocks
//	static U16 nameEraseCnt=0;                                       //Count the blocks
//	if(exterFlashConnect){                                           // If exter flash connected
//		if(exterFlashErase.dataEraseFlag){                             // Need to erase the data block
//			if(!sstIsBusy()){                                            // Device is free
//			  if(dataEraseCnt<exterFlashErase.dataBlockNumbers){		     // If the need to erase blocks is not finish 
//					sstEraseSector((exterFlashErase.dataBlockStart+dataEraseCnt)<<12); // Erase the sector
//					dataEraseCnt++;                                          // Update the count
//			   }else{                                                    // If the blocks need to erase is done
//				  setExterMemoryFree(exterFlashErase.dataBlockStart-EXTER_BASIC_BLOCK,exterFlashErase.dataBlockNumbers); // Set the exter bit map to the free statues
//				  exterFlashErase.dataBlockNumbers=0;                      // Update            
//				  exterFlashErase.dataBlockStart=0;                        // Update
//				  exterFlashErase.dataEraseFlag=0;                         // Update
//				  dataEraseCnt=0;                                          // Update 
//				  requestVariableUpdate(MACRO_CHANGE,NULL);                     // Update the variable
//			  }
//		  }
//		}
//		if(exterFlashErase.nameEraseFlag){                              // Need to erase the name block
//			if(!sstIsBusy()){                                             // Device is free
//			  if(nameEraseCnt<exterFlashErase.nameBlockNumbers){	 			  // If the need to erase blocks is not finish  
//					sstEraseSector((exterFlashErase.nameBlockStart+nameEraseCnt)<<12); // Erase
//					nameEraseCnt++;				                                    // Update the count
//			  }else{                                                      // If the blocks need to erase is done  
//				  setExterMemoryFree(exterFlashErase.nameBlockStart-EXTER_BASIC_BLOCK,exterFlashErase.nameBlockNumbers);
//				  exterFlashErase.nameBlockNumbers=0;                       // Update            
//				  exterFlashErase.nameBlockStart=0;                         // Update
//				  exterFlashErase.nameEraseFlag=0;                          // Update
//				  nameEraseCnt=0;                                           // Update 
//				  requestVariableUpdate(MACRO_CHANGE,NULL);                      // Update the variableble
//			  }
//			}
//		}
// }
}
bool checkMacroCRC(U16 id)
		/*++
Function Description:
  This function check the macro crc , will read the data out and calculate the crc ,
then compare with the crc bytes behide the data , if the crc is the same ,the return 
success , or return fail
Arguments:
  id   -- Target id to check crc
Returns: 
 1  -- success
 0  -- fail
--*/
{
	U16 crc_cal=0xffff;
	U16 crc;
  U16 i,k=0;
//	U16 i,j,k=0;
//	U8 buffer[256];
//	U32 addr;
	bool ret=FALSE;
	for(i=0;i<macroTable.macroNumbers+k;i++){                    // Search macro table
    if(macroTable.macro[i].id.id==id){                            // ID match		
      if(macroTable.macro[i].dataBlockAddr!=0){
//        if(macroTable.macro[i].dataBlockAddr&EXTER_BLOCK_FLAG){   // Exter flash .check data
//          addr=(macroTable.macro[i].dataBlockAddr&(~EXTER_BLOCK_FLAG))<<12; // Get the address
//          for(j=0;j<(macroTable.macro[i].dataSize>>8);j++){       // Read data every 256 bytes
//            sstRead(addr+(j<<8),buffer,256);                      // Read 
//            crc_cal=crc16_parm(buffer,256,crc_cal);               // Calculate crc 
//          }
//          sstRead(addr+(j<<8),buffer,macroTable.macro[i].dataSize%256);  // Read the rest data which is not enough 256 bytes
//          crc_cal=crc16_parm(buffer,macroTable.macro[i].dataSize%256,crc_cal); // Calculate crc
//          sstRead(addr+(j<<8)+macroTable.macro[i].dataSize%256,(U8*)&crc,2);  // Read the last crc bytes
//          if(crc_cal==crc){                                       // If crc match
//            ret=TRUE;                                             // Return success
//          }
//        }else{                                                    // Internal flash 
          crc_cal=crc16((U8*)(macroTable.macro[i].dataBlockAddr<<6),macroTable.macro[i].dataSize);  // Calculate the crc 
          memcpy((U8*)&crc,(U8*)((macroTable.macro[i].dataBlockAddr<<6)+macroTable.macro[i].dataSize),2); // Read the crc bytes 
          //crc=*((U16*)((macroTable.macro[i].dataBlockAddr<<8)+macroTable.macro[i].dataSize));
          if(crc_cal==crc){                                       // Match
            ret=TRUE;                                             // Return success
          }
//        }
      }
			if(ret){         // Check name
        if(macroTable.macro[i].nameBlockAddr!=0){
//          if(macroTable.macro[i].nameBlockAddr&EXTER_BLOCK_FLAG){   // Exter flash .check data
//            addr=(macroTable.macro[i].nameBlockAddr&(~EXTER_BLOCK_FLAG))<<12;
//            for(j=0;j<(macroTable.macro[i].nameSize>>8);j++){
//              sstRead(addr+(j<<8),buffer,256);
//              crc_cal=crc16_parm(buffer,256,crc_cal);
//            }
//            sstRead(addr+(j<<8),buffer,macroTable.macro[i].nameSize%256);
//            crc_cal=crc16_parm(buffer,macroTable.macro[i].nameSize%256,crc_cal);
//            sstRead(addr+(j<<8)+macroTable.macro[i].nameSize%256,(U8*)&crc,2);
//            if(crc_cal==crc){
//              ret=TRUE;
//            }else{
//              ret=FALSE;
//            }
//          }else{  // Internal flash 
            crc_cal=crc16((U8*)(macroTable.macro[i].nameBlockAddr<<6),macroTable.macro[i].nameSize);
            memcpy((U8*)&crc,(U8*)((macroTable.macro[i].nameBlockAddr<<6)+macroTable.macro[i].nameSize),2);
            //crc=*((U16*)((macroTable.macro[i].dataBlockAddr<<8)+macroTable.macro[i].dataSize));
            if(crc_cal==crc){
              ret=TRUE;
            }else{
              ret=FALSE;
            }
//          }
        }
			}
			
			break;
		}
	  if(macroTable.macro[i].id.id==ID_NA){
		 k++;
	  }			
	}
	return ret;
}

void checkMacroPowerUp(void)
	/*++
Function Description:
  This function check all macros crc , when error come out ,
Macro id 's bit 15 will set "1", which will be check when macro 
running , if is 1, will not run the macro.
Arguments:
  NONE
Returns: 
  NONE
--*/
{
	U16 i;
	U16 j=0;
	for(i=0;(i<macroTable.macroNumbers+j);i++){ // Return every macro id
		if(macroTable.macro[i].id.id!=ID_NA){
			if(!checkMacroCRC(macroTable.macro[i].id.id)){  // Macro Data error
				macroTable.macro[i].id.error=true;
				requestVariableUpdate(MACRO_CHANGE,i);                      // Update the variableble
			}
		}else{
			j++;
		}
	}
}
bool checkDataError(U16 id)
{
	U16 i;
	U16 j=0;
	bool ret=false;
	for(i=0;(i<macroTable.macroNumbers+j);i++){ // Return every macro id
		if(macroTable.macro[i].id.id==id){
			if(macroTable.macro[i].id.error==true){  // Macro Data error
				ret=true;
			}
			break;
		}
		if(macroTable.macro[i].id.id==ID_NA){
			j++;
		}
	}
	return ret;
}
void checkMacroDataCRC(void)
	/*++
Function Description:
  This function check all macros crc , it will be process in main loop and 
check one macro every cycle.if one macro fail , stop and return the fail macro 
id.
Arguments:
  NONE
Returns: 
  NONE
--*/
{
	static U16 cnt=0;
	static U16 cnt_macro=0;
	if(macroMemoryCheck==1){                            // Need to check crc 
		if(++cnt_macro<=macroTable.macroNumbers){         // Check all macros         
	    while((macroTable.macro[cnt].id.id==ID_NA)&&(cnt<MACRO_MAX_NUMBERS))cnt++;		 // Ignor the na id
			if(!checkMacroCRC(macroTable.macro[cnt].id.id)){   // Error
				erroMacroId=macroTable.macro[cnt].id.id;         // Error id
			  macroMemoryCheck=0;                            // Stop
				memoryManageStaue=FLASH_ERROR;                // upddate statues
		  }	
    }else{                                           // Finish
			macroMemoryCheck=0;                            //Update
			erroMacroId=0;                                  //Update
			cnt_macro=0;                                    //Update
			cnt=0;                                          //Update
			memoryManageStaue=FLASH_COMPLETED;              //Update  
		}
  }
}
void macroDataErro(U16 id)
{
	U16 i;
	U16 j=0;
	for(i=0;i<macroTable.macroNumbers+j;i++){         // Check every macro table 
		if(macroTable.macro[i].id.id==id){                 // Id match
			macroTable.macro[i].id.error=true;
			requestVariableUpdate(MACRO_CHANGE,i);                      // Update the variableble
			break;                                        // Break the cycle
		}
		if(macroTable.macro[i].id.id==ID_NA){              // If the macro id is NA , need  to check one more
      j++;
		}
	}
}
void macroRead(U8 idx)                       // Read the active macro to macro buffer
/*++
Function Description:
  This function get the active macro to macro buffer.
The fist time will full fill the buffer , others will 
keep fill the buffer till full unless the readcount is 
reach the total size of data.

Arguments:
  id: The macro id to get data
Returns:
  NONE
--*/
{
		U32 addr;
	  U32 dataSize;
	  U32 tmpFillSize;
	  U32 sizeBackup;
	  addr=macroActive[idx].dataAddr;                         // Remove the flag bytes
	  dataSize=macroActive[idx].dataSize;                               // Get the data size
		if(macroActive[idx].readcount<dataSize){                           // Does not get all the macro data				
			if(dataSize>MACRO_BUFFER_SIZE){                                  // Data size big then macro buffer size ,need more than one time to read
				tmpFillSize=MACRO_BUFFER_SIZE-macroActive[idx].bufferFilled;
				tmpFillSize=tmpFillSize>(dataSize-macroActive[idx].offset)?dataSize-macroActive[idx].offset:tmpFillSize;
				sizeBackup=tmpFillSize;
				if(MACRO_BUFFER_SIZE-macroActive[idx].bufferFilled<=MACRO_BUFFER_SIZE-macroActive[idx].pushIndex){                                                  // The size to fill is not over the buffer
					 readFlash(addr+macroActive[idx].offset,&macroBuffer[idx][macroActive[idx].pushIndex],tmpFillSize);   // Get the block avalible flag		
           macroActive[idx].pushIndex+=tmpFillSize;				
           macroActive[idx].offset+=tmpFillSize;					 
				}else{                                                         // The data size to fill will over the bufffer ,need to read to part
					 readFlash(addr+macroActive[idx].offset,&macroBuffer[idx][macroActive[idx].pushIndex],MACRO_BUFFER_SIZE-macroActive[idx].pushIndex);     // Fill the buffer to end
					 macroActive[idx].offset+=MACRO_BUFFER_SIZE-macroActive[idx].pushIndex;	                                                             // Update offset
					 tmpFillSize=tmpFillSize-(MACRO_BUFFER_SIZE-macroActive[idx].pushIndex);
					 readFlash(addr+macroActive[idx].offset,&macroBuffer[idx][0],tmpFillSize);     // Fill the buffer from the start
					 macroActive[idx].offset+=tmpFillSize;	                                                 // Update offset
					 macroActive[idx].pushIndex=tmpFillSize;                                                // Update the pushIndex				
				}
				macroActive[idx].readcount+=sizeBackup;	                                   // Update readcount
				macroActive[idx].bufferFilled+=sizeBackup;                                  // Update bufferfiled 				
			}else{                                                          // If the total size is small than buffer size ,finish read data by one time
				readFlash(addr+macroActive[idx].offset,&macroBuffer[idx][macroActive[idx].pushIndex],dataSize);
				macroActive[idx].pushIndex+=dataSize;
				macroActive[idx].offset+=dataSize;
				macroActive[idx].bufferFilled+=dataSize;
				macroActive[idx].readcount+=dataSize;
			}				
		}
}
U32 bufferPopIndex(U8 idx)
/*++
Function Description:
  This function is return the macroActive[idx].popIndex value ,
and make it pluse one, when reach the MACRO_BUFFER_SIZE,back to 
0;

Arguments:
  id: The index id of active macro
Returns:
  U32 the popIndex befor pluse one
--*/
{
	U32 ret=macroActive[idx].popIndex;
	if(macroActive[idx].popIndex==(MACRO_BUFFER_SIZE-1)){
		macroActive[idx].popIndex=0;
	}else{
		macroActive[idx].popIndex++;
	}
	return ret;
}
void dataToEven(U8 idx)                 // Make the macro buffer dato to even ,then put enven to usb queue
/*++
Function Description:
  This function analysis macro buffer data to even. 

Arguments:
  id: The macro id to get data
Returns:
  NONE
--*/
{
	U8 i;
	U32 tmp;
	U8 flag=0;
	if(macroActive[idx].popCount!=macroActive[idx].readcount){                          // popIndex not equal pushIndex, not finish current cycle
		switch(macroBuffer[idx][bufferPopIndex(idx)]){
			case KEY_MAKE:                             // Keyboard key make
				for(i=2;i<MAX_KEY;i++){                  // If the button value has been press, ignord
					if(hid_key.macroIn[i]==macroBuffer[idx][macroActive[idx].popIndex]){
						flag=1;
					}
				}
				for(i=2;(i<MAX_KEY)&&(flag==0);i++){
					if(hid_key.macroIn[i]==0){
						hid_key.macroIn[i]=macroBuffer[idx][macroActive[idx].popIndex];		   // Use keyboard enpoint report				
						macroEven|=MACRO_KEY_EVEN;
						break;
					}					
				}
				 bufferPopIndex(idx);          // Update popIndex			 
			   macroActive[idx].bufferFilled-=2;       //One byte for enven id ,other byte for data		
         macroActive[idx].popCount+=2;				
				break;
			case KEY_BREAK:                            // Keyboard key released
				for(i=2;i<MAX_KEY;i++){
					if(macroBuffer[idx][macroActive[idx].popIndex]==hid_key.macroIn[i]){   
						hid_key.macroIn[i]=0;                                                // Use keyboard enpoint report	
						macroEven|=MACRO_KEY_EVEN;
						break;
					}
				}
				bufferPopIndex(idx);          // Update popIndex
				macroActive[idx].bufferFilled-=2; //One byte for enven id ,other byte for data
				macroActive[idx].popCount+=2;
				macroActive[idx].stepFlag--;     // Just for macro type IV
        //[ slash add for DTS 17_108-130
//        if((macroActive[idx].type == BTN_MACRO_IV) && (macroActive[idx].popCount == macroActive[idx].readcount)) {
//	         macroActive[idx].count++;                                                 // Finish one macro display
//	         macroActive[idx].timer=0;                     // Clear timer
//	         macroActive[idx].miniDelay=getMacroMinDelay();         // Get the  macro minimum delay
//	         macroActive[idx].miniDelay=(macroActive[idx].miniDelay==0)?1:macroActive[idx].miniDelay;
//	         macroActive[idx].bufferFilled=0;               // Clear bufferFilled ,then need to refill the buffer
//	         macroActive[idx].offset=0;                     // Reset the offset
//	         macroActive[idx].popIndex=0;                   // Reset the popIndex
//	         macroActive[idx].pushIndex=0;                  // Reset the pushIndex
//	         macroActive[idx].readcount=0;                  // Reset the read count
//	         macroActive[idx].popCount=0;
//           macroActive[idx].stepFlag=0;   
//			     macroActive[idx].flag=false;
//        }  
        //]        
				break;
			case POWER_MAKE:
				Macro_Ep2_In[0]=2;//0x03;         // Media report id :0x02
			  Macro_Ep2_In[1]=macroBuffer[idx][bufferPopIndex(idx)];	
			  macroActive[idx].bufferFilled-=2; //One byte for enven id ,other byte for data
			  macroActive[idx].popCount+=2;
			  macroEven|=MACRO_POWER_EVEN;      // Update Even 
				break;
			case POWER_BREAK:
				Macro_Ep2_In[0]=2;//0x03;         // Media report id :0x02
			  Macro_Ep2_In[1]=macroBuffer[idx][bufferPopIndex(idx)];
			  macroActive[idx].bufferFilled-=2; //One byte for enven id ,other byte for data
			  macroActive[idx].popCount+=2;
			  macroEven|=MACRO_POWER_EVEN;      // Update Even 
			  macroActive[idx].stepFlag--;     // Just for macro type IV
        //[ slash add for DTS  17_108-130
//        if((macroActive[idx].type == BTN_MACRO_IV) && (macroActive[idx].popCount == macroActive[idx].readcount)) {
//	         macroActive[idx].count++;                                                 // Finish one macro display
//	         macroActive[idx].timer=0;                     // Clear timer
//	         macroActive[idx].miniDelay=getMacroMinDelay();         // Get the  macro minimum delay
//	         macroActive[idx].miniDelay=(macroActive[idx].miniDelay==0)?1:macroActive[idx].miniDelay;
//	         macroActive[idx].bufferFilled=0;               // Clear bufferFilled ,then need to refill the buffer
//	         macroActive[idx].offset=0;                     // Reset the offset
//	         macroActive[idx].popIndex=0;                   // Reset the popIndex
//	         macroActive[idx].pushIndex=0;                  // Reset the pushIndex
//	         macroActive[idx].readcount=0;                  // Reset the read count
//	         macroActive[idx].popCount=0;
//           macroActive[idx].stepFlag=0;   
//			     macroActive[idx].flag=false;
//        }
        //]        
				break;
			case MEDIA_MAKE:                    // Media make even
				Macro_Ep2_In[0]=1;//0x02;         // Media report id :0x02
			  Macro_Ep2_In[1]=macroBuffer[idx][bufferPopIndex(idx)];  // Get two bytes media hid usage id
			  Macro_Ep2_In[2]=macroBuffer[idx][bufferPopIndex(idx)];
			  macroActive[idx].bufferFilled-=3; //One byte for enven id ,other byte for data
			  macroActive[idx].popCount+=3;
			  macroEven|=MACRO_MEDIA_EVEN;      // Update Even 
				break;
			case MEDIA_BREAK:                    // Media make even
				Macro_Ep2_In[0]=1;//0x02;         // Media report id :0x02
			  Macro_Ep2_In[1]=macroBuffer[idx][bufferPopIndex(idx)]; 
			  Macro_Ep2_In[2]=macroBuffer[idx][bufferPopIndex(idx)]; 
			  macroActive[idx].bufferFilled-=3; //One byte for enven id ,other byte for data
			  macroActive[idx].popCount+=3;
			  macroEven|=MACRO_MEDIA_EVEN;      // Update Even 
			  macroActive[idx].stepFlag--;     // Just for macro type IV
        //[ slash add for DTS  17_108-130
//        if((macroActive[idx].type == BTN_MACRO_IV) && (macroActive[idx].popCount == macroActive[idx].readcount)) {
//	         macroActive[idx].count++;                                                 // Finish one macro display
//	         macroActive[idx].timer=0;                     // Clear timer
//	         macroActive[idx].miniDelay=getMacroMinDelay();         // Get the  macro minimum delay
//	         macroActive[idx].miniDelay=(macroActive[idx].miniDelay==0)?1:macroActive[idx].miniDelay;
//	         macroActive[idx].bufferFilled=0;               // Clear bufferFilled ,then need to refill the buffer
//	         macroActive[idx].offset=0;                     // Reset the offset
//	         macroActive[idx].popIndex=0;                   // Reset the popIndex
//	         macroActive[idx].pushIndex=0;                  // Reset the pushIndex
//	         macroActive[idx].readcount=0;                  // Reset the read count
//	         macroActive[idx].popCount=0;
//           macroActive[idx].stepFlag=0;   
//			     macroActive[idx].flag=false;
//        }  
        //]        
				break;
			case BUTTON:                             // Mouse Button even
				macroButton=macroBuffer[idx][bufferPopIndex(idx)];
			  macroActive[idx].bufferFilled-=2; //One byte for enven id ,other byte for data
			  macroActive[idx].popCount+=2;
			  macroEven|=MACRO_BUTTON_EVEN;      // Update Even 
			  macroActive[idx].stepFlag--;     // Just for macro type IV
        //[ slash add for DTS  17_108-130
//        if((macroActive[idx].type == BTN_MACRO_IV) && (macroActive[idx].popCount == macroActive[idx].readcount)) {
//	         macroActive[idx].count++;                                                 // Finish one macro display
//	         macroActive[idx].timer=0;                     // Clear timer
//	         macroActive[idx].miniDelay=getMacroMinDelay();         // Get the  macro minimum delay
//	         macroActive[idx].miniDelay=(macroActive[idx].miniDelay==0)?1:macroActive[idx].miniDelay;
//	         macroActive[idx].bufferFilled=0;               // Clear bufferFilled ,then need to refill the buffer
//	         macroActive[idx].offset=0;                     // Reset the offset
//	         macroActive[idx].popIndex=0;                   // Reset the popIndex
//	         macroActive[idx].pushIndex=0;                  // Reset the pushIndex
//	         macroActive[idx].readcount=0;                  // Reset the read count
//	         macroActive[idx].popCount=0;
//           macroActive[idx].stepFlag=0;   
//			     macroActive[idx].flag=false;
//        }
        //]        
				break;
			case SCROLL_WHEEL:
				macroScroll=macroBuffer[idx][bufferPopIndex(idx)];
			  macroActive[idx].bufferFilled-=2; //One byte for enven id ,other byte for data
			  macroActive[idx].popCount+=2;
			  macroEven|=MACRO_BUTTON_EVEN;      // Update Even 
			  macroActive[idx].stepFlag--;     // Just for macro type IV
				break;
			case CURSOR:
				macroXCursor=macroBuffer[idx][bufferPopIndex(idx)];
				macroXCursor=(macroXCursor<<8)|macroBuffer[idx][bufferPopIndex(idx)];
			  macroYCursor=macroBuffer[idx][bufferPopIndex(idx)];
			  macroYCursor=(macroYCursor<<8)|macroBuffer[idx][bufferPopIndex(idx)];
			  macroEven|=MACRO_BUTTON_EVEN;      // Update Even 
			  macroActive[idx].bufferFilled-=5; //One byte for enven id ,other byte for data
			  macroActive[idx].popCount+=5;
			  macroActive[idx].stepFlag--;     // Just for macro type IV
				break;
			case DELAY_1:                            // Delay one byte
				macroActive[idx].timer+=macroBuffer[idx][bufferPopIndex(idx)];   // Get the delay time                                        
			  macroActive[idx].bufferFilled-=2; //One byte for enven id ,other byte for data
			  macroActive[idx].popCount+=2;
				break;
			case DELAY_2:                            // Delay two bytes
				tmp=macroBuffer[idx][bufferPopIndex(idx)];   // Get the delay time
			  tmp=(tmp<<8)|macroBuffer[idx][bufferPopIndex(idx)];   // Get the delay time
			  macroActive[idx].timer+=tmp;                                    // Get the total delay
			  macroActive[idx].bufferFilled-=3; //One byte for enven id ,two bytes for data
			  macroActive[idx].popCount+=3;
				break;
			case DELAY_3:                            // Delay three bytes
				tmp=macroBuffer[idx][bufferPopIndex(idx)];   // Get the delay time
			  tmp=(tmp<<8)|macroBuffer[idx][bufferPopIndex(idx)];   // Get the delay time
			  tmp=(tmp<<8)|macroBuffer[idx][bufferPopIndex(idx)];   // Get the delay time
			  macroActive[idx].timer+=tmp;                                   // Get the total delay      
			  macroActive[idx].bufferFilled-=4; //One byte for enven id ,three bytes for data
			  macroActive[idx].popCount+=4;
				break;
			case DELAY_4:                            // Delay four bytes
				tmp=macroBuffer[idx][bufferPopIndex(idx)];   // Get the delay time
			  tmp=(tmp<<8)|macroBuffer[idx][bufferPopIndex(idx)];   // Get the delay time
			  tmp=(tmp<<8)|macroBuffer[idx][bufferPopIndex(idx)];   // Get the delay time
			  tmp=(tmp<<8)|macroBuffer[idx][bufferPopIndex(idx)];   // Get the delay time
			  macroActive[idx].timer+=tmp;                                   // Get the total delay
			  macroActive[idx].bufferFilled-=5; //One byte for enven id ,four bytes for data
			  macroActive[idx].popCount+=5;
				break;
			default:	 
//			   macroActive[idx].bufferFilled-=1;           //One byte for enven id ,other byte for data		
//         macroActive[idx].popCount+=1;	             // Mark , need to stop the macro , need to disscuss more
			   macroActive[idx].flag=false;                  // Error found , stop running
			   macroDataErro(macroActive[idx].id);            // Macro is error, mark it and will not run next time
			   memset(hid_key.macroIn,0,sizeof(hid_key.macroIn));
	       macroEven|=MACRO_KEY_EVEN;                     // Release all macro key
         macroButton=0;
	       macroEven|=MACRO_BUTTON_EVEN;                  // Release all macro button
			   
				break;
		}
	}else{ // Finish one cycle ,start other cycle if needed
		 macroActive[idx].count++;                                                 // Finish one macro display
		 macroActive[idx].timer=0;                     // Clear timer
		 macroActive[idx].miniDelay=getMacroMinDelay();         // Get the  macro minimum delay
		 macroActive[idx].miniDelay=(macroActive[idx].miniDelay==0)?1:macroActive[idx].miniDelay;
		 macroActive[idx].bufferFilled=0;               // Clear bufferFilled ,then need to refill the buffer
		 macroActive[idx].offset=0;                     // Reset the offset
		 macroActive[idx].popIndex=0;                   // Reset the popIndex
		 macroActive[idx].pushIndex=0;                  // Reset the pushIndex
		 macroActive[idx].readcount=0;                  // Reset the read count
		 macroActive[idx].popCount=0;
//		if(macroActive[idx].type == BTN_MACRO_IV){      // Just for macro type IV
//			macroActive[idx].stepFlag=0;   
//			macroActive[idx].flag=false;
//		}	                 
	}
}
void macroProcess(void)                               // Process the macro function
	/*++
Function Description:
  This function main dear with the macro include read macro buffer,
analysis data to even,control the repeatimes times in difference type

Arguments:
  NONE
Returns:
  NONE
--*/
{
	U8 i;
	for(i=0;i<MAX_ACTIVE_MACRO;i++){
		if(macroActive[i].flag==TRUE){                    // If this macro is running
		switch(macroActive[i].type){                    // Differ the macro type
				case BTN_MACRO_I:                             // Repeat times is control by 
					if(macroActive[i].count<macroActive[i].repeatTimes){
						if(macroActive[i].timer==0){ // Waiting for delays
							macroActive[i].timer=macroActive[i].miniDelay;                       // Reset timer
						  macroRead(i);                                 // Read macro data to macroBuffer
						  dataToEven(i);                                // Makes macroBuffer to even 							
						}
					}else{
						macroActive[i].flag=FALSE;
					}
         break;		
        case BTN_MACRO_II:                                // Active macro till key released
				case BTN_MACRO_III:                               // Active macro till press the key again
					if(macroActive[i].timer==0){                    // Waiting for delays
						macroActive[i].timer=macroActive[i].miniDelay;// Reset timer
						macroRead(i);                                 // Read macro data to macroBuffer
						dataToEven(i);                                // Makes macroBuffer to even 						 
					}
					if((macroActive[i].repeatTimes==0)&&(macroActive[i].offset==0)){  // In macro II,III, repeatTimes=0 means stop macro, but need to wait macro display finish
						macroActive[i].flag=FALSE;
					}
          break;	
//				case BTN_MACRO_IV:                                 // Type IV , every time button make , come out an upload even
//        if(macroActive[i].timer==0){                    // Waiting for delays
//				  macroActive[i].timer=macroActive[i].miniDelay;// Reset timer
//				  if(macroActive[i].stepFlag!=0){                      // Go 
//					  macroRead(i);                                 // Read macro data to macroBuffer
//						dataToEven(i);                                // Makes macroBuffer to even 	
//				  }
//			  }
//				break;				
		  }
	  }
	}
}
void activeMacro(U8 type,U8 btnIdx,U16 id,U8 times)
/*++
Function Description:
  This function pass the active macro to active buffer, include 
macro type ,macro id ,repeate times,data address flag,data address,
data size.

Arguments:
  type:  macro type
  id:    macro id
  times: macro repeat times
Returns:
  0xff: Fail , reach for the max active macro numbers
  others: active index
--*/
{      
	U32 addr=0;
	if(macroIsExist(id)){                           // If macro is exit
	  if(!checkDataError(id)){                  // IF the data is not error
		//if(macroActive[btnIdx].flag==FALSE){         // It is not active now		  
			macroActive[btnIdx].id=id;				          // Active id
			macroActive[btnIdx].repeatTimes=times;     // Active times,just for macro i
			macroActive[btnIdx].type=type;             // macro type
			macroActive[btnIdx].timer=0;
			macroActive[btnIdx].offset=0;
			macroActive[btnIdx].popIndex=0;
			macroActive[btnIdx].pushIndex=0;
			macroActive[btnIdx].readcount=0;
			macroActive[btnIdx].count=0;
			macroActive[btnIdx].miniDelay=getMacroMinDelay();   // Get the  macro minimum delay
			macroActive[btnIdx].miniDelay=(macroActive[btnIdx].miniDelay==0)?1:macroActive[btnIdx].miniDelay;
			macroActive[btnIdx].flag=TRUE;
			addr=getMacroDataAddr(id);   
			macroActive[btnIdx].dataAddr=addr;
			macroActive[btnIdx].dataSize= getMacroDataSize(id);
			macroActive[btnIdx].popCount=0;
			macroActive[btnIdx].stepFlag=1;
			}
		}
 // }
}
void macroStepGo(U8 btnIdx)
{
	macroActive[btnIdx].stepFlag++;                 // Go 
}
void resetRepeattimes(U8 btnIdx)
{
	macroActive[btnIdx].count=0;
}
void deActiveMacro(U8 idx)                    // Kill the active macro
{
	macroActive[idx].repeatTimes=0;              // Makes the macro stop 
}

U8 buttonIsRunning(U8 btnIdx)
{
  U8 ret=0;
	if(macroActive[btnIdx].flag==true){
			ret=1;
	}
	return ret;
}
void macroEngine(void)
{
	macroProcess();
	checkMacroDataCRC();
//	earseExFlash();
}
void stopHIDeven(void)
{
	U8 i;
	for(i=0;i<MAX_ACTIVE_MACRO;i++){
		macroActive[i].flag=false;
	}
	memset(hid_key.macroIn,0,sizeof(hid_key.macroIn));
	memset(hid_key.keyIn,0,sizeof(hid_key.keyIn));
	memset(hid_key.turboIn,0,sizeof(hid_key.turboIn));
	turbo.kb.stage=TURBO_NONE;
	turbo.btn.stage=TURBO_NONE;
	macroEven|=MACRO_KEY_EVEN;
  turbo.btn.buttonValue=macroButton=currentButtons=doubleClickButton=0;
	macroEven|=MACRO_BUTTON_EVEN;                 // Update Even 
}
