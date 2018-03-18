#include "keyboard.h"
#include "type.h"
#include "string.h"
KEY_HID_IN hid_key;

void fillKeyHid(U8* epBuf,U8 param)
{
	U8 i;
	U8 j=2;
	memset(epBuf,0,8);
	if(param&KEY_IN){
		*epBuf|=hid_key.keyIn[0];
		for(i=2;i<8;i++){                                       // Button key first              
			if(hid_key.keyIn[i]!=0){
				*(epBuf+j)=hid_key.keyIn[i];
				j++;
			}
		}
  }
	if(param&MACRO_IN){
		*epBuf|=hid_key.macroIn[0];
		for(i=2;i<8;i++){                                       // Macro key second
			if(hid_key.macroIn[i]!=0){
				if(j<8){
				*(epBuf+j)=hid_key.macroIn[i];
				 j++;
			 }else{
				 break;
			 }
		 }
		}
  }
	if(param&TURBO_IN){
		*epBuf|=hid_key.turboIn[0];
		for(i=2;i<8;i++){                                       // Turbo key third
			if(hid_key.turboIn[i]!=0){
				if(j<8){
				*(epBuf+j)=hid_key.turboIn[i];
				 j++;
			 }else{
				 break;
			 }
		 }
		}
  }
}

