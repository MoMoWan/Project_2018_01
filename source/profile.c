#include "profile.h"
#include "led_driver.h"
#include "button.h"
//#include "sst26vf016b.h"
#include "flash.h"
#include "avago_api.h"
#include "FlashPrg.h"

bool  newProfile(U8 id)
{
	bool ret=FALSE;
//	U16 nameSize;
//	if((0<id)&&(id<=profileNO)){        // Id range from 1 -5
//	  if((user.aliveProfile&(1<<(id)))==0){ // profile id is not exist
//		  user.aliveProfile|=(1<<(id));
//		  ret=TRUE;
//	  }
//   }
//	if(id==1){                                // Special deal ,because the profile 1 will not be deleted , so if profile name is set , need to release space here 
//		if(user.profile[id-1].nameAdd!=0){                   // Has the profile name , need to release the space to recycleable 
//			//readFlash((U32)(user.profile[id-1].nameAdd),(U8*)&nameSize,2);	   // Get the profile size
//			nameSize=user.profile[id-1].nameSize+CRC_SIZE;               // add the crc bytes and the size bytes
//			if(user.profile[id-1].nameAdd&EXTER_ADDR_FLAG){     // Extern flash
//				macroTable.recycleSizes+=(nameSize/SECTOR_SIZE+1)*SECTOR_SIZE;
//			}else{
//				macroTable.recycleSizes+=(nameSize/PAGE_SIZE+1)*PAGE_SIZE;
//			}	
//			requestVariableUpdate(MACRO_CHANGE,0);	
//		}
//	}
//	if(ret==TRUE||id==1){             // Need to back to default setting
//		user.profile[id-1]=dft_generic2.profile[id-1];
//		requestVariableUpdate(SW_CHANGEDPr,id-1);              // Generic variable storage needs updating	
//		ret=TRUE;
//	}
	return ret;
}
void loadProfileLed(void)
{
	   CHROMA_PARM *led=ledParameters(1, lighting[0].id);
		switch (user.PClass) {
			case 1:		                                          // Profile 1 Red
       ledPwmData[LED_PROFILE].r= 0xff;//(0xff*((U32)brightnesstable[led[0].level/4])+0xff)>>8;
			 ledPwmData[LED_PROFILE].g= 0xff;//(0xff*((U32)brightnesstable[led[0].level/4])+0xff)>>8;
			 ledPwmData[LED_PROFILE].b= 0xff;//(0xff*((U32)brightnesstable[led[0].level/4])+0xff)>>8;
			break;
			case 2:                                            // Profile 2 Yellow
       ledPwmData[LED_PROFILE].r= 0xff;//(0xff*((U32)brightnesstable[led[0].level/4])+0xff)>>8;
			 ledPwmData[LED_PROFILE].g= 0;
			 ledPwmData[LED_PROFILE].b= 0;
			break;
			case 3:                                            // Profile 3 Green
       ledPwmData[LED_PROFILE].r= 0;
			 ledPwmData[LED_PROFILE].g= 0xff;//(0xff*((U32)brightnesstable[led[0].level/4])+0xff)>>8;
			 ledPwmData[LED_PROFILE].b= 0;
			break;
			case 4:                                            // Profile 4 Cyan
       ledPwmData[LED_PROFILE].r= 0;
			 ledPwmData[LED_PROFILE].g= 0;
			 ledPwmData[LED_PROFILE].b= 0xff;//(0xff*((U32)brightnesstable[led[0].level/4])+0xff)>>8;
			break;
			case 5:                                            // Profile 5 Blue
       ledPwmData[LED_PROFILE].r= 0;
			 ledPwmData[LED_PROFILE].g= 0xff;//(0xff*((U32)brightnesstable[led[0].level/4])+0xff)>>8;
			 ledPwmData[LED_PROFILE].b= 0xff;//(0xff*((U32)brightnesstable[led[0].level/4])+0xff)>>8;
			break;
			case 6:                                            // Profile 5 Blue
       ledPwmData[LED_PROFILE].r= 0xff;//(0xff*((U32)brightnesstable[led[0].level/4])+0xff)>>8;
			 ledPwmData[LED_PROFILE].g= 0;
			 ledPwmData[LED_PROFILE].b= 0xff;//(0xff*((U32)brightnesstable[led[0].level/4])+0xff)>>8;
			break;
		}
}
bool deleteProfile(U8 id)
{
	bool ret=FALSE;
//	U16 nameSize;
//	if(((user.aliveProfile&(1<<(id)))==(1<<id))&&(id!=1)){ // profile id is not exist , profile 1 can not be delete
//		user.aliveProfile&=~(1<<(id));
//		if(user.profile[id-1].nameAdd!=0){                   // Has the profile name , need to release the space to recycleable 
//			//readFlash((U32)(user.profile[id-1].nameAdd),(U8*)&nameSize,2);	   // Get the profile size
//			nameSize=user.profile[id-1].nameSize+CRC_SIZE;               // add the crc bytes and the size bytes
//			if(user.profile[id-1].nameAdd&EXTER_ADDR_FLAG){     // Extern flash
//				macroTable.recycleSizes+=(nameSize/SECTOR_SIZE+1)*SECTOR_SIZE;
//			}else{
//				macroTable.recycleSizes+=(nameSize/PAGE_SIZE+1)*PAGE_SIZE;
//			}	
//			requestVariableUpdate(MACRO_CHANGE,0);	
//		}
//		ret=TRUE;
//	}
	return ret;
}
U8 getProfileNumber(void)
{
	U8 i;
	U8 cnt=0;
	for(i=0;i<8;i++){
		if((user.aliveProfile>>i)&0x01){
			cnt++;
		}
	}
	return cnt;
}

bool switchProfile(U8 type,U8 num)
{
//	U8 i;
	bool ret=FALSE;
//	switch (type){
//		case PROFILE_UP:
//	    for(i=user.PClass+1;;i++){
//				i=i==8?1:i;
//				if(i==(user.PClass)){
//					break;
//				}
//		    if((user.aliveProfile>>i)&0x01){
//					user.PClass=i;
//					user.ProNO=user.PClass-1;
//          for(i=0;i<NUMBER_OF_LEDS;i++){
//            user.profile[user.ProNO].savedLight[i].index=i;
//          }
//          for(i=0;i<NUMBER_OF_LEDS;i++){
//            updateProfileLed(&user.profile[user.ProNO].savedLight[i],user.PClass);
//            copyLighting2Active(&user.profile[user.ProNO].savedLight[i]);       // copy ram to light	           
//          }
//          loadProfileLed();   // Load the profile indicate led
//					//forceLightingReload();
//					copyLiftoff2Active(0x00);                                     // Copy the lift value to active function
//				//	setResolution(user.profile[user.ProNO].activeDpi.x,user.profile[user.ProNO].activeDpi.y);
//					setResolution(dpiCurrent.x,dpiCurrent.y);
//					stopHIDeven();
//					memcpy((U8*)&btnAssign,(U8*)&user.profile[user.ProNO].btnAssign,sizeof(btnAssign));  // Copy the profile assignment to btnAssign
//					ret=TRUE;
//					break;
//		     }
//	     }
//			break;
//		case PROFILE_DOWN:
//			 for(i=user.PClass-1;;i--){
//				i=i==0?7:i;
//				if(i==(user.PClass)){
//					break;
//				}
//		    if((user.aliveProfile>>i)&0x01){
//					user.PClass=i;
//					user.ProNO=user.PClass-1;
//          for(i=0;i<NUMBER_OF_LEDS;i++){
//            user.profile[user.ProNO].savedLight[i].index=i;
//          }
//          for(i=0;i<NUMBER_OF_LEDS;i++){
//            updateProfileLed(&user.profile[user.ProNO].savedLight[i],user.PClass);
//            copyLighting2Active(&user.profile[user.ProNO].savedLight[i]);       // copy ram to light	          
//          }
//          loadProfileLed();   // Load the profile indicate led
//					//forceLightingReload();
//					copyLiftoff2Active(0x00);                                     // Copy the lift value to active function
//					//setResolution(user.profile[user.ProNO].activeDpi.x,user.profile[user.ProNO].activeDpi.y);
//					setResolution(dpiCurrent.x,dpiCurrent.y);
//					stopHIDeven();
//					memcpy((U8*)&btnAssign,(U8*)&user.profile[user.ProNO].btnAssign,sizeof(btnAssign));  // Copy the profile assignment to btnAssign
//					ret=TRUE;
//					break;
//		     }
//	     }
//			break;
//		case PROFILE_NUM:
//			if((user.aliveProfile>>num)&0x01){    // is alive
//				user.PClass=num;
//				user.ProNO=user.PClass-1;
//        for(i=0;i<NUMBER_OF_LEDS;i++){
//          user.profile[user.ProNO].savedLight[i].index=i;
//        }
//        for(i=0;i<NUMBER_OF_LEDS;i++){
//          updateProfileLed(&user.profile[user.ProNO].savedLight[i],user.PClass);
//          copyLighting2Active(&user.profile[user.ProNO].savedLight[i]);       // copy ram to light	        
//        }
//        loadProfileLed();   // Load the profile indicate led
//				//forceLightingReload();
//				copyLiftoff2Active(0x00);                                     // Copy the lift value to active function
//				//setResolution(user.profile[user.ProNO].activeDpi.x,user.profile[user.ProNO].activeDpi.y);
//				setResolution(dpiCurrent.x,dpiCurrent.y);
//				stopHIDeven();
//				memcpy((U8*)&btnAssign,(U8*)&user.profile[user.ProNO].btnAssign,sizeof(btnAssign));  // Copy the profile assignment to btnAssign
//				ret=TRUE;
//			}
//			break;
//	}
	return ret;
}
U32 getProfileNameAddr(U8 id)
{
//	U8 i;
	U32 ret=0;
//	for(i=0;i<profileNO;i++){
//		if(user.profile[i].profile_ID==id/*&&(user.profile[i].nameStage==true)*/){
//			ret=user.profile[i].nameAdd;
//			break;
//		}
//	}
	return ret;
}
bool getProfileName(U8 id,U8* totalSize,U16 offset,U8* pData,U8 size)
{
//	U32 addr;
//	U16 nameSize;
	bool ret=false;
//	if(id>0&&id<=profileNO){
//    if(user.profile[id-1].nameStage==true){        // If the profile name has been setting
//      addr=user.profile[id-1].nameAdd;             // Get profile addr
//      if(addr!=0){
//       // readFlash(addr,(U8*)&nameSize,2);	   // Get the profile size
//        nameSize=user.profile[id-1].nameSize;
//        *(totalSize++)=((nameSize>>8)&0xff);       // Hight byte
//        *(totalSize)=(nameSize&0xff);              // Hight byte
//        if(nameSize>offset){
//          if(offset+size>nameSize){
//            size=nameSize-offset;
//          }
//          readFlash((U32)(addr+offset),pData,size);	   // Get the profile size
//          ret=true;
//        }
//      }
//    }
//	}
	return ret;
}
U8 setProfileName(U8 id,U16 totalSize,U16 offset,U8* pData,U8 size)
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
//	U32 addr;
//	U32 nameSize=0;
//	U32 blockNum;
//	static U8 wait=0;
//	static U32 cnt;
//	static U32 pageIndex;
//	static U16 crc=0xffff;
//	static U8 crcLeftSize=0;
//	static U32 offsetCheck=0;

	U8 ret=0;
//	if(!(user.aliveProfile&(1<<(id)))){ // profile id is not exist ,return fail 
//		return ret;
//	}
//	if(offset+size>totalSize){  // Ignore the over part
//		size=totalSize-offset;
//	}
//	if(offset==0){          // Change name ,need to malloc another block
//		blockNum=mallocBlock(totalSize+CRC_SIZE); // +2 is for the name total size
//		if(blockNum!=0){       // Get the address is right		
//		  if(user.profile[id-1].nameAdd!=0){                   // Has the profile name , need to release the space to recycleable 
//			 // readFlash((U32)(user.profile[id-1].nameAdd),(U8*)&nameSize,2);	   // Get the profile size
//			 // nameSize+=(CRC_SIZE+2);               // add the crc bytes and the size bytes
//			  if(user.profile[id-1].nameAdd&EXTER_ADDR_FLAG){     // Extern flash
//				  macroTable.recycleSizes+=((user.profile[id-1].nameSize+2)/SECTOR_SIZE+1)*SECTOR_SIZE;
//			  }else{
//				  macroTable.recycleSizes+=((user.profile[id-1].nameSize+2)/PAGE_SIZE+1)*PAGE_SIZE;
//			  }	
//			  user.profile[id-1].nameAdd=0;
//				user.profile[id-1].nameSize=0;
//			  requestVariableUpdate(MACRO_CHANGE,0);	
//		  }
//			if(blockNum==EXTER_BLOCK_FLAG){  // 
//				user.profile[id-1].nameAdd=(blockNum*SECTOR_SIZE);
//			}else{
//				user.profile[id-1].nameAdd=(blockNum*PAGE_SIZE);
//			}
//			user.profile[id-1].nameSize=totalSize;
//      user.profile[id-1].nameCrc=0;                        // Clear the crc bytes 
//      user.profile[id-1].nameStage=false;                  // Disable the profile name 
//			requestVariableUpdate(SW_CHANGEDPr,id-1);	
//		//	protocolBuffer[0]=(totalSize&0xff);     // Low  byte
//		//	protocolBuffer[1]=((totalSize>>8)&0xff);          // Hight byte
//		//	cnt=2;
//		//	crc=crc16_parm((U8*)&totalSize,2,crc);       // Add the size crc 
//		}else{
//			return ret;                                    // Space is not enouth , return fail
//		}			
//	}
//	addr=(U32)getProfileNameAddr(id);
//	nameSize=totalSize;
//	crc=crc16_parm(pData,size,crc);
////	if(addr&EXTER_ADDR_FLAG){        // External flash
////		addr&=~(EXTER_ADDR_FLAG);      // Remove exter flag
////		if(wait!=WAIT_THREE){            // Not for writing the last crc bytes
////			 if(wait!=WAIT_TWO){          // Not the final data state
////					if(size+cnt<DATA_BUFFER_SIZE){ // Buffer is not full , keep filled buffer
////						memcpy(&protocolBuffer[cnt],pData,size);
////						cnt+=size;
////						ret=RETURN_SUCCESS;
////					}else{               // 
////						if(wait==WAIT_NONE){                     // Data has been all recieve , wati to write
////							memcpy(&protocolBuffer[cnt],pData,DATA_BUFFER_SIZE-cnt);
////						}
////						wait=WAIT_ONE;
////						ret=RETURN_WAIT;
////						if(!sstIsBusy()){            // Flash is not busy
////							sstPageProgram((addr>>8)+pageIndex,protocolBuffer,DATA_BUFFER_SIZE);
////							pageIndex++;
////							memcpy(&protocolBuffer,pData+(DATA_BUFFER_SIZE-cnt),size-(DATA_BUFFER_SIZE-cnt));
////							cnt=size-(DATA_BUFFER_SIZE-cnt);
////							wait=WAIT_NONE;
////							ret=RETURN_SUCCESS;
////					}
////				}
////			}
////			if(offset+size>=nameSize&&(wait!=WAIT_ONE)){    // Reach the end of the data ,need to wirte
////				wait=WAIT_TWO;
////				ret=RETURN_WAIT;
////				if(!sstIsBusy()){            // Flash is not busy
////					if(cnt+CRC_SIZE<=DATA_BUFFER_SIZE){                   // Add crc size and not full yet
////						memcpy(&protocolBuffer[cnt],&crc,CRC_SIZE);
////						sstPageProgram((addr>>8)+pageIndex,protocolBuffer,cnt+CRC_SIZE);
////						wait=0;
////						cnt=0;
////						pageIndex=0;
////						ret=RETURN_SUCCESS;
////						wait=0;
////            user.profile[id-1].nameCrc=crc;
////            user.profile[id-1].nameStage=true;
////            requestVariableUpdate(SW_CHANGEDPr,id-1);	
////						crc=0xffff;
//////								offsetCheck=0;
////					}else{               // Add crc over one byte ,need another block
////						memcpy(&protocolBuffer[cnt],&crc,DATA_BUFFER_SIZE-cnt);
////						sstPageProgram((addr>>8)+pageIndex,protocolBuffer,DATA_BUFFER_SIZE);
////						pageIndex++;
////						wait=0;
////						wait=WAIT_THREE;
////						ret=RETURN_WAIT;
////						crcLeftSize=2-(DATA_BUFFER_SIZE-cnt);
////					}
////				}
////			}
////	 }else{                          // Write the left crc byte
////		 if(!sstIsBusy()){            // Flash is not busy
////			 memcpy(&protocolBuffer,&crc+(DATA_BUFFER_SIZE-cnt),crcLeftSize);
////			 sstPageProgram((addr>>8)+pageIndex,protocolBuffer,crcLeftSize);
////			 ret=RETURN_SUCCESS;
////			 cnt=0;
////			 pageIndex=0;
////			 wait=0;
////			 wait=0;
////			 crcLeftSize=0;		 
////       user.profile[id-1].nameCrc=crc;
////       user.profile[id-1].nameStage=true;
////       requestVariableUpdate(SW_CHANGEDPr,id-1);	
////       crc=0xffff;
//////					 offsetCheck=0;
////		 }
////	 }
////	}else{                           // Innternal flash
//		if(size+cnt<DATA_BUFFER_SIZE){ // Buffer is not full , keep filled buffer
//			memcpy(&protocolBuffer[cnt],pData,size);
//			cnt+=size;
//			ret=RETURN_SUCCESS;
//		}else{
//			memcpy(&protocolBuffer[cnt],pData,DATA_BUFFER_SIZE-cnt);					
//			ret=FLASH_Program(addr+pageIndex*PAGE_SIZE,0x40,(U32*)protocolBuffer);
//			pageIndex++;
//			memcpy(&protocolBuffer,pData+(DATA_BUFFER_SIZE-cnt),size-(DATA_BUFFER_SIZE-cnt));
//			cnt=size-(DATA_BUFFER_SIZE-cnt);
//		}
//		
//		if(offset+size>=nameSize){
//			if(cnt+CRC_SIZE<=DATA_BUFFER_SIZE){
//				memcpy(&protocolBuffer[cnt],&crc,CRC_SIZE);
//				memset(protocolBuffer+cnt+CRC_SIZE,0,DATA_BUFFER_SIZE-cnt-CRC_SIZE);
//				ret=FLASH_Program(addr+pageIndex*PAGE_SIZE,0x40,(U32*)protocolBuffer);
//				pageIndex=0;
//				cnt=0;	
////				wait=0;	
//        user.profile[id-1].nameCrc=crc;
//        user.profile[id-1].nameStage=true;
//        requestVariableUpdate(SW_CHANGEDPr,id-1);	        
//				crc=0xffff;		
////            offsetCheck=0;						
//			}else{           // Need another block to writ crc 
//				memcpy(&protocolBuffer[cnt],&crc,DATA_BUFFER_SIZE-cnt);
//				FLASH_Program(addr+pageIndex*PAGE_SIZE,0x40,(U32*)protocolBuffer);
//				crcLeftSize=CRC_SIZE-(DATA_BUFFER_SIZE-cnt);
//				pageIndex++;
//				memcpy(protocolBuffer,&crc+(CRC_SIZE-crcLeftSize),CRC_SIZE-crcLeftSize);
//				memset(&protocolBuffer[CRC_SIZE-crcLeftSize],0,DATA_BUFFER_SIZE-CRC_SIZE-crcLeftSize);
//				ret=FLASH_Program(addr+pageIndex*PAGE_SIZE,0x40,(U32*)protocolBuffer);
//				pageIndex=0;
//				cnt=0;	
////				wait=0;	
//        user.profile[id-1].nameCrc=crc;
//        user.profile[id-1].nameStage=true;
//        requestVariableUpdate(SW_CHANGEDPr,id-1);	
//				crc=0xffff;
////						offsetCheck=0;
//			}					
//		}
////	}
	return ret;
}
U8 profileNameCheck(U8 id)
{
  U8 ret=1;
//  U32 addr, nameSize;
//  U16 crc;
//  U16 crc_cal;
//  if(user.aliveProfile&(1<<id)){    // If the profile is active and the profile name is settle success
//    addr=user.profile[id-1].nameAdd;
//    nameSize=user.profile[id-1].nameSize;
//    if((addr!=0)||(nameSize!=0)){
//       ret=0;                                                                    // retrun value false till the crc match
//       crc_cal=crc16((U8*)user.profile[id-1].nameAdd,user.profile[id-1].nameSize);  // Calculate the crc 
//       readFlash(addr+nameSize,(U8*)&crc,2);	   // Get the profile size
//       if(crc_cal==crc){                                       // Match
//         ret=TRUE;                                             // Return success
//      }
//    }    
//  }
  return ret;
}
void checkAllProfileName(void)
{
//  U8 i;
//  U32 nameSize;
//	for(i=0;i<8;i++){
//		if((user.aliveProfile>>i)&0x01){                 // The profile is alive
//      if(!profileNameCheck(i)){                         // Profile name check fail
//       nameSize=user.profile[i-1].nameSize+CRC_SIZE;
//       if(user.profile[i-1].nameAdd&EXTER_ADDR_FLAG){     // Extern flash
//				 macroTable.recycleSizes+=(nameSize/SECTOR_SIZE+1)*SECTOR_SIZE;
//			 }else{
//				 macroTable.recycleSizes+=(nameSize/PAGE_SIZE+1)*PAGE_SIZE;
//			 }	
//        user.profile[i-1]=dft_generic2.profile[i-1]; // Profile become defautl setting 
//        user.aliveProfile&=~(1<<i);                  // Disable the profile
//        requestVariableUpdate(SW_CHANGEDPr,i-1);	    // Update eeprom profile data
//        requestVariableUpdate(MACRO_CHANGE,0);	      // Update the recycleSizes value
//      }
//		}
//	}
}

void profileBtnTest(U8 num)
{
		switch (num) {
			case 0:		                                          // Profile 1 Red
       ledPwmData[LED_PROFILE].r= 0xff;
			 ledPwmData[LED_PROFILE].g= 0xff;
			 ledPwmData[LED_PROFILE].b= 0xff;
			break;
			case 1:                                            // Profile 2 Yellow
       ledPwmData[LED_PROFILE].r= 0xff;
			 ledPwmData[LED_PROFILE].g= 0;
			 ledPwmData[LED_PROFILE].b= 0;
			break;
			case 2:                                            // Profile 3 Green
       ledPwmData[LED_PROFILE].r= 0;
			 ledPwmData[LED_PROFILE].g= 0xff;
			 ledPwmData[LED_PROFILE].b= 0;
			break;
			case 3:                                            // Profile 4 Cyan
       ledPwmData[LED_PROFILE].r= 0;
			 ledPwmData[LED_PROFILE].g= 0;
			 ledPwmData[LED_PROFILE].b= 0xff;
			break;
			case 4:                                            // Profile 5 Blue
       ledPwmData[LED_PROFILE].r= 0;
			 ledPwmData[LED_PROFILE].g= 0xff;
			 ledPwmData[LED_PROFILE].b= 0xff;
			break;
			case 5:                                            // Profile 5 Blue
       ledPwmData[LED_PROFILE].r= 0xff;
			 ledPwmData[LED_PROFILE].g= 0;
			 ledPwmData[LED_PROFILE].b= 0xff;
			break;
		}
}
