/*++

Copyright (c) 2013-2014 Razer Ltd.
All rights reserved. This program contains proprietary and confidential information.

Module Name:
  protocol.c

Compiler:
  Keil Software µVision v5.01, with NXP Semiconductor LPC11U2x proccessor support.

Abstract:
  This file provides the driver to device command interface functions and support.

--*/

#include "main.h"
#include "flashprg.h"
#include "protocol.h"
#include "variables.h"
#include "usb_hardware.h"
#include "mouse.h"
#include "gpio.h"
#include "avago_api.h"
//#include "spi0_driver.h"
#include "macro.h"
#include "led_driver.h"
#include "button.h"
//#include "sst26vf016b.h"
#include "storage.h"
#include "profile.h"
#include "SPI.h"
#include "Utility.h"
#include "LED.h"
#include "LED_RamSetting.h"
#include "LED_Const.h"
#include "LED_Table.h"

EEPROM_SAVE eepromSave;
volatile U8 usbBusyFlag=0;  // To set the flag if any get/set command arrive
U8 protocolBuffer[DATA_BUFFER_SIZE]  __attribute__((aligned(4)));
void processPendingCommand(void)
/*++

Function Description:
  This function process the protocol command packet, creates the response packet and 
  returns. If there is data to envaluate in the protocol buffer and its checksum is
  valid, then the commands suitable for this device are processed. This devices supports
  Device Information, LED, Profile, and Macro commands. The checksum calculation is after
  skipping the first two bytes (status, transaction ID), all the bytes to and including
  the checksum XOR to zero.
 
Arguments:
  NONE

Returns: 
  NONE

--*/
{
//  U8 i, j;

  if ((usbReceiveFlag != 0)) {                            // If there USB data packet has been received, then...
//    for (i = 2, j = 0; i < CHECKSUM + 1; i++) {           // Calculate data checksum value (skip Status, trans ID)
//      j ^= protocolTransmit[i];                           // Checksum 6 bytes Command/Status + 80 bytes data + Checksum
//    }
    if (1){//j == 0) {                                         // Exit if invalid checksum
      protocolTransmit[STATUS] = COMMAND_BUSY;            // Set command packet is being processed
      switch (protocolTransmit[COMMAND_CLASS]) {          // Dispatch to corresponding Command type processing

        case INFO_CMD:
				   processDeviceInfoCommand();                    // Dispatch to Device Information command processing
        break;
		
        case BUTTON_CMD:
				  processButtonCommand();                         // Dispatch to button command processing
        break; 
				
//        case DPI_CMD:
//				  processDpiCommand();                            // Dispatch to DPI command processing
//        break;
				
//				case PROFILE_CMD:
//					processProfileCommand();                        // Dispatch to Profile command processing
//				break;
      
      case SENSOR_CMD:
				 processProximityCommand();                       // Dispatch to proximity command processing
      break;
			
			case LED_CMD:
				processLedCommand();                           // Dispatch to chroma command processing
			break;
			case MACRO_CMD:     
        processMacroCommand();                            // 	Dispatch to macro command processing
			break;
      default:                                            // Command Classes not support by this device
        protocolTransmit[STATUS] = COMMAND_NOTSUPPORT;    // Not support cmd, return 04 to SW
        break;
      }
			
//      for (i = 2, j = 0; i < CHECKSUM; i++) {             // Calculate data checksum value (skip Status, trans ID)
//        j ^= protocolTransmit[i];                         // Checksum 8 bytes Command/Status + 80 bytes data
//      }
//      protocolTransmit[CHECKSUM] = j;                     // Update data checksum value
      if (protocolTransmit[STATUS] == COMMAND_BUSY) {
        protocolTransmit[STATUS] = RESPONSE_READY;        // Set Report ID
      }
    } else {
      protocolTransmit[STATUS] = COMMAND_ERROR;           // Checksum error, return error cmd
    }
		if(!protocolWait){
     usbReceiveFlag = 0;                                   // Clear receive flag to aollow next protocol cmd can be enter
		}
  }
}

void processDeviceInfoCommand(void)
/*++

Function Description:

  This function processes the Device Information protocol commands. This device
  allows one to retrieve the Firmware Version (standard & extended), the Serial 
  Number, the Protocol Version, and the current Device Mode. This device allows
  the Serial Number, the Protocol Version, and the Device Mode values to be set.
 
Arguments:
  NONE

Returns: 
  NONE

--*/
{
    U8 *ptr,i,j,n,m,temp,temp1;
//  U16 tempDpi;
  ptr = &protocolTransmit[PAYLOAD];
  switch (protocolTransmit[COMMAND_ID]) {
    case (SET | DEVICE_MODE):
      deviceMode = ptr[0];
      break;
    case (GET | DEVICE_MODE):
      ptr[0] = deviceMode;
      break;      
    case (GET | FW_VERSION):
      ptr[0] = firmwareVersion[0];
      ptr[1] = firmwareVersion[1];
      ptr[2] = firmwareVersion[2];
     break;
    case (GET | POLLING_INFO):
      ptr[0] = user.pollingrate;
      break;  
    case (SET | POLLING_INFO):
      user.pollingrate = ptr[0];
      requestVariableUpdate(SW_CHANGEDPr,NULL);
      break; 
    case (GET | PROFILE_DPI):  
        n = ptr[0]; 
        if (n) {    
        i = ptr[0]-1;
        *++ptr = user.profile[i].sensor.angleSnap;
        *++ptr = user.profile[i].sensor.maxStages;
        *++ptr = user.profile[i].sensor.dpiStage;
        *++ptr = (U8)(user.profile[i].activeDpi.x >> 8);
        *++ptr = (U8)(user.profile[i].activeDpi.x );
        *++ptr = (U8)(user.profile[i].activeDpi.y >> 8);
        *++ptr = (U8)(user.profile[i].activeDpi.y ); 
        for (j=0; j<user.profile[i].sensor.maxStages; j++) {
          *++ptr = (U8)(user.profile[i].sensor.stageDPI[j].x >> 8);
          *++ptr = (U8)(user.profile[i].sensor.stageDPI[j].x );
          *++ptr = (U8)(user.profile[i].sensor.stageDPI[j].y >> 8);
          *++ptr = (U8)(user.profile[i].sensor.stageDPI[j].y );    
        }
      } else {
        ptr[4] = (U8)(dpiCurrent.x >> 8);
        ptr[5] = (U8)dpiCurrent.x;
        ptr[6] = (U8)(dpiCurrent.y >> 8);
        ptr[7] = (U8)dpiCurrent.y;        
      }        
//      }    
      break;  
    case (SET | PROFILE_DPI):
//      for (i=0; i<ptr[0]; i++) {
        n = ptr[0];                          // profile number 
        if (n) {
          i = n-1;                             // get the correct profile numbe if not 0, i should always =0 for 3325
          user.profile[i].sensor.angleSnap = *++ptr;  // angle snap setting 
  //        n = *++ptr;  
          m = user.profile[i].sensor.maxStages = *++ptr;  // max stages  
          user.profile[i].sensor.dpiStage = *++ptr;       // current stage 
          temp =  *++ptr;
          temp1 = *++ptr;
          user.profile[i].activeDpi.x  = temp << 8 | temp1;  // current DPI x 
          temp =  *++ptr;
          temp1 = *++ptr;       
          user.profile[i].activeDpi.y  = temp << 8 | temp1;  // current DPI y 
          for (j=0; j<m; j++) {
            temp =  *++ptr;
            temp1 = *++ptr;
            user.profile[i].sensor.stageDPI[j].x = temp << 8 | temp1;
            temp =  *++ptr;
            temp1 = *++ptr;
            user.profile[i].sensor.stageDPI[j].y = temp << 8 | temp1;   
          }
        if (i == user.ProNO) {
         setResolution(user.profile[user.ProNO].activeDpi.x,user.profile[user.ProNO].activeDpi.y);
//          __UT_MAIN_tSWW;
          SPI_W_2BYTE(REG_ANGLE_SNAP, user.profile[i].sensor.angleSnap > 0 ? 0x80:0x00); // setting the angle snap  
        }
        requestVariableUpdate(SW_CHANGEDPr,NULL);          
      } else {
        temp =  ptr[4];
        temp1 = ptr[5];
        dpiCurrent.x = temp << 8 | temp1;
        temp =  ptr[6];
        temp1 = ptr[7];        
        dpiCurrent.y = temp << 8 | temp1;
        //SPI_W_2BYTE(SENSOR_RESOLUTION_3325, dpiTable[( dpiCurrent.x -200)/100]); 
        setResolution(dpiCurrent.x,dpiCurrent.y);
      }
      copyLiftoff2Active(0);      // copy the setting to IRsensor 
      break;        
    default:
      break;
  }
//  U8 i;
//  U8 *ptr = &protocolTransmit[PAYLOAD];                   // Reference first byte of Protocol Payload
//  
//  switch (protocolTransmit[COMMAND_ID]) {
//    case (GET | EXT_FIRMWARE_VER):                        // Return 4 bytes [Major, Minor, Internal, Reserved]
//      *(ptr + 2) = firmwareVersion[2];
//      *(ptr + 3) = firmwareVersion[3];
//    case (GET | FIRMWARE_VER):                            // Return 2 bytes [Major value, Minor value]
//      *(ptr + 0) = firmwareVersion[0];
//      *(ptr + 1) = firmwareVersion[1];
//    break;
//                  
//    case (GET | SERIAL_NUMBER):                           // Return 22 bytes [...]
//      for (i = 0; i < sizeof(sys.devSerialNumber); i++) {
//        *ptr++ = sys.devSerialNumber[i];
//      }
//    break;
//		case (SET | SERIAL_NUMBER):                           // Change Serial Number (22 bytes)
//      for (i = 0; i < sizeof(sys.devSerialNumber); i++) {
//        sys.devSerialNumber[i] = *ptr++;
//      }
//      requestVariableUpdate(FACTORY_CHANGED,NULL);             // Generic variable storage needs updating
//    break;
//                  
//    case (GET | DEVICE_MODE):                             // Return 1 byte [Device Mode]
//      *ptr = deviceMode;
//    break;
//		case (SET | DEVICE_MODE):                             // [Device Mode]
//      deviceMode = ptr[0];
//		  if (deviceMode == BOOTLOADER_MODE) {				                                             // All proflie need to update 
//		     requestVariableUpdate(BOOTLOADER_CHANGE,0);              // Generic1 variable storage needs updating
//      }else if(deviceMode == NORMAL_MODE){                                    // Active to current profile
////				 memset((void*)lighting,(int)0,(int)sizeof(lighting));                // clear lighting
////         memset((void*)matrixStaging,(int)0,(int)sizeof(matrixStaging));       //clear staging
////				 forceLightingReload();                                                // Force a reload & refresh of all LEDs
////				 setResolution(user.profile[user.ProNO].activeDpi.x,user.profile[user.ProNO].activeDpi.y);
//			}else if(deviceMode == DRIVER_MODE){                                     // Change to profile 1
////				user.PClass=1;
////				user.ProNO=user.PClass-1;
////				//memset((void*)lighting,(int)0,(int)sizeof(lighting));                  // clear lighting
////        //memset((void*)matrixStaging,(int)0,(int)sizeof(matrixStaging));        //clear staging
////				forceLightingReload();
////				setResolution(user.profile[user.ProNO].activeDpi.x,user.profile[user.ProNO].activeDpi.y);
//				switchProfile(PROFILE_NUM,1);
//				requestVariableUpdate(USER_CHANGED,user.ProNO);                                  // Generic variable storage needs updating
//			}
//    break;
//        
//    case (GET | POLLING_PERIOD):                          // Return 1 byte [Polling rate]
//		  *ptr = user.pollingrate;
//    break;
//    case (SET | POLLING_PERIOD):                          // [Polling rate]
//			user.pollingrate = ptr[0]; 
//			requestVariableUpdate(SW_CHANGED,NULL);                // Profile 1/2/3 Generic variable storage needs updating
//    break;
//        
//    case (GET | EDITION_INFO):                            // [Device Edtion Information]
//      ptr[0] = sys.editionInfo[0];                   // Keyboard layout value, set 0 invail value for mouse
//      ptr[1] = sys.editionInfo[1];   
//    break;
//    case (SET | EDITION_INFO):                            // [Device Edtion Information]
//      sys.editionInfo[0] = ptr[0];                   // Keyboard layout value, set 0 invail value for mouse
//      sys.editionInfo[1] = ptr[1];   
//      requestVariableUpdate(FACTORY_CHANGED,NULL);             // Generic variable storage needs updating
//    break;
//		
//    case (SET | DEVICE_RESET):
//       if (ptr[0] == RESET_DEVICE) {
//         __disable_irq();                                 // Disable interrupts while flashing, take about 12ms to write the eeprom
//         NVIC_SystemReset();                              // Request a MCU soft reset
//         for ( ; ; ) ;
//       } else if (ptr[0] == RESET_VARIABLES) {            //Reset Variables Storage to default
//				  memcpy(&user, (void *)&dft_generic1, sizeof(user));           // copy dft_generic1 to user
//				  memcpy(&user, (void *)&dft_generic2, sizeof(user));       // copy dft_generic2 to user
//				  copyLiftoff2Active(0x00);                                     // Copy the lift value to active function
//				  memcpy((U8*)&btnAssign,(U8*)&user.profile[user.ProNO].btnAssign,sizeof(btnAssign));  // Copy the profile assignment to btnAssign
//					requestVariableUpdate(SW_CHANGED,user.ProNO);              // Generic variable storage needs updating
//				  i=profileNO;
//				  while(i){                                              // All proflie need to update 
//				   requestVariableUpdate(SW_CHANGEDPr,--i);              // Generic1 variable storage needs updating
//					}
//          (void)memset((void*)lighting,(int)0,(int)sizeof(lighting));   // clear lighting
//          (void)memset((void*)matrixStaging,(int)0,(int)sizeof(matrixStaging));     //clear staging
//          forceLightingReload();                                        // Force a reload & refresh of all LEDs
//          setResolution(user.profile[user.ProNO].activeDpi.x,user.profile[user.ProNO].activeDpi.y);
//				  formatMacroMemory();
//				  memset((void *)&macroTable,0,sizeof(macroTable));
//					i=MAX_MACRO;
//					while(i){
//				    requestVariableUpdate(MACRO_CHANGE,--i);
//					}
//			 }
//    break;
//		case (GET|AMBIDEXATROUS):
//			ptr[0]=user.ambidextrous;
//			break;
//		case (SET|AMBIDEXATROUS):
//			user.ambidextrous=ptr[0];
//		  requestVariableUpdate(SW_CHANGED,user.ProNO);             // Generic variable storage needs updating
//			break;
//    default:    
//      protocolTransmit[STATUS] = COMMAND_NOTSUPPORT;        // Not support cmd, return 05 to SW
//    break;
//  }
}

void processButtonCommand(void)
/*++

Function Description:
  This function process the button protocol commands. 
  Only support button debounce time cmd for this project
 
Arguments:
  NONE

Returns: 
  NONE

--*/
{
  U8 *ptr = &protocolTransmit[PAYLOAD];                   // Reference first byte of Protocol Payload
	U8 i = 0,j = 0;
	U8 mode;
//	U8 tmp;  
  switch (protocolTransmit[COMMAND_ID]) {
		case (GET | BTN_SINGLE_ASSIGN):			
			while ((user.profile[0].btnAssign[i].id != ptr[1]) && (i < NUMBER_OF_BUTTONS)) {    
					i++;                                              // Look for parameters with matching btnAssign
			}
			if(i<NUMBER_OF_BUTTONS){                              // button is legal
				if (ptr[0] == 0 ) {                                 // cureent profile		
						mode=(ptr[2]==MODE_NORM)?NROM:SPECIFIC;
						ptr[1] = btnAssign[i].id;          // Return id
						ptr[2] = btnAssign[i].mode;        // Return type
						ptr[3] = btnAssign[i].type[mode];        // Return type
						ptr[4] = btnAssign[i].size[mode];        // Return size
						ptr[5] = btnAssign[i].d1[mode];    // Return d1
						ptr[6] = btnAssign[i].d2[mode];    // Return d2
						ptr[7] = btnAssign[i].d3[mode];    // Return d3
						ptr[8] = btnAssign[i].d4[mode];    // Return d4
						ptr[9] = btnAssign[i].d5[mode];    // Return d5
						protocolTransmit [DATA_SIZE] = 10;
				} else {                                            // others profile
					  j = ptr[0] - 1;			
						mode=(ptr[2]==MODE_NORM)?NROM:SPECIFIC;
						ptr[1] = user.profile[j].btnAssign[i].id;          // Return id
						ptr[2] = user.profile[j].btnAssign[i].mode;        // Return type
						ptr[3] = user.profile[j].btnAssign[i].type[mode];        // Return type
						ptr[4] = user.profile[j].btnAssign[i].size[mode];        // Return size
						ptr[5] = user.profile[j].btnAssign[i].d1[mode];    // Return d1
						ptr[6] = user.profile[j].btnAssign[i].d2[mode];    // Return d2
						ptr[7] = user.profile[j].btnAssign[i].d3[mode];    // Return d3
						ptr[8] = user.profile[j].btnAssign[i].d4[mode];    // Return d4
						ptr[9] = user.profile[j].btnAssign[i].d5[mode];    // Return d5
						protocolTransmit [DATA_SIZE] = 10;
					}
	 }else{
		 protocolTransmit[STATUS] = COMMAND_ERROR;         // if over the buttons,then error
	 }
		break;
		case (GET|BUTTON_ID_LIST):
			ptr[0]=NUMBER_OF_BUTTONS;
		  for(i=1;i<=NUMBER_OF_BUTTONS;i++){
				ptr[i]=user.profile[0].btnAssign[i-1].id;
			}		  
			break;
		case (SET | BTN_SINGLE_ASSIGN):
			while ((user.profile[0].btnAssign[i].id != ptr[1]) && (i < NUMBER_OF_BUTTONS)) {    
					i++;                                              // Look for parameters with matching btnAssign
			}
			if(i<NUMBER_OF_BUTTONS){
				if (ptr[0] == 0 ) {                                 // cureent profile
					mode=(ptr[2]==0)?NROM:SPECIFIC;
					btnAssign[i].id          = ptr[1];        // Update id
					btnAssign[i].mode        = ptr[2];        // Update mode
					btnAssign[i].type[mode]  = ptr[3];        // Update type
					btnAssign[i].size[mode]  = ptr[4];        // Update size
					btnAssign[i].d1[mode]    = ptr[5];        // Update d1
					btnAssign[i].d2[mode]    = ptr[6];        // Update d2
					btnAssign[i].d3[mode]    = ptr[7];        // Update d3
					btnAssign[i].d4[mode]    = ptr[8];        // Update d3
					btnAssign[i].d5[mode]    = ptr[9];        // Update d3
				} else if(ptr[0]==user.PClass) {                                            // others profile
					j = ptr[0] - 1;
					mode=(ptr[2]==0)?NROM:SPECIFIC;
					user.profile[j].btnAssign[i].id          = ptr[1];        // Update id
					user.profile[j].btnAssign[i].mode        = ptr[2];        // Update mode
					user.profile[j].btnAssign[i].type[mode]  = ptr[3];        // Update type
					user.profile[j].btnAssign[i].size[mode]  = ptr[4];        // Update size
					user.profile[j].btnAssign[i].d1[mode]    = ptr[5];        // Update d1
					user.profile[j].btnAssign[i].d2[mode]    = ptr[6];        // Update d2
					user.profile[j].btnAssign[i].d3[mode]    = ptr[7];        // Update d3
					user.profile[j].btnAssign[i].d4[mode]    = ptr[8];        // Update d3
					user.profile[j].btnAssign[i].d5[mode]    = ptr[9];        // Update d3
					memcpy((U8*)&btnAssign[i],(U8*)&user.profile[j].btnAssign[i],sizeof(btnAssign[i])); // copy to the active variable
					requestVariableUpdate(SW_CHANGEDPr,j);              // Generic1 variable storage needs updating
				}else{
					j = ptr[0] - 1;
					mode=(ptr[2]==0)?NROM:SPECIFIC;
					user.profile[j].btnAssign[i].id          = ptr[1];        // Update id
					user.profile[j].btnAssign[i].mode        = ptr[2];        // Update mode
					user.profile[j].btnAssign[i].type[mode]  = ptr[3];        // Update type
					user.profile[j].btnAssign[i].size[mode]  = ptr[4];        // Update size
					user.profile[j].btnAssign[i].d1[mode]    = ptr[5];        // Update d1
					user.profile[j].btnAssign[i].d2[mode]    = ptr[6];        // Update d2
					user.profile[j].btnAssign[i].d3[mode]    = ptr[7];        // Update d3
					user.profile[j].btnAssign[i].d4[mode]    = ptr[8];        // Update d3
					user.profile[j].btnAssign[i].d5[mode]    = ptr[9];        // Update d3
					requestVariableUpdate(SW_CHANGEDPr,j);              // Generic1 variable storage needs updating
				}
		}else{
			protocolTransmit[STATUS] = COMMAND_ERROR;         // if over the buttons,then error
		}
	  break;
		
//    case (GET | BUTTON_DEBOUNCE):                         // Return make debounce
//      ptr[0] = user.makeDebounce;
//      ptr[1] = user.makeMsk;
//      ptr[2] = user.breakDebounce;
//      ptr[3] = user.breakMsk;
//    break;
//    case (SET | BUTTON_DEBOUNCE):                         // Update Button debounce time
//      user.makeDebounce  = ptr[0];
//      user.makeMsk       = ptr[1];
//      user.breakDebounce = ptr[2];
//      user.breakMsk      = ptr[3];
//      if (protocolTransmit[DATA_SIZE] == 2) {             // forward compatibilty
//        user.breakDebounce = user.makeDebounce;
//        user.breakMsk = user.makeMsk;
//      }
//      requestVariableUpdate(SW_CHANGED,user.ProNO);               // Generic1 variable storage needs updating

//    break;  
//		case (GET|BTN_ASSIGN_LIST):                        // Get the button assignment list with offset
//			if(ptr[0]!=0){
//			  tmp=protocolTransmit[DATA_SIZE];
//		    GetBtnAssgin(ptr[0],ptr[1],ptr[2],&ptr[3],(tmp-3)/8);   // -2:for profile and offset , /9: very button assign 9 parmarters 
//			}else{
//				 protocolTransmit[STATUS] = COMMAND_ERROR;           // Not support cmd, return error to SW
//			}
//			break;
    default:
      protocolTransmit[STATUS] = COMMAND_ERROR;           // Not support cmd, return error to SW
    break;
  }
}


void processDpiCommand(void)
/*++

Function Description:
  This function processes the DPI commands. This device support getting and setting of
  a single DPI Stage. Because the sensor only allows symmetrical settings (i.e. X = Y),
  the sensor resolution is set based on the dpi_X entry.
 
Arguments:
  NONE

Returns: 
  NONE

--*/
{

//	U8 *ptr = &protocolTransmit[PAYLOAD];                   // Reference first byte of Protocol Payload
//	U8 i = 0,j = 0;
//  switch (protocolTransmit[COMMAND_ID]) {
//	  case (GET | DPI_ACTIVE_STAGE):                        // to get or set the device's active DPI stage of the currently selected profile
//     // ptr[0] = user.profile[user.ProNO].sensor.dpiStage + 1;
//		ptr[0] =  IRSensor.dpiStage + 1;
//    break;
//    case (SET | DPI_ACTIVE_STAGE):
//      if ((ptr[0] > 0) && (ptr[0] <= IRSensor.maxStages)) {
//        IRSensor.dpiStage = user.profile[user.ProNO].sensor.dpiStage = ptr[0] - 1;
//				dpiCurrent.x=IRSensor.stageDPI[IRSensor.dpiStage].x;
//				dpiCurrent.y=IRSensor.stageDPI[IRSensor.dpiStage].y;
//				user.profile[user.ProNO].activeDpi.x=dpiCurrent.x;
//				user.profile[user.ProNO].activeDpi.y=dpiCurrent.y;
//				setResolution(dpiCurrent.x,dpiCurrent.y);
//        requestVariableUpdate(SW_CHANGEDPr,user.ProNO);              // Generic variable storage needs updating
//      } else {
//        protocolTransmit[STATUS] = COMMAND_ERROR;         // stage out of the range
//      } 
//    break;
//    case (GET|DPI_CLASS_TABLE):
//      if(ptr[0]==0){                                    // Get current table
//				ptr[1]=IRSensor.dpiStage+1;
//				ptr[2]=0x05;
//				j=3;
//				for(i=0;i<NUMBER_OF_STAGES;i++){
//	        ptr[j++]=i+1;             // stage id
//				  ptr[j++]=IRSensor.stageDPI[i].x>>8;
//				  ptr[j++]=IRSensor.stageDPI[i].x&0xff;
//				  ptr[j++]=IRSensor.stageDPI[i].y>>8;
//				  ptr[j++]=IRSensor.stageDPI[i].y&0xff;
//				  ptr[j++]=IRSensor.stageDPI[i].z>>8;
//				  ptr[j++]=IRSensor.stageDPI[i].z&0xff;
//				}
//			}else{                                         // Others need to get from user
//				ptr[1]=user.profile[ptr[0]-1].sensor.dpiStage+1;
//				ptr[2]=0x05;
//				j=3;
//				for(i=0;i<NUMBER_OF_STAGES;i++){
//	        ptr[j++]=i+1;             // stage id
//				  ptr[j++]=user.profile[ptr[0]-1].sensor.stageDPI[i].x>>8;
//				  ptr[j++]=user.profile[ptr[0]-1].sensor.stageDPI[i].x&0xff;
//				  ptr[j++]=user.profile[ptr[0]-1].sensor.stageDPI[i].y>>8;
//				  ptr[j++]=user.profile[ptr[0]-1].sensor.stageDPI[i].y&0xff;
//				  ptr[j++]=user.profile[ptr[0]-1].sensor.stageDPI[i].z>>8;
//				  ptr[j++]=user.profile[ptr[0]-1].sensor.stageDPI[i].z&0xff;
//				}
//			}
//			break;
//		case (SET|DPI_CLASS_TABLE):
//			if((ptr[1]>0)&&(ptr[1]<=NUMBER_OF_STAGES)){           // Active stage need to range 1-5
//				if(ptr[0]==0){                                 // Profile 0 ,will not change
//					for(i=0;i<NUMBER_OF_STAGES;i++){
//						IRSensor.stageDPI[i].x=ptr[4+i*7];
//						IRSensor.stageDPI[i].x=(IRSensor.stageDPI[i].x<<8)|ptr[5+i*7];
//						IRSensor.stageDPI[i].y=ptr[6+i*7];
//						IRSensor.stageDPI[i].y=(IRSensor.stageDPI[i].y<<8)|ptr[7+i*7];
//						IRSensor.stageDPI[i].z=ptr[8+i*7];
//						IRSensor.stageDPI[i].z=(IRSensor.stageDPI[i].z<<8)|ptr[9+i*7];
//					}
//					IRSensor.dpiStage=ptr[1]-1;
//					dpiCurrent.x=IRSensor.stageDPI[IRSensor.dpiStage].x;
//					dpiCurrent.y=IRSensor.stageDPI[IRSensor.dpiStage].y;
//					setResolution(dpiCurrent.x,dpiCurrent.y);
//				}else if(ptr[0]==user.PClass){              // Current profile
//					for(i=0;i<NUMBER_OF_STAGES;i++){
//						user.profile[user.ProNO].sensor.stageDPI[i].x=ptr[4+i*7];
//						user.profile[user.ProNO].sensor.stageDPI[i].x=(user.profile[user.ProNO].sensor.stageDPI[i].x<<8)|ptr[5+i*7];
//						user.profile[user.ProNO].sensor.stageDPI[i].y=ptr[6+i*7];
//						user.profile[user.ProNO].sensor.stageDPI[i].y=(user.profile[user.ProNO].sensor.stageDPI[i].y<<8)|ptr[7+i*7];
//						user.profile[user.ProNO].sensor.stageDPI[i].z=ptr[8+i*7];
//						user.profile[user.ProNO].sensor.stageDPI[i].z=(user.profile[user.ProNO].sensor.stageDPI[i].z<<8)|ptr[9+i*7];
//					}	
//					IRSensor=user.profile[user.ProNO].sensor;
//					user.profile[user.ProNO].sensor.dpiStage=IRSensor.dpiStage=ptr[1]-1;
//					dpiCurrent.x=IRSensor.stageDPI[IRSensor.dpiStage].x;
//					dpiCurrent.y=IRSensor.stageDPI[IRSensor.dpiStage].y;
//					user.profile[user.ProNO].activeDpi.x=dpiCurrent.x;
//					user.profile[user.ProNO].activeDpi.y=dpiCurrent.y;
//					setResolution(dpiCurrent.x,dpiCurrent.y);
//					requestVariableUpdate(SW_CHANGEDPr,user.ProNO);              // Generic variable storage needs updating		
//				}else{                                                          // Other profile ,just save
//					for(i=0;i<NUMBER_OF_STAGES;i++){
//						user.profile[ptr[0]-1].sensor.stageDPI[i].x=ptr[4+i*7];
//						user.profile[ptr[0]-1].sensor.stageDPI[i].x=(user.profile[ptr[0]-1].sensor.stageDPI[i].x<<8)|ptr[5+i*7];
//						user.profile[ptr[0]-1].sensor.stageDPI[i].y=ptr[6+i*7];
//						user.profile[ptr[0]-1].sensor.stageDPI[i].y=(user.profile[ptr[0]-1].sensor.stageDPI[i].y<<8)|ptr[7+i*7];
//						user.profile[ptr[0]-1].sensor.stageDPI[i].z=ptr[8+i*7];
//						user.profile[ptr[0]-1].sensor.stageDPI[i].z=(user.profile[ptr[0]-1].sensor.stageDPI[i].z<<8)|ptr[9+i*7];
//					}	
//					user.profile[ptr[0]-1].sensor.dpiStage=ptr[1]-1;
//					user.profile[ptr[0]-1].activeDpi.x=user.profile[ptr[0]-1].sensor.stageDPI[ptr[1]-1].x;
//					user.profile[ptr[0]-1].activeDpi.y=user.profile[ptr[0]-1].sensor.stageDPI[ptr[1]-1].y;
//					requestVariableUpdate(SW_CHANGEDPr,ptr[0]-1);              // Generic variable storage needs updating		
//				}
//		}else{                                                           // stage is out of range ,error
//			protocolTransmit[STATUS] = COMMAND_ERROR;        // stage out of the range
//		}
//			break;
//    case (GET | DPI_CLASS_STAGE):                         // To get the device's single DPI stage data
//      i = (ptr[0] > 0) ? (ptr[0] - 1) : ptr[0];           // DPI Class
//      if ((ptr[1] > 0) && (ptr[1] <= IRSensor.maxStages)) {
//        j = ptr[1] - 1;
//        if  (ptr[0] != 0) {
//          ptr[2] = (U8)(user.profile[i].sensor.stageDPI[j].x >> 8);
//          ptr[3] = (U8)(user.profile[i].sensor.stageDPI[j].x);
//          ptr[4] = (U8)(user.profile[i].sensor.stageDPI[j].y >> 8);
//          ptr[5] = (U8)(user.profile[i].sensor.stageDPI[j].y);
//        } else {
//          ptr[2] = (U8)(IRSensor.stageDPI[j].x >> 8);
//          ptr[3] = (U8)(IRSensor.stageDPI[j].x);
//          ptr[4] = (U8)(IRSensor.stageDPI[j].y>> 8);
//          ptr[5] = (U8)(IRSensor.stageDPI[j].y);
//        }
//        ptr[6] = 0;
//        ptr[7] = 0;
//      } else {
//        protocolTransmit[STATUS] = COMMAND_ERROR;         // stage out of the range
//      }
//    break;
//    case (SET | DPI_CLASS_STAGE):                         // To set the device's single DPI stage data
//      i = (ptr[0] > 0) ? (ptr[0] - 1) : ptr[0];           // DPI Class
//      if ((ptr[1] > 0) &&( (ptr[0]==0&&ptr[1] <= IRSensor.maxStages)||(ptr[0]!=0&&ptr[1] <= user.profile[i].sensor.maxStages))) {
//        j = ptr[1] - 1;
//        if (ptr[0] != 0) {
//          user.profile[i].sensor.stageDPI[j].x = ((U16)ptr[2] << 8) | ptr[3];
//          user.profile[i].sensor.stageDPI[j].y  = ((U16)ptr[4] << 8) | ptr[5];
//          if (ptr[0] ==  user.PClass) {
//            IRSensor.stageDPI[j].x = ((U16)ptr[2] << 8) | ptr[3];
//            IRSensor.stageDPI[j].y = ((U16)ptr[4] << 8) | ptr[5];
//          }
//          requestVariableUpdate(SW_CHANGEDPr,i);            // Generic variable storage needs updating
//        } else {                                          // profile 0
//          IRSensor.stageDPI[j].x = ((U16)ptr[2] << 8) | ptr[3];
//          IRSensor.stageDPI[j].y= ((U16)ptr[4] << 8) | ptr[5];
//        }
//      } else {
//         protocolTransmit[STATUS] = COMMAND_ERROR;        // stage out of the range
//      }
//    break;
//		case (GET|DPI_STAGE_TABLE):
//      if(ptr[0]==0){                                    // Get current table
//				ptr[1]=IRSensor.dpiStage+1;
//				ptr[2]=IRSensor.maxStages;
//				j=3;
//				for(i=0;i<IRSensor.maxStages;i++){
//	        ptr[j++]=i+1;             // stage id
//				  ptr[j++]=IRSensor.stageDPI[i].x>>8;
//				  ptr[j++]=IRSensor.stageDPI[i].x&0xff;
//				  ptr[j++]=IRSensor.stageDPI[i].y>>8;
//				  ptr[j++]=IRSensor.stageDPI[i].y&0xff;
//				  ptr[j++]=IRSensor.stageDPI[i].z>>8;
//				  ptr[j++]=IRSensor.stageDPI[i].z&0xff;
//				}
//			}else{                                         // Others need to get from user
//				ptr[1]=user.profile[ptr[0]-1].sensor.dpiStage+1;
//				ptr[2]=user.profile[ptr[0]-1].sensor.maxStages;
//				j=3;
//				for(i=0;i<user.profile[ptr[0]-1].sensor.maxStages;i++){
//	        ptr[j++]=i+1;             // stage id
//				  ptr[j++]=user.profile[ptr[0]-1].sensor.stageDPI[i].x>>8;
//				  ptr[j++]=user.profile[ptr[0]-1].sensor.stageDPI[i].x&0xff;
//				  ptr[j++]=user.profile[ptr[0]-1].sensor.stageDPI[i].y>>8;
//				  ptr[j++]=user.profile[ptr[0]-1].sensor.stageDPI[i].y&0xff;
//				  ptr[j++]=user.profile[ptr[0]-1].sensor.stageDPI[i].z>>8;
//				  ptr[j++]=user.profile[ptr[0]-1].sensor.stageDPI[i].z&0xff;
//				}
//			}
//			break;
//		case (SET|DPI_STAGE_TABLE):
//			if((ptr[1]>0)&&(ptr[1]<=5)&&(ptr[2]>0)&&(ptr[2]<=5)){           // Active stage need to range 1-5
//				if(ptr[0]==0){                                 // Profile 0 ,will not change
//					IRSensor.maxStages=ptr[2];
//					for(i=0;i<IRSensor.maxStages;i++){
//						IRSensor.stageDPI[i].x=ptr[4+i*7];
//						IRSensor.stageDPI[i].x=(IRSensor.stageDPI[i].x<<8)|ptr[5+i*7];
//						IRSensor.stageDPI[i].y=ptr[6+i*7];
//						IRSensor.stageDPI[i].y=(IRSensor.stageDPI[i].y<<8)|ptr[7+i*7];
//						IRSensor.stageDPI[i].z=ptr[8+i*7];
//						IRSensor.stageDPI[i].z=(IRSensor.stageDPI[i].z<<8)|ptr[9+i*7];
//					}
//					if(ptr[1]>IRSensor.maxStages){         // If the active stage bigger than max stages , make it the same
//						ptr[1]=IRSensor.maxStages;
//					}
//					IRSensor.dpiStage=ptr[1]-1;
//					dpiCurrent.x=IRSensor.stageDPI[IRSensor.dpiStage].x;
//					dpiCurrent.y=IRSensor.stageDPI[IRSensor.dpiStage].y;
//					setResolution(dpiCurrent.x,dpiCurrent.y);
//				}else if(ptr[0]==user.PClass){              // Current profile
//					user.profile[user.ProNO].sensor.maxStages=ptr[2];
//					for(i=0;i<user.profile[user.ProNO].sensor.maxStages;i++){
//						user.profile[user.ProNO].sensor.stageDPI[i].x=ptr[4+i*7];
//						user.profile[user.ProNO].sensor.stageDPI[i].x=(user.profile[user.ProNO].sensor.stageDPI[i].x<<8)|ptr[5+i*7];
//						user.profile[user.ProNO].sensor.stageDPI[i].y=ptr[6+i*7];
//						user.profile[user.ProNO].sensor.stageDPI[i].y=(user.profile[user.ProNO].sensor.stageDPI[i].y<<8)|ptr[7+i*7];
//						user.profile[user.ProNO].sensor.stageDPI[i].z=ptr[8+i*7];
//						user.profile[user.ProNO].sensor.stageDPI[i].z=(user.profile[user.ProNO].sensor.stageDPI[i].z<<8)|ptr[9+i*7];
//					}	
//					if(ptr[1]>user.profile[user.ProNO].sensor.maxStages){         // If the active stage bigger than max stages , make it the same
//						ptr[1]=user.profile[user.ProNO].sensor.maxStages;
//					}
//					user.profile[user.ProNO].sensor.dpiStage=ptr[1]-1;
//					IRSensor=user.profile[user.ProNO].sensor;
//					dpiCurrent.x=IRSensor.stageDPI[IRSensor.dpiStage].x;
//					dpiCurrent.y=IRSensor.stageDPI[IRSensor.dpiStage].y;
//					user.profile[user.ProNO].activeDpi.x=dpiCurrent.x;
//					user.profile[user.ProNO].activeDpi.y=dpiCurrent.y;
//					setResolution(dpiCurrent.x,dpiCurrent.y);
//					requestVariableUpdate(SW_CHANGEDPr,user.ProNO);              // Generic variable storage needs updating		
//				}else{                                                          // Other profile ,just save
//					user.profile[ptr[0]-1].sensor.maxStages=ptr[2];
//					for(i=0;i<user.profile[ptr[0]-1].sensor.maxStages;i++){
//						user.profile[ptr[0]-1].sensor.stageDPI[i].x=ptr[4+i*7];
//						user.profile[ptr[0]-1].sensor.stageDPI[i].x=(user.profile[ptr[0]-1].sensor.stageDPI[i].x<<8)|ptr[5+i*7];
//						user.profile[ptr[0]-1].sensor.stageDPI[i].y=ptr[6+i*7];
//						user.profile[ptr[0]-1].sensor.stageDPI[i].y=(user.profile[ptr[0]-1].sensor.stageDPI[i].y<<8)|ptr[7+i*7];
//						user.profile[ptr[0]-1].sensor.stageDPI[i].z=ptr[8+i*7];
//						user.profile[ptr[0]-1].sensor.stageDPI[i].z=(user.profile[ptr[0]-1].sensor.stageDPI[i].z<<8)|ptr[9+i*7];
//					}	
//					if(ptr[1]>user.profile[ptr[0]-1].sensor.maxStages){         // If the active stage bigger than max stages , make it the same
//						ptr[1]=user.profile[ptr[0]-1].sensor.maxStages;
//					}
//					user.profile[ptr[0]-1].sensor.dpiStage=ptr[1]-1;
//					user.profile[ptr[0]-1].activeDpi.x=user.profile[ptr[0]-1].sensor.stageDPI[ptr[1]-1].x;
//					user.profile[ptr[0]-1].activeDpi.y=user.profile[ptr[0]-1].sensor.stageDPI[ptr[1]-1].y;
//					requestVariableUpdate(SW_CHANGEDPr,ptr[0]-1);              // Generic variable storage needs updating		
//				}
//		}else{                                                           // stage is out of range ,error
//			protocolTransmit[STATUS] = COMMAND_ERROR;        // stage out of the range
//		}
//			break;
//		case (GET|DPI_PROFILE_STAGE):
//			if(ptr[0]==0){
//				ptr[1]=IRSensor.dpiStage+1;
//			}else{
//				ptr[1]=user.profile[ptr[0]-1].sensor.dpiStage+1;
//			}
//			break;
//		case (SET|DPI_PROFILE_STAGE):
//				if(ptr[0]==0){
//					if((ptr[1]>0)&&(ptr[1]<=IRSensor.maxStages)){
//					  IRSensor.dpiStage =ptr[1]-1;
//					  dpiCurrent.x=IRSensor.stageDPI[IRSensor.dpiStage].x;
//					  dpiCurrent.y=IRSensor.stageDPI[IRSensor.dpiStage].y;
//					  setResolution(dpiCurrent.x,dpiCurrent.y);
//					}else{
//						protocolTransmit[STATUS] = COMMAND_ERROR;        // stage out of the range
//					}
//				}else if(ptr[0]==user.PClass){
//					if((ptr[1]>0)&&(ptr[1]<=user.profile[user.ProNO].sensor.maxStages)){
//						IRSensor.dpiStage = user.profile[user.ProNO].sensor.dpiStage =ptr[1]-1;
//						dpiCurrent.x=IRSensor.stageDPI[IRSensor.dpiStage].x;
//						dpiCurrent.y=IRSensor.stageDPI[IRSensor.dpiStage].y;
//						user.profile[user.ProNO].activeDpi.x=dpiCurrent.x;
//						user.profile[user.ProNO].activeDpi.y=dpiCurrent.y;
//						setResolution(dpiCurrent.x,dpiCurrent.y);
//						requestVariableUpdate(SW_CHANGEDPr,user.ProNO);              // Generic variable storage needs updating
//					}else{
//						protocolTransmit[STATUS] = COMMAND_ERROR;        // stage out of the range
//					}						
//				}else{
//					if((ptr[1]>0)&&(ptr[1]<=user.profile[ptr[0]-1].sensor.maxStages)){
//					  user.profile[ptr[0]-1].sensor.dpiStage=ptr[1]-1;
//					  user.profile[ptr[0]-1].activeDpi.x=user.profile[ptr[0]-1].sensor.stageDPI[ptr[1]-1].x;
//					  user.profile[ptr[0]-1].activeDpi.y=user.profile[ptr[0]-1].sensor.stageDPI[ptr[1]-1].y;
//					  requestVariableUpdate(SW_CHANGEDPr,(ptr[0]-1));              // Generic variable storage needs updating	
//					}else{
//							protocolTransmit[STATUS] = COMMAND_ERROR;        // stage out of the range
//					}
//				}
//			break;
//    case (GET | DPI_PROFILE_DPI):                         // Return 7 bytes  [Profile Active DPI Values]
//			if(ptr[0] == 0){
//			  ptr[1] = (U8)(dpiCurrent.x >> 8);
//        ptr[2] = (U8)(dpiCurrent.x);          
//        ptr[3] = (U8)(dpiCurrent.y >> 8);
//        ptr[4] = (U8)(dpiCurrent.y);
//			}else{
//		    ptr[1] = user.profile[ptr[0] - 1].activeDpi.x>>8;
//		    ptr[2] = user.profile[ptr[0] - 1].activeDpi.x;
//			  ptr[3] = user.profile[ptr[0] - 1].activeDpi.y>>8;
//		    ptr[4] = user.profile[ptr[0] - 1].activeDpi.y;
//			}
//    break;

//    case (SET | DPI_PROFILE_DPI):                         // Profile Active DPI Values
//		  if (ptr[0] != 0 ) {                                 // cureent profile
//			  user.profile[ptr[0] - 1].activeDpi.x = ((U16)ptr[1] << 8) | ptr[2];
//			  user.profile[ptr[0] - 1].activeDpi.y = ((U16)ptr[3] << 8) | ptr[4];
//      }
//			if ((user.PClass == ptr[0]) || (ptr[0] == 0 ))  {  // if same class ,then set; or different class, only unpdate
//				 dpiCurrent.x = ((U16)ptr[1] << 8) | ptr[2];
//         dpiCurrent.y = ((U16)ptr[3] << 8) | ptr[4];
//         setResolution(dpiCurrent.x,dpiCurrent.y);
//			}
//			if ( ptr[0] != 0)	
//				requestVariableUpdate(SW_CHANGEDPr,ptr[0] - 1);              // Generic variable storage needs updating
//   	break;
//			case (GET |DPI_MAX_SATGES):
//				if(ptr[0] == 0 ){
//					ptr[2]=IRSensor.maxStages;
//				}else if(ptr[0]<=profileNO){
//					ptr[2]=user.profile[ptr[0] - 1].sensor.maxStages;
//				}
//				break;
//		case (SET |DPI_MAX_SATGES):
//		 if(ptr[1]>0&&ptr[1]<=NUMBER_OF_STAGES){
//			if (ptr[0] == 0 ) {                                  // cureent profile
//				IRSensor.maxStages=ptr[1];
//				if(IRSensor.dpiStage>IRSensor.maxStages-1){
//					IRSensor.dpiStage=IRSensor.maxStages-1;
//					dpiCurrent.x=IRSensor.stageDPI[IRSensor.dpiStage].x;
//					dpiCurrent.y=IRSensor.stageDPI[IRSensor.dpiStage].y;
//					setResolution(dpiCurrent.x,dpiCurrent.y);
//				}
//			}else{
//				user.profile[ptr[0] - 1].sensor.maxStages=ptr[1];
//				if(user.profile[ptr[0] - 1].sensor.dpiStage>user.profile[ptr[0] - 1].sensor.maxStages-1){
//					user.profile[ptr[0] - 1].sensor.dpiStage=user.profile[ptr[0] - 1].sensor.maxStages-1;
//					user.profile[ptr[0]-1].activeDpi.x=user.profile[ptr[0]-1].sensor.stageDPI[ptr[1]-1].x;
//					user.profile[ptr[0]-1].activeDpi.y=user.profile[ptr[0]-1].sensor.stageDPI[ptr[1]-1].y;
//					if(ptr[0]==user.PClass){                                    // Current class
//						IRSensor.maxStages=user.profile[ptr[0] - 1].sensor.maxStages;
//						IRSensor.dpiStage=user.profile[ptr[0] - 1].sensor.dpiStage;
//						dpiCurrent.x=user.profile[ptr[0]-1].activeDpi.x;
//					  dpiCurrent.y=user.profile[ptr[0]-1].activeDpi.y;
//					  setResolution(dpiCurrent.x,dpiCurrent.y);
//					}
//				}
//			  requestVariableUpdate(SW_CHANGEDPr,(ptr[0]-1));              // Generic variable storage needs updating	
//			}
//		}else{
//			protocolTransmit[STATUS] = COMMAND_ERROR;        // stage out of the range
//		}
//			break;
//    default:
//      protocolTransmit[STATUS] = COMMAND_ERROR;           // Not support cmd, return error to SW
//    break;
//  }
}

void processProfileCommand(void) 
{
//  U8 *ptr = &protocolTransmit[PAYLOAD];                   // Reference first byte of Protocol Payload
//  U8 i,j;
//	U8 tmp;
//	switch (protocolTransmit[COMMAND_ID]) {
//		case (GET | PROFILE_NUMBER):
//		   ptr[0] = getProfileNumber();
//	  break;
//		
//		case (GET | PROFILE_ID):
//			ptr[0]=profileNO;
//			for(i=0,j=1;i<8;i++){
//				if((user.aliveProfile>>i)&0x01){
//					ptr[j++]=i;
//				}
//			}
//		  protocolTransmit [DATA_SIZE] = j;
//		break;
//		case (SET|PROFILE_NEW):
//			if(!newProfile(ptr[0])){
//				protocolTransmit[STATUS] = COMMAND_ERROR;           // Not support cmd, return error to SW
//			}else{
//			 requestVariableUpdate(SW_CHANGEDPr,user.ProNO);
//			}
//			break;
//		case (SET|PROFILE_DEL):
//			if(!deleteProfile(ptr[0])){
//				protocolTransmit[STATUS] = COMMAND_ERROR;           // Not support cmd, return error to SW
//			}else{
//			 requestVariableUpdate(SW_CHANGEDPr,user.ProNO);
//			}
//     break;
//		case (GET | PROFILE_ACTIVE):
//			ptr[0] = user.PClass;
//		break;
//		case (SET | PROFILE_ACTIVE):
//			if(ptr[0]>=1){
//				if(user.PClass != ptr[0]){					
//					if(switchProfile(PROFILE_NUM,ptr[0])){
//		        requestVariableUpdate(SW_CHANGEDPr,user.ProNO);
//					}else{
//						protocolTransmit[STATUS] = COMMAND_ERROR;   
//					}
//				}
//			}else{
//				protocolTransmit[STATUS] = COMMAND_ERROR;           // Not support cmd, return error to SW
//			}
//		break;
////		case (GET|PROFILE_NAME_MASS):                           // Profile name size more then 90 bytes
////			tmp=protocolTransmit[DATA_SIZE];                      // 
////		  if(!getProfileName(ptr[0],&ptr[3],B2W(ptr[1],ptr[2]),&ptr[5],tmp-5)){
////				 protocolTransmit[STATUS] = COMMAND_ERROR;           // Not support cmd, return error to SW
////			}       
////			break;
////		case (SET|PROFILE_NAME_MASS):                           // Set profile name mass 
////			tmp=protocolTransmit[DATA_SIZE];                      // 
////			tmp=setProfileName(ptr[0],B2W(ptr[3],ptr[4]),B2W(ptr[1],ptr[2]),&ptr[5],tmp-5);
////			if(tmp==RETURN_WAIT){
////				protocolWait=TRUE;
////			}else if(tmp==RETURN_FAIL){
////				protocolTransmit[STATUS] = COMMAND_ERROR;             // Not support cmd, return error to SW
////				protocolWait=FALSE;
////			}else{
////				protocolWait=FALSE;
////			}
////			break;
//		case (GET|PROFILE_MAX):
//			ptr[0]=profileNO;
//			break;
//		default:
//      protocolTransmit[STATUS] = COMMAND_ERROR;           // Not support cmd, return error to SW
//    break;
//	}
}




void processProximityCommand(void)
/*++

Function Description:
  This function processes the sensor calibration commands. test version
 
Arguments:
  NONE

Returns: 
  NONE

--*/
{
  U8 *ptr = &protocolTransmit[PAYLOAD];                   // Reference first byte of Protocol Payload
  switch (protocolTransmit[COMMAND_ID]) {
   
//		case (GET | PROXIMITY_SENSOR_LST):                    // Return 1 bytes [calibration control setting]
//		  ptr[0] = 1;
//			ptr[1] = 4;                                         //  Avago
//    break;

		case (GET | SENSOR_STATUS):
//      if (ptr[1] == MOUSE_SENSOR_ID) {
//        ptr[2] = calEnable;                            // profile 0 or same profile, return current ctrl
//        calStatus = CAL_S_OFF;
//      } else {
//        protocolTransmit[STATUS] = COMMAND_ERROR;          // if not Avago sensor, then...
//      }

        ptr[0] = calEnable;                            // profile 0 or same profile, return current ctrl
        ptr[1] = calStatus;
  
		break;
			
		case (SET | SENSOR_STATUS):
//      if (ptr[1] == MOUSE_SENSOR_ID) {
// //       calEnable = ptr[2];
//        if (ptr[2] == ENABLE) {
//          calEnable = ptr[2];
//          DisableCalibraiton = 0;
//        } else if (ptr[2] == DISABLE) {
//			    DisableCalibraiton = 1;
//        }
//      } else {
//        protocolTransmit[STATUS] = COMMAND_ERROR;         // if not Avago sensor, then...
//      }
       calEnable =  ptr[0];                            // profile 0 or same profile, return current ctrl
       calStatus =  ptr[1];    
		break;

		case (GET | SENSOR_THRESHOLD):
//		  ptr[2] = CAL_LiftCal2;
//			ptr[3] = CAL_LiftCal3;
//			ptr[4] = CAL_LiftConfig;
//			ptr[5] = 0;
//			ptr[6] = 0;
//			ptr[7] = 0;
//			ptr[8] = 0;
//      ptr[9] = 0;
//		  ptr[10] = CAL_LiftCal4;                              // manual calibration value
		  ptr[0] = CAL_LiftCal2;
			ptr[1] = CAL_LiftCal3;    
		break;
		case (SET | SENSOR_THRESHOLD):
      CAL_LiftCal2 = ptr[2];
      CAL_LiftCal3 = ptr[3];     
//      CAL_LiftCal2 = ptr[2];
//      CAL_LiftCal3 = ptr[3];
//      CAL_LiftConfig = ptr[4];
//      CAL_Threshold = ptr[9];
//      if ((calEnable == ENABLE) && (ptr[1] == MOUSE_SENSOR_ID)) {    // state is enable
//     if (calEnable == ENABLE) {   
//        calStatus = CAL_S_OFF;
        CalibrationDate(CAL_LiftCal2,CAL_LiftCal3,CAL_LiftConfig,CAL_Threshold);
//			} else {
//				protocolTransmit[STATUS] = COMMAND_ERROR;          //
//			}
		break;

		case (GET | SENSOR_CONFIGURATION):
        ptr[0] = user.profile[0].sensor.angleTune;                            // profile 0 or same profile, return current ctrl
        ptr[1] = user.profile[0].sensor.liftDis; 
        
  
		break;
			
		case (SET | SENSOR_CONFIGURATION):
       if (ptr[0]) {
         user.profile[0].sensor.angleTune =  ptr[0];                            // profile 0 or same profile, return current ctrl
         SPI_W_2BYTE(REG_ANGLE_TUNE,ptr[0]);                 // write register 0x65 data to register 0x2C
       }
       if (ptr[0] == 3) {
         user.profile[0].sensor.liftDis =  ptr[1];
         SPI_W_2BYTE(REG_LIFT_CONFIG,ptr[1]);
       }         
		break;     
//		case (SET | PROXIMITY_SENSOR_CTRL):
//			if ((user.PClass == ptr[0]) || (ptr[0] == 0)) {
//				if (ptr[1] == MOUSE_SENSOR_ID) {                         //correct sensor
//					if (ptr[2] == 0) {                                     // For manual calibration
//						 if(ptr[3] == 0){
//						  calStatus = CAL_S_PREPARE;                           // type 0, start maunal calibration
//						  CAL_LiftCal2 = 0;
//						  CAL_LiftCal3 = 0;
//						  CAL_LiftConfig = 0;
//						  CAL_LiftCal4 = 0;
//						 }else if(ptr[3] == 1){
//							 calStatus=CAL_S_END;
//						 }
//					}
//				}
//			} else {
//				protocolTransmit[STATUS] = COMMAND_ERROR;                         //
//			}
//		break;

//		case (GET | PXM_LIFT_LED_INDICATOR):
//			if (ptr[1] == MOUSE_SENSOR_ID){
//				if ((user.PClass == ptr[0]) || (ptr[0] == 0)) {					
//					ptr[2] =  user.lift_off_dim;
//				} else {
//					ptr[2] =  user.lift_off_dim;
//				}
//				
//			} else {
//				protocolTransmit[STATUS] = COMMAND_ERROR;           // Not support cmd, return error to SW  
//			}

//		break;
//		case (SET | PXM_LIFT_LED_INDICATOR):
//			if(ptr[1] == MOUSE_SENSOR_ID) {
//				if ((user.PClass == ptr[0]) || (ptr[0] == 0)) {	
//				  user.lift_off_dim = ptr[2];
//				} else {
//					user.lift_off_dim = ptr[2];
//				}
//				requestVariableUpdate(SW_CHANGEDPr);
//			} else {
//				protocolTransmit[STATUS] = COMMAND_ERROR;           // Not support cmd, return error to SW 
//			}
//		break;
    default:
      protocolTransmit[STATUS] = COMMAND_ERROR;             // Not support cmd, return error to SW
    break;
	}
}

void processLedCommand(void)
/*++

	Function Description:
  		This function process the LED commands. This device has the standard keyboard
  		LEDs, which this command is not meant for. This command is to be used on the
  		Logo LED (ID = 0x04), Macro Record LED (ID = 0x07), and Game Mode LED (ID = 0x09).

         	Commands                LEDs => Logo     MacroRecord   GameMode
        LED State                         x              x           x
        LED Effect                        x              x           x
        LED Brightness                    x 
        LED Blinking Parameters           x              x           x
        LED Pulsating Parameters          x

  		If there is a LED state change, all of the variables are set here, but the actual
  		state of the LED will be changed on the next interrupt. Note that an LED reports
  		"on", even if it in a special effect during it "off" cycle. Therefore "off" means
  		nothing is happening to the LED, nor will it happen until another command occurs
  		to change that.

	Arguments:
  		NONE

	Returns: 
  		NONE

--*/
{
  U8 *ptr;//, *color;
  U8 i;
//  LED_BASIC_PARM *led;
////  LED_MAXM *cal;
////  LED_FADE *fx;
//	U8 table[5] = {20,30,40,50,80};
  ptr = &protocolTransmit[PAYLOAD];                   // Reference first byte of Protocol Payload
//  led = ledParameters(1, LED_BCKLIT_ID);
//  cal = ((ptr[0] == 0) ? &calibrating[led[0].index] : &sys.maxLight[led[0].index]);
//  fx  = &staging[led[0].index];

  switch (protocolTransmit[COMMAND_ID]) {
//  case (GET | LED_STATE):                                 // Return 3 bytes  [LED Class, LED ID, State]
//		if (ptr[1] == LED_GAME_ID) {
//			ptr[2] = gameMode;
//		} else if (ptr[1] == LED_MACRO_ID) {
//		  ptr[2] = macroLed;
//    } else {
//      ptr[2] = led[0].on;
//    }
//    break;
//  case (SET | LED_STATE):                                 // [LED Class, LED ID, State]
//		if (ptr[1] == LED_GAME_ID) {
//			gameMode = ptr[2];
//      kbIndicators = gameMode ? (kbIndicators | 0x80) : (kbIndicators & (~0x80));                       
//		} else if (ptr[1] == LED_MACRO_ID) {
//		  macroLed = ptr[2];
//      kbIndicators = ptr[2] ? (kbIndicators | 0x10) : (kbIndicators & (~0x10));                       
//    } else {
//      led[0].on = ptr[2];
//      led[0].change = TRUE;
//    }
//    break;  
  case (GET | LED_RGB_PARAM):                                // Return 3 bytes  [LED Class, LED ID, Effect]
    ptr[2] = user.profile[0].led[ptr[1]].color.r;
    ptr[3] = user.profile[0].led[ptr[1]].color.g;
    ptr[4] = user.profile[0].led[ptr[1]].color.b;
    break;
  case (SET | LED_RGB_PARAM):                                // Return 3 bytes  [LED Class, LED ID, Effect]
    user.profile[0].led[ptr[1]].color.r = ptr[2];
    user.profile[0].led[ptr[1]].color.g = ptr[3];
    user.profile[0].led[ptr[1]].color.b = ptr[4];
    LED_Mode_ReInit(ptr[1]);
     if (ptr[0]) {
       requestVariableUpdate(SW_CHANGEDPr,NULL);     
     }  
    break;
  case (GET | LED_BRIGHTNESS):                                // Return 3 bytes  [LED Class, LED ID, Effect]
   ptr[2] = user.profile[0].led[ptr[1]].bright;
    break;
  case (SET | LED_BRIGHTNESS):                                // Return 3 bytes  [LED Class, LED ID, Effect]
//    for (i= 0; i<NUMBER_OF_LEDS; i++ )
     user.profile[0].led[ptr[1]].bright = ptr[2];
     if (ptr[0]) {
       requestVariableUpdate(SW_CHANGEDPr,NULL);     
     }
    break;      
  case (GET | LED_EFFECT):                                // Return 3 bytes  [LED Class, LED ID, Effect]
    
    ptr[2] = user.profile[0].led[ptr[1]].effect;
    ptr[3] = 0;
    ptr[4] = user.profile[0].led[ptr[1]].speed;  
    ptr[5] = user.profile[0].led[ptr[1]].mode;     
//    switch (ptr[0]) {       
//      case WAVE:
//            ptr[2] = ram.effectParm.backlightupdateTime;
//        break;
//			case RIPPLEING:	
//            ptr[2] = ram.effectParm.backlightupdateTime;
//			      ptr[1] = ram.effectParm.waveAngle;
//        break;
//          case CONTROL:
//             ptr[2] = ram.effectParm.backlightupdateTime;
//            break; 
//          default:
//           ram.effectParm.backlightupdateTime = ptr[2];
//            break;      
//        }    
    break;
  case (SET | LED_EFFECT): 
    
          user.profile[0].led[ptr[1]].effect  = bLED_Mode =  ptr[2];
//  				wLED_Status |= mskLED_ModeChange;	
          user.profile[0].led[ptr[1]].speed = bLED_DataRefreshTime_Reload[bLED_Mode][ptr[1]] = ptr[4];//table[ptr[2]];
          user.profile[0].led[ptr[1]].mode =  ptr[5];
          LED_Mode_ReInit(ptr[1]);
        //[s 
//          flash_buttfer[1] = bLED_Mode;
//          FLASH_ProgramPage(0x7000,1,(uint32_t*)flash_buttfer);
        //]
		// [LED Class, LED ID, Effect]
//	  led[0].on =1 ;
//   	
//    switch (ptr[0]) {

//	     case 0:
//    //				if (ptr[0] == 0) 
//           led[0].bgdEffect = STATIC;
//        led[0].on = 0;
//        break;
//    //	     case STATIC:
//    //				if (ptr[0] == ON) 
//    //        led[0].on = OFF;
//    //        break;        
//      case WAVE:
//            ram.effectParm.backlightupdateTime = ptr[2];
////			  ram.effectParm.waveDir = ptr[1]; 
//        break;
//          case PAUSE:
//            lightingPause ^= 1;
//            break;
//			case RIPPLEING:	
//            ram.effectParm.backlightupdateTime = ptr[2];
//			  ram.effectParm.waveAngle = ptr[1];
//        break;
//    //      case FIRE:
//    //        ram.effectParm.backlightupdateTime = ptr[2];
//    //        break;
//    //      case RANDOM_COLOR:
//    //        ram.effectParm.backlightupdateTime = ptr[2];
//    //        break;
//    //      case BREATH:
//    //      case BREATH_2CORLOR:
//    //      case BREATH_RANDOM:
//    //        ram.effectParm.backlightupdateTime = ptr[2];
//    //        break;
//    //      case SPECTRUM:
//    //        ram.effectParm.backlightupdateTime = ptr[2];
//    //        break;
//    //      case STRIKE:
//    //			case CUSTOMIZE:	
//    //        ram.effectParm.backlightupdateTime = ptr[2];
//    //        break;
//          case CONTROL:
//            ram.effectParm.backlightupdateTime = ptr[2];
//            break; 
//          default:
//           ram.effectParm.backlightupdateTime = ptr[2];
//            break;      
//        }
//        if (ptr[0] != PAUSE) {
//        
//        if ((led[0].bgdEffect != ptr[0]) || (ram.effectParm.waveDir != ptr[1]) || (ram.savedLight[0].backlightMode != ptr[3])) {
//          ram.savedLight[0].backlightMode = ptr[3];
//          ram.effectParm.waveDir = ptr[1]; 
//        forceLightingReload();                                  // Force a reload & refresh of all LEDs    
//        setBgdLedEffect(led, ptr[0]);
//        }
//        }
        requestVariableUpdate(SW_CHANGEDPr,NULL);     
        break;
  case (GET | DPI_LED_COLOR):                                // Return 3 bytes  [LED Class, LED ID, Effect]
    memcpy(&ptr[2],user.profile[0].sensor.stageColor,20);
    break;
  case (SET | DPI_LED_COLOR):                                // Return 3 bytes  [LED Class, LED ID, Effect]
    for (i= 0; i<NUMBER_OF_LEDS; i++ )
     memcpy(user.profile[0].sensor.stageColor,&ptr[2],20);
     if (ptr[0]) {
       requestVariableUpdate(SW_CHANGEDPr,NULL);     
     }
    break;  
// case (SET | LED_SOFTWARE_CONTROL): 
//    if (lighting[0].bgdEffect == CONTROL) {
////      for (i=0; i<(protocolTransmit[DATA_SIZE]-4)/3; i++) {
////        if ((ptr[2] < mX) && ((ptr[3]+i) < mY) && ((6+(i*3)) <= 58)) {
////          bckMatrix[ptr[2]][ptr[3]+i].r = ptr[4+(i*3)];
////          bckMatrix[ptr[2]][ptr[3]+i].g = ptr[5+(i*3)];
////          bckMatrix[ptr[2]][ptr[3]+i].b = ptr[6+(i*3)];
////        }
////      } 
//        for (i=0; i<ptr[0]; i++) {
//          for (j=0; j<ptr[1]; j++ ) {
//            bckMatrix[i][j].r = ptr[2+(i*3*ptr[1])+(j*3)];
//            bckMatrix[i][j].g = ptr[3+(i*3*ptr[1])+(j*3)];
//            bckMatrix[i][j].b = ptr[4+(i*3*ptr[1])+(j*3)];             
//          }
//        }
//    } else if (lighting[0].bgdEffect == CUSTOMERDEFINE) {
////      for (i=0; i<(protocolTransmit[DATA_SIZE]-4)/3; i++) {
////        if ((ptr[2] < mX) && ((ptr[3]+i) < mY) && ((6+(i*3)) <= 58)) {
////          ram.userMatrix[ptr[2]][ptr[3]+i].r = ptr[4+(i*3)];
////          ram.userMatrix[ptr[2]][ptr[3]+i].g = ptr[5+(i*3)];
////          ram.userMatrix[ptr[2]][ptr[3]+i].b = ptr[6+(i*3)];
////        }
////      } 
//        for (i=0; i<ptr[0]; i++) {
//          for (j=0; j<ptr[1]; j++ ) {
//            ram.userMatrix[i][j].r = ptr[2+(i*3*ptr[1])+(j*3)];
//            ram.userMatrix[i][j].g = ptr[3+(i*3*ptr[1])+(j*3)];
//            ram.userMatrix[i][j].b = ptr[4+(i*3*ptr[1])+(j*3)];             
//          }
//        }      
//     requestVariableUpdate(SW_CHANGED);        
//    }     
//        break;
// case (GET | LED_SOFTWARE_CONTROL): 
//     ptr[0] = mX;
//     ptr[1] = mY;
//        break;
//   case (GET | LED_DYN_EFFECT):                                // Return 3 bytes  [LED Class, LED ID, Effect]
//   ptr[0] = led[0].dynEffect;
//   ptr[1] = ram.effectParm.inputdateTime;
//   if (ram.savedLight[0].inputcolorIdex == 8) {
//     ptr[2] = 1;
//   } else {
//     ptr[2] = 0;
//   }
//        break;
// case (SET | LED_DYN_EFFECT): 
//     ram.savedLight[0].dynEffect = led[0].dynEffect = ptr[0];
//     ram.effectParm.inputdateTime = ptr[1];
//     if (ptr[2] == 1) {
//       ram.savedLight[0].inputcolorIdex = 8;
//     } else {
//       ram.savedLight[0].inputcolorIdex = 0;
//     }
//     memorySet((void *)frcMatrix, 0x00, sizeof(frcMatrix));
//     memorySet((void *)dynamic, 0x00, sizeof(dynamic));  // clear all dynamic effects
//     requestVariableUpdate(SW_CHANGED);
//      break;
//  case (GET | INPUT_COLOR):                                // Return 3 bytes  [LED Class, LED ID, Effect]
//    ptr[2] = ram.effectParm.dynColor.r;
//    ptr[3] = ram.effectParm.dynColor.g;
//    ptr[4] = ram.effectParm.dynColor.b;
//    break;
//  case (SET | INPUT_COLOR):                                // Return 3 bytes  [LED Class, LED ID, Effect]
//    ram.effectParm.dynColor.r = ptr[2];
//    ram.effectParm.dynColor.g = ptr[3];
//    ram.effectParm.dynColor.b = ptr[4];
//    if (ptr[0]) {
//      requestVariableUpdate(SW_CHANGED);     
//    } 
//    break; 
//  case (GET | INPUT_BRIGHTNESS):                                // Return 3 bytes  [LED Class, LED ID, Effect]
//   ptr[1] = ram.effectParm.inputLevel;
//        break;
//  case (SET | INPUT_BRIGHTNESS):                                // Return 3 bytes  [LED Class, LED ID, Effect]
//     ram.effectParm.inputLevel = ptr[1];
//     if (ptr[0]) {
//       requestVariableUpdate(SW_CHANGED);     
//     }  
//    break;
  default:
    protocolTransmit[STATUS] = COMMAND_NOTSUPPORT;        // Not support cmd, return error to SW
    break;
  }
  if (((protocolTransmit[COMMAND_ID]) & 0x80) == 0) {
//    if ((*(ptr + 0) != 0) && ((LED_GAME_ID == *(ptr + 1)) || (LED_MACRO_ID == *(ptr + 1)) || (LED_BCKLIT_ID == *(ptr + 1)))) {                                // If 'Set' command and not Class #0, then...
//      copyLighting2Active(led[0].id);                     // Copy data to 'active' lighting structure 
//      setBgdLedEffect(&lighting[0], lighting[0].bgdEffect);
//      if (protocolTransmit[COMMAND_ID] == (SET | LED_RGB_MAXIMUM)) {
//        requestVariableUpdate(FACTORY_CHANGED);           // Special variable storage needs updating
//      }
//      else {
//        requestVariableUpdate(SW_CHANGED);                // Generic variable storage needs updating
//      }
//			
//			if (protocolTransmit[COMMAND_ID] == (SET | LED_RGB_PARAM)) {
//        lighting[led[0].index].change = TRUE;             // For class1 command
//			}	
//    }
  }
}
//void processChromaCommand (void) 
//{

//  U8 *ptr = &protocolTransmit[PAYLOAD];                   // Reference first byte of Protocol Payload
//  U8 i=0,j;
//  U8 CommandMaking = 0;
//  CHROMA_PARM *led,*ledcomp;
//  
//  CommandMaking = protocolTransmit[COMMAND_ID] & 0x7F;    // include GET and SET command
//  if ((CommandMaking == CHROMA_EFFECT_ID) || (CommandMaking == CHROMA_CALIBRATION_DATA)) {
//    if ((ptr[0] == CHROMA_LOGO_ID) ||  (ptr[0] == CHROMA_WHEEL_ID)||(ptr[0] == CHROMAR_RS_ID)||((ptr[0] == CHROMAR_LS_ID))) { // for lily mouse ,supprot logo and wheel ID
//      led = ledParameters(1, ptr[0]);                     // Reference 'user' LED parameters
//      ledcomp = ledParameters(1, lighting[0].id);         // actually not use in this case,if detele,Keil reports warning
//    }else{
//  	  protocolTransmit[STATUS] = COMMAND_ERROR;           // GET the error ID, return error to SW
//      return;
//    } 
//  } else if ((CommandMaking == CHROMA_EFFECT) || (CommandMaking == CHROMA_FRAME) || (CommandMaking == CHROMA_BRIGHTNESS)) {
//    if ((ptr[1] == CHROMA_LOGO_ID) ||  (ptr[1] == CHROMA_WHEEL_ID)||(ptr[1] == CHROMAR_RS_ID)||((ptr[1] == CHROMAR_LS_ID))) { // for lily mouse ,supprot logo and wheel ID
//      led = ledParameters(ptr[0], ptr[1]);                // Reference 'active' LED parameters
//      ledcomp = ledParameters(ptr[0], lighting[0].id);    // actually not use in this case,if detele,Keil reports warning
//    } else if (ptr[1] == CHROMA_ALL_ID) {                 // ALL region ID,ledcomp
//      led = ledParameters(ptr[0], CHROMA_LOGO_ID);        // actually not use in this case,if detele,Keil reports warning
//      ledcomp = ledParameters(ptr[0], lighting[0].id);    // get the address of the first lighting index
//    } else if(ptr[1] == CHROMA_UNDERGLOW_ID){             // Using underglow led for profile led for white blance test
//			profileLedComand(CHROMA_UNDERGLOW_ID);              // Change the color not matter what effect 
//			return ;                                            // After finish the profile command ,return , profile led doesn't need to save 
//		}else{
//       protocolTransmit[STATUS] = COMMAND_ERROR;           // GET the error ID,return error to SW
//      return;
//    }
//  } else if(CommandMaking == CHROMA_REGION_ID){          // Get reginon id doesn't need any limit
//		led = ledParameters(ptr[0], CHROMA_LOGO_ID);         // actually not use in this case,if detele,Keil reports warning
//		ledcomp = ledParameters(ptr[0], CHROMA_LOGO_ID);     // actually not use in this case,if detele,Keil reports warning
//	} else{
//     protocolTransmit[STATUS] = COMMAND_ERROR;           // Not support cmd, return error to SW
//     return;
//  }

//	switch (protocolTransmit[COMMAND_ID]) {
//		
//		case (GET | CHROMA_REGION_ID):                        // get Chroma region ID list
//			i = 0;
//		  *(ptr + (i++)) = CHROMA_WHEEL_ID;                   // Region ID Scroll wheel LED
//		  *(ptr + (i++)) = 25;                                // Update Rate
//		  *(ptr + (i++)) = 0x03;                              // Data Type  RGB LED (RGB format)
//		  *(ptr + (i++)) = 1;                                 // Array X
//		  *(ptr + (i++)) = 1;                                 // Array Y

//		  *(ptr + (i++)) = CHROMA_LOGO_ID ;                   // Region ID logo LED
//		  *(ptr + (i++)) = 25;                                // Update Rate
//		  *(ptr + (i++)) = 0x03;                              // Data Type  RGB LED (RGB format)
//		  *(ptr + (i++)) = 1;                                 // Array X
//		  *(ptr + (i++)) = 1;                                 // Array Y
//		
//			*(ptr + (i++)) = CHROMAR_RS_ID;                     // Region ID logo LED
//		  *(ptr + (i++)) = 25;                                // Update Rate
//		  *(ptr + (i++)) = 0x03;                              // Data Type  RGB LED (RGB format)
//		  *(ptr + (i++)) = 1;                                 // Array X
//		  *(ptr + (i++)) = 7;                                 // Array Y
//		
//			*(ptr + (i++)) = CHROMAR_LS_ID;                     // Region ID logo LED
//		  *(ptr + (i++)) = 25;                                // Update Rate
//		  *(ptr + (i++)) = 0x03;                              // Data Type  RGB LED (RGB format)
//		  *(ptr + (i++)) = 1;                                 // Array X
//		  *(ptr + (i++)) = 7;                                 // Array Y
//		  protocolTransmit [DATA_SIZE] = i;
//		break;

//		case (GET | CHROMA_EFFECT_ID):                        // get Chroma Effect ID list
//		  i = 0;
//		  if((*(ptr + 0)==CHROMA_WHEEL_ID)||(*(ptr + 0)==CHROMA_LOGO_ID)){
//				*(ptr + (i++)) = *(ptr + 0);                        // region ID
//		    *(ptr + (i++)) = 0x00;                              // EFX_OFF
//			  *(ptr + (i++)) = 0x01;                              // EFX_STATIC
//		    *(ptr + (i++)) = 0x02;                              // EFX_BREATHE
//			  *(ptr + (i++)) = 0x03;                              // EFX_SPECTRUM
//		    *(ptr + (i++)) = 0x05;                              // EFX_REACTIVE
//		    *(ptr + (i++)) = 0x08;                              // EFX_CUSTOM_FRAME
//			}else if((*(ptr + 0)==CHROMAR_RS_ID)||(*(ptr + 0)==CHROMAR_LS_ID)){
//				*(ptr + (i++)) = *(ptr + 0);                        // region ID
//		    *(ptr + (i++)) = 0x00;                              // EFX_OFF
//			  *(ptr + (i++)) = 0x01;                              // EFX_STATIC
//		    *(ptr + (i++)) = 0x02;                              // EFX_BREATHE
//			  *(ptr + (i++)) = 0x03;                              // EFX_SPECTRUM
//				*(ptr + (i++)) = 0x04;                              // EFX_WAVE
//		    *(ptr + (i++)) = 0x05;                              // EFX_REACTIVE
//		    *(ptr + (i++)) = 0x08;                              // EFX_CUSTOM_FRAME
//			}
//			protocolTransmit [DATA_SIZE] = i;
//		break;
//		
//		
//	  case (GET | CHROMA_EFFECT):                           // get Chroma effect
//			i = 0;
// 			*(ptr + (i++)) =  0;                                // profile
//		  *(ptr + (i++))  =  led[0].id;                       // region ID
//		  *(ptr + (i++))  =  led[0].pendingeffect;            // Effect ID
//		  *(ptr + (i++))  =  led[0].Chroma_effect.flags;      // flags
//		  *(ptr + (i++))  =  led[0].Chroma_effect.rate;       // Rate
//		  *(ptr + (i++))  =  led[0].Chroma_effect.colorNO;    // #color
//		  *(ptr + (i++))  =  led[0].Chroma_effect.r;          // r
//		  *(ptr + (i++))  =  led[0].Chroma_effect.g;          // g
//		  *(ptr + (i++))  =  led[0].Chroma_effect.b;          // b
//		  *(ptr + (i++))  =  led[0].Chroma_effect.rr;         // rr
//		  *(ptr + (i++))  =  led[0].Chroma_effect.gg;         // gg
//		  *(ptr + (i++))  =  led[0].Chroma_effect.bb;         // bb
//			protocolTransmit [DATA_SIZE] = i;
//		break;
//		
//		case (SET |CHROMA_EFFECT):		                        // Set Chroma effect
//		  switch (ptr[2]) {                                   // effect ID
//        case CHROMA_EFX_OFF:                              // effect ID: 0x00 EFX_OFF
//			  case CHROMA_EFX_STATIC:                           // effect ID: 0x01 EFX_STATIC
//        case CHROMA_EFX_BREATHE:                          // effect ID: 0x02 EFX_BREATHE
//				case CHROMA_EFX_SPECTRUM:                         // effect ID: 0x03 EFX_SPECTRUM
//				case CHROMA_EFX_WAVE:                             // effect ID: 0x04 EFX_WAVE
//        case CHROMA_EFX_REACTIVE:                         // effect ID: 0x05 EFX_REACTIVE
//			  case CHROMA_EFX_CUSTOM_FRAME:
//          if ((user.PClass == ptr[0]) || (ptr[0] == 0)) {// current profile or profile 0
//            if (ptr[1] == 0) {                            // ptr[1]: region Id == All region then
//              for (i = 0; i < NUMBER_OF_LEDS; i++) {
//                updateLEDeffectParm (ledcomp[i].id);
//              }
//            } else {                                      // signle region then
//              updateLEDeffectParm(ptr[1]);                // ptr[2] = effect id
//            }
//						if(ptr[0] != 0){
//						 requestVariableUpdate(SW_CHANGEDPr,ptr[0]-1);// unpdate user
//						}
//          } else {                                        // others profile, only update
//            if (ptr[1] == 0) {                            // ptr[1]: region Id all
//              for (i=0; i < NUMBER_OF_LEDS; i++) {
//               CopyOthersProfileParm(i);                  // update all region ID
//              }
//            } else {                                      // region ID
//              i = 0;
//							while ((user.profile[ptr[0]-1].savedLight[i].id != ptr[1] )&& (i < NUMBER_OF_LEDS)) {
//								i++;
//							}
//              CopyOthersProfileParm(i);                   // uptat effect id
//            }
//						requestVariableUpdate(SW_CHANGEDPr,ptr[0]-1);              // unpdate user
//          }
//				break;
//				// ]
//		    default:
//          protocolTransmit[STATUS] = COMMAND_ERROR;       // Not support cmd, return error to SW
//        break;
//		  }
//			
//		 	if (ptr[0] != 0) {                                  // if ptr 0, don't copy to user and don't unpdate EEPROM
//				if (user.PClass == ptr[0]) {
//          if (ptr[1] == CHROMA_ALL_ID) {
//            for (i=0; i < NUMBER_OF_LEDS; i++) {
//              copyLighting2Active(&ledcomp[i]);
//						 if((ptr[2]==CHROMA_EFX_WAVE)&&(previousEffect[ledcomp[i].index]==CHROMA_EFX_WAVE)){
//				       lighting[ledcomp[i].index].effect=CHROMA_EFX_WAVE;
//				       lighting[ledcomp[i].index].extendeffect=NO_EFFECT;
//							 memcpy(waveCycleBuffer[ledcomp[i].index],waveBuffer,sizeof(waveBuffer));
//			       }
//            }
//					} else {
//            copyLighting2Active(led);
//						if((ptr[2]==CHROMA_EFX_WAVE)&&(previousEffect[led[0].index]==CHROMA_EFX_WAVE)){
//				      lighting[led[0].index].effect=CHROMA_EFX_WAVE;
//				      lighting[led[0].index].extendeffect=NO_EFFECT;
//							memcpy(waveCycleBuffer[led[0].index],waveBuffer,sizeof(waveBuffer));
//			      }
//					}
//				}
//		  }else{                                     // If ptr 0, need to deal the wave effect
//				  if (ptr[1] == CHROMA_ALL_ID) {
//            for (i=0; i < NUMBER_OF_LEDS; i++) {
//						 if((ptr[2]==CHROMA_EFX_WAVE)&&(previousEffect[ledcomp[i].index]==CHROMA_EFX_WAVE)){
//				       lighting[ledcomp[i].index].effect=CHROMA_EFX_WAVE;
//				       lighting[ledcomp[i].index].extendeffect=NO_EFFECT;
//							 memcpy(waveCycleBuffer[ledcomp[i].index],waveBuffer,sizeof(waveBuffer));
//			       }
//            }
//					} else {
//						if((ptr[2]==CHROMA_EFX_WAVE)&&(previousEffect[led[0].index]==CHROMA_EFX_WAVE)){
//				      lighting[led[0].index].effect=CHROMA_EFX_WAVE;
//				      lighting[led[0].index].extendeffect=NO_EFFECT;
//							memcpy(waveCycleBuffer[led[0].index],waveBuffer,sizeof(waveBuffer));
//			      }
//					}
//			}
//		break;
//    //]
//		//[ Frame
//		case (SET | CHROMA_FRAME):
//			if ((ptr[2] == 0xFE) && (ptr[3] == 0xFE) && (ptr[4] == 0xFE)) {  // clear 
//        for (i=0; i < 16; i++) {
//          CHROMAFRAME[i].r = 0;
//				  CHROMAFRAME[i].g = 0;
//				  CHROMAFRAME[i].b = 0;
//        } 
//			} else {
//        j = 5;
//        for (i=0; i < 16; i++) {
//          CHROMAFRAME[i].r = ptr[j++];
//				  CHROMAFRAME[i].g = ptr[j++];
//				  CHROMAFRAME[i].b = ptr[j++];
//        }
//			}
//		break;
//		//]
//		//[ brightness
//		case (GET | CHROMA_BRIGHTNESS):
//      if(ptr[1] == 0){
//        protocolTransmit[STATUS] = COMMAND_ERROR;         // Not support cmd, return error to SW
//      }else{
//        ptr[2] = led[0].level;
//      }
//		  protocolTransmit [DATA_SIZE] = 3;			
//		break;
//		case (SET | CHROMA_BRIGHTNESS):
//       if (ptr[0] == 0) {                           // If the profile is profile 0 , no need                    
//         if(ptr[1] == 0){
//           for (i=0; i < NUMBER_OF_LEDS; i++) {
//             lighting[ledcomp[i].index].level = ptr[2]; 
//           }
//         } else {
//          led[0].level = ptr[2];
//         }       
//      }else if(ptr[0]==user.PClass){               // If the profile is current profile , need to saved and change 
//        if(ptr[1] == 0){
//           for (i=0; i < NUMBER_OF_LEDS; i++) {
//             lighting[led[i].index].level = ptr[2]; 
//             led[i].level = ptr[2];
//           }
//         } else {
//          lighting[led[0].index].level = ptr[2]; 
//          led[0].level = ptr[2];
//         }  
//         requestVariableUpdate(SW_CHANGEDPr,ptr[0]-1);         
//      }else{                                       // If the profile need to saved only
//       if (ptr[1] == 0) {                         
//          for (i=0; i < NUMBER_OF_LEDS; i++) {
//            led[i].level = ptr[2];
//          }
//        } else {
//          led[0].level = ptr[2];
//        }
//        requestVariableUpdate(SW_CHANGEDPr,ptr[0]-1);
//      }
//		break;
//		// ]
//		//[ Chroma calibration
////		case (GET | CHROMA_CALIBRATION_DATA):
////      ptr[1] = sys.maxLight[led[0].index].max.r;
////		  ptr[2] = sys.maxLight[led[0].index].max.g;
////		  ptr[3] = sys.maxLight[led[0].index].max.b;
////		  ptr[4] = sys.maxLight[led[0].index].max.v;
////			protocolTransmit [DATA_SIZE] = 5;		
////    break;

////    case (SET | CHROMA_CALIBRATION_DATA):
////      calibrating[led[0].index].max.r=sys.maxLight[led[0].index].max.r =  ptr[1];
////			calibrating[led[0].index].max.g=sys.maxLight[led[0].index].max.g =  ptr[2];
////			calibrating[led[0].index].max.b=sys.maxLight[led[0].index].max.b =  ptr[3];
////			calibrating[led[0].index].max.v=sys.maxLight[led[0].index].max.v =  ptr[4];
////		  
////			protocolTransmit [DATA_SIZE] = 5;
////			requestVariableUpdate(FACTORY_CHANGED,NULL);
////    break;		 
//		//]
//		
//	  default:
//      protocolTransmit[STATUS] = COMMAND_ERROR;             // Not support cmd, return error to SW
//    break;

//	}
//}
void updateLEDeffectParm (U8 led_id) 
{
  U8 *ptr = &protocolTransmit[PAYLOAD];                   // Reference first byte of Protocol Payload
	U8 i;
  CHROMA_PARM *led = ledParameters(ptr[0], led_id), *comp;// Reference 'active' LED parameters
  LED_FADE *fx;
	COLOR c={0,0,0,0};                                                // Temp color
  previousEffect[led[0].index]=lighting[led[0].index].effect;      // Backup the previous effect
  if (ptr[2] == CHROMA_EFX_OFF) {                         // set OFF,the effect need run the previous effect
    if (led[0].pendingeffect != CHROMA_EFX_OFF) {         // aviod two set EFX_OFF
      led[0].effect = led[0].pendingeffect;               // if running dissolve, effect is NO_EFFECT, when changed to EFX_OFF, make sure effect run last pendingefect and extendeffect is fade out
    }
  } else if (ptr[2] == CHROMA_EFX_REACTIVE) {
    led[0].effect = ptr[2];
  } else if (ptr[2] == CHROMA_EFX_CUSTOM_FRAME) {
    led[0].effect = ptr[2];                               // if reactive, off LED immediately
		led[0].extendeffect = NO_EFFECT;
  } else {
    led[0].effect = NO_EFFECT;                            // any effect changed (except fade out),then effect is NO_EFFECT
  }
  
  led[0].pendingeffect = ptr[2];                          // byte2; effect ID, pendingeffect
	if(led[0].pendingeffect!=CHROMA_EFX_OFF){               // Set to OFF doesn't need to change the flag and rate for keep current effect and fade out
    led[0].Chroma_effect.flags = ptr[3];                    // byte3: flages 
    led[0].Chroma_effect.rate = ptr[4];                     // byte4: rate
		if (ptr[4] == 1)     {TrailFadeoutTime[led[0].index]= 2;}// Trail time 2*255 = 0.5s   
    else if(ptr[4] == 2) {TrailFadeoutTime[led[0].index]= 4;}// Trail time 4*255 = 1s
    else if(ptr[4] == 3) {TrailFadeoutTime[led[0].index]= 6;}// Trail time 6*255 = 1.5s
    else if(ptr[4] == 4) {TrailFadeoutTime[led[0].index]= 8;}// Trail time 8*255 = 2s   
	}

  led[0].Chroma_effect.colorNO = ptr[5];                  // byte5: color (0~2)  
  if ((ptr[2] == CHROMA_EFX_BREATHE)||(ptr[2] == CHROMA_EFX_REACTIVE)||(ptr[2] == CHROMA_EFX_SPECTRUM)) { // if it a ramdom color selected
    comp = ledParameters(ptr[0], lighting[0].id);        // get the address of the first lighting index
    randomcolorIdex = randomGenerate(0,15);              // get new random color 
    for (i=0; i< NUMBER_OF_LEDS; i++) {                  // check for sync LED
			fx = &matrixStaging[i][0];                                    // get fx address
      if (comp[i].pendingeffect ==  ptr[2]) {            // if effect synced
        if (comp[i].Chroma_effect.colorNO == 0) {
          if (comp[i].id != led[0].id) {
            comp[i].Chroma_effect.r = randomList[randomcolorIdex].r;                // same effect, different ID,then sync the r/g/b
            comp[i].Chroma_effect.g = randomList[randomcolorIdex].g;
            comp[i].Chroma_effect.b = randomList[randomcolorIdex].b;
						c.r=comp[i].Chroma_effect.r;                                            // Temp color
						c.g=comp[i].Chroma_effect.g;
						c.b=comp[i].Chroma_effect.g;
						setStageOneColor(fx,NEW|RGB,c);                                          // Set .new  rgb to c 
          } else {
            ptr[6] = comp[i].Chroma_effect.r = randomList[randomcolorIdex].r;       // same ID,         
            ptr[7] = comp[i].Chroma_effect.g = randomList[randomcolorIdex].g;         
            ptr[8] = comp[i].Chroma_effect.b = randomList[randomcolorIdex].b;    
						c.r=comp[i].Chroma_effect.r;                                            // Temp color
						c.g=comp[i].Chroma_effect.g;
						c.b=comp[i].Chroma_effect.g;
						setStageOneColor(fx,NEW|RGB,c);                                         // Set .new to c 
          } 
          if (comp[i].pendingeffect != CHROMA_EFX_SPECTRUM) {
            randomFlag |= 1<<i;                                          // update corresponding sync flag
          } else {
            randomFlag &= ~(1<<i); 
          }
        } else {
          randomFlag &= ~(1<<i);                                       // update correcsponding unsync flag    
        }
//        copyLighting2Active(&comp[i]);                   // copy to active
        comp[i].pendingeffect = ptr[2];                  // byte2; effect ID, pendingeffect
        comp[i].Chroma_effect.flags = ptr[3];            // byte3: flages         
        
        if (ptr[2] == CHROMA_EFX_REACTIVE) {
          comp[i].effect = ptr[2];
          fx = &matrixStaging[comp[i].index][0];
          comp[i].Chroma_effect.v = comp[i].Chroma_effect.vv = 0;
					c.v=0;
					setStageOneColor(fx,MIX|V,c);                                         // Set .new to c 
        } else {
          comp[i].effect = NO_EFFECT;                    // any effect changed (except fade out),then effect is NO_EFFECT
        }
        if(ptr[2] == CHROMA_EFX_SPECTRUM) {
          comp[i].script = 0;                                    // Initialize spectrum script
          setSpectumTransitionTarget(&comp[i]);                      // Initialize faderTime/scratch/faderStep/fx.new/pwm/tmr
        } else {
	        setTransitionTarget(&comp[i],(COLOR *)&comp[i].Chroma_effect.r);
        }
        if (comp[i].id != led[0].id) {
          decodeExternEffect(&comp[i], &matrixStaging[comp[i].index][0]);  
          copyLighting2Active(&comp[i]);                   // copy to active
        }           
        breathType[comp[i].index] = 0;
      } 
    }
  } else {
    randomFlag &= ~(1<<led[0].index);                     // update correspoding flag if effect change from breathing to others
    randomCompFlag &= ~(1<<led[0].index);   
  }
  
  led[0].Chroma_effect.r = ptr[6];                        // byte6: r
  led[0].Chroma_effect.g = ptr[7];                        // byte7: g
  led[0].Chroma_effect.b = ptr[8];                        // byte8: b
  led[0].Chroma_effect.rr = ptr[9];                       // byte9: r when color is 2
  led[0].Chroma_effect.gg = ptr[10];                      // byte10: r when color is 2
  led[0].Chroma_effect.bb = ptr[11];                      // byte11: r when color is 2
  led[0].Chroma_effect.v = led[0].Chroma_effect.vv = 0xFF;// reset v 0xff

  if (ptr[2] == CHROMA_EFX_OFF) {
    led[0].Chroma_effect.r = 0;                           // byte6: r
    led[0].Chroma_effect.g = 0;                           // byte7: g
    led[0].Chroma_effect.b = 0;                           // byte8: b
    led[0].Chroma_effect.rr = 0;                          // byte9: r when color is 2
    led[0].Chroma_effect.gg = 0;                          // byte10: g when color is 2
    led[0].Chroma_effect.bb = 0;                          // byte11: b when color is 2
    led[0].Chroma_effect.v = led[0].Chroma_effect.vv = 0; // effect off, set v = 0;
  } else if (ptr[2] == CHROMA_EFX_REACTIVE) {
    fx =&matrixStaging[led[0].index][0];
    led[0].Chroma_effect.v = led[0].Chroma_effect.vv = 0;
		c.v=0;
    setStageOneColor(fx,MIX|V,c);
  }
  if (ptr[2] != CHROMA_EFX_CUSTOM_FRAME) {
    if(ptr[2] == CHROMA_EFX_SPECTRUM) {
      led[0].script = 0;                                    // Initialize spectrum script
      setSpectumTransitionTarget(led);                      // Initialize faderTime/scratch/faderStep/fx.new/pwm/tmr
    } else {
	    setTransitionTarget(led,(COLOR *)&led[0].Chroma_effect.r);
    }
    decodeExternEffect(led, &matrixStaging[led[0].index][0]);  
  }
  
  breathType[led[0].index] = 0;                            // breathe two color flag
  Trail_Parameter[led[0].index].Trailflag = 0;             // reactive flag
  Trail_Parameter[led[0].index].TrailPWM =  0;             // reactive PWM (0~255)
	after_glow_ready[led[0].index] = 0;                      // reactive timing flag
	TrailTimerCnt[led[0].index] = 0;                         // reactive timer flag
}
void updateProfileLed(CHROMA_PARM *ramLed,U8 profile) 
{
  //U8 *ptr = &protocolTransmit[PAYLOAD];                   // Reference first byte of Protocol Payload
	U8 i;
  U8 pendingEffect=ramLed[0].pendingeffect;
  CHROMA_PARM *led = ledParameters(profile, ramLed[0].id), *comp;// Reference 'active' LED parameters
  LED_FADE *fx;
	COLOR c={0,0,0,0};                                                // Temp color
  previousEffect[led[0].index]=lighting[led[0].index].effect;      // Backup the previous effect
  if (pendingEffect == CHROMA_EFX_OFF) {                         // set OFF,the effect need run the previous effect
    if (led[0].pendingeffect != CHROMA_EFX_OFF) {         // aviod two set EFX_OFF
      led[0].effect = led[0].pendingeffect;               // if running dissolve, effect is NO_EFFECT, when changed to EFX_OFF, make sure effect run last pendingefect and extendeffect is fade out
    }
  } else if (pendingEffect == CHROMA_EFX_REACTIVE) {
    led[0].effect = pendingEffect;
  } else if (pendingEffect == CHROMA_EFX_CUSTOM_FRAME) {
    led[0].effect = pendingEffect;                               // if reactive, off LED immediately
		led[0].extendeffect = NO_EFFECT;
  } else {
    led[0].effect = NO_EFFECT;                            // any effect changed (except fade out),then effect is NO_EFFECT
  }
  
  led[0].pendingeffect = pendingEffect;                          // byte2; effect ID, pendingeffect
	if(led[0].pendingeffect!=CHROMA_EFX_OFF){               // Set to OFF doesn't need to change the flag and rate for keep current effect and fade out
    led[0].Chroma_effect.flags = ramLed[0].Chroma_effect.flags;                    // byte3: flages 
    led[0].Chroma_effect.rate = ramLed[0].Chroma_effect.rate;                     // byte4: rate
		if (led[0].Chroma_effect.rate == 1)     {TrailFadeoutTime[led[0].index]= 2;}// Trail time 2*255 = 0.5s   
    else if(led[0].Chroma_effect.rate == 2) {TrailFadeoutTime[led[0].index]= 4;}// Trail time 4*255 = 1s
    else if(led[0].Chroma_effect.rate == 3) {TrailFadeoutTime[led[0].index]= 6;}// Trail time 6*255 = 1.5s
    else if(led[0].Chroma_effect.rate == 4) {TrailFadeoutTime[led[0].index]= 8;}// Trail time 8*255 = 2s   
	}

  led[0].Chroma_effect.colorNO = ramLed[0].Chroma_effect.colorNO;                  // byte5: color (0~2)  
  if ((pendingEffect == CHROMA_EFX_BREATHE)||(pendingEffect == CHROMA_EFX_REACTIVE)||(pendingEffect == CHROMA_EFX_SPECTRUM)) { // if it a ramdom color selected
    comp = ledParameters(profile, lighting[0].id);        // get the address of the first lighting index
    randomcolorIdex = randomGenerate(0,15);              // get new random color 
    for (i=0; i< NUMBER_OF_LEDS; i++) {                  // check for sync LED
			fx = &matrixStaging[i][0];                                    // get fx address
      if (comp[i].pendingeffect ==  pendingEffect) {            // if effect synced
        if (comp[i].Chroma_effect.colorNO == 0) {
          if (comp[i].id != led[0].id) {
            comp[i].Chroma_effect.r = randomList[randomcolorIdex].r;                // same effect, different ID,then sync the r/g/b
            comp[i].Chroma_effect.g = randomList[randomcolorIdex].g;
            comp[i].Chroma_effect.b = randomList[randomcolorIdex].b;
						c.r=comp[i].Chroma_effect.r;                                            // Temp color
						c.g=comp[i].Chroma_effect.g;
						c.b=comp[i].Chroma_effect.g;
						setStageOneColor(fx,NEW|RGB,c);                                          // Set .new  rgb to c 
          } else {
            ramLed[0].Chroma_effect.r = comp[i].Chroma_effect.r = randomList[randomcolorIdex].r;       // same ID,         
            ramLed[0].Chroma_effect.g  = comp[i].Chroma_effect.g = randomList[randomcolorIdex].g;         
            ramLed[0].Chroma_effect.b  = comp[i].Chroma_effect.b = randomList[randomcolorIdex].b;    
						c.r=comp[i].Chroma_effect.r;                                            // Temp color
						c.g=comp[i].Chroma_effect.g;
						c.b=comp[i].Chroma_effect.g;
						setStageOneColor(fx,NEW|RGB,c);                                         // Set .new to c 
          } 
          if (comp[i].pendingeffect != CHROMA_EFX_SPECTRUM) {
            randomFlag |= 1<<i;                                          // update corresponding sync flag
          } else {
            randomFlag &= ~(1<<i); 
          }
        } else {
          randomFlag &= ~(1<<i);                                       // update correcsponding unsync flag    
        }
//        copyLighting2Active(&comp[i]);                   // copy to active
        comp[i].pendingeffect = pendingEffect;                  // byte2; effect ID, pendingeffect
        comp[i].Chroma_effect.flags = ramLed[0].Chroma_effect.flags;            // byte3: flages         
        
        if (pendingEffect == CHROMA_EFX_REACTIVE) {
          comp[i].effect = pendingEffect;
          fx = &matrixStaging[comp[i].index][0];
          comp[i].Chroma_effect.v = comp[i].Chroma_effect.vv = 0;
					c.v=0;
					setStageOneColor(fx,MIX|V,c);                                         // Set .new to c 
        } else {
          comp[i].effect = NO_EFFECT;                    // any effect changed (except fade out),then effect is NO_EFFECT
        }
        if(pendingEffect == CHROMA_EFX_SPECTRUM) {
          comp[i].script = 0;                                    // Initialize spectrum script
          setSpectumTransitionTarget(&comp[i]);                      // Initialize faderTime/scratch/faderStep/fx.new/pwm/tmr
        } else {
	        setTransitionTarget(&comp[i],(COLOR *)&comp[i].Chroma_effect.r);
        }
        if (comp[i].id != led[0].id) {
          decodeExternEffect(&comp[i], &matrixStaging[comp[i].index][0]);  
          copyLighting2Active(&comp[i]);                   // copy to active
        }           
        breathType[comp[i].index] = 0;
      } 
    }
  } else {
    randomFlag &= ~(1<<led[0].index);                     // update correspoding flag if effect change from breathing to others
    randomCompFlag &= ~(1<<led[0].index);   
  }
  
  led[0].Chroma_effect.r = ramLed[0].Chroma_effect.r;                        // byte6: r
  led[0].Chroma_effect.g = ramLed[0].Chroma_effect.g;                        // byte7: g
  led[0].Chroma_effect.b = ramLed[0].Chroma_effect.b;                        // byte8: b
  led[0].Chroma_effect.rr = ramLed[0].Chroma_effect.rr;                       // byte9: r when color is 2
  led[0].Chroma_effect.gg = ramLed[0].Chroma_effect.gg;                      // byte10: r when color is 2
  led[0].Chroma_effect.bb = ramLed[0].Chroma_effect.bb;                      // byte11: r when color is 2
  led[0].Chroma_effect.v = led[0].Chroma_effect.vv = 0xFF;// reset v 0xff

  if (pendingEffect == CHROMA_EFX_OFF) {
    led[0].Chroma_effect.r = 0;                           // byte6: r
    led[0].Chroma_effect.g = 0;                           // byte7: g
    led[0].Chroma_effect.b = 0;                           // byte8: b
    led[0].Chroma_effect.rr = 0;                          // byte9: r when color is 2
    led[0].Chroma_effect.gg = 0;                          // byte10: g when color is 2
    led[0].Chroma_effect.bb = 0;                          // byte11: b when color is 2
    led[0].Chroma_effect.v = led[0].Chroma_effect.vv = 0; // effect off, set v = 0;
  } else if (pendingEffect == CHROMA_EFX_REACTIVE) {
    fx =&matrixStaging[led[0].index][0];
    led[0].Chroma_effect.v = led[0].Chroma_effect.vv = 0;
		c.v=0;
    setStageOneColor(fx,MIX|V,c);
  }
  if (pendingEffect != CHROMA_EFX_CUSTOM_FRAME) {
    if(pendingEffect == CHROMA_EFX_SPECTRUM) {
      led[0].script = 0;                                    // Initialize spectrum script
      setSpectumTransitionTarget(led);                      // Initialize faderTime/scratch/faderStep/fx.new/pwm/tmr
    } else {
	    setTransitionTarget(led,(COLOR *)&led[0].Chroma_effect.r);
    }
    decodeExternEffect(led, &matrixStaging[led[0].index][0]);  
  }
  
  breathType[led[0].index] = 0;                            // breathe two color flag
  Trail_Parameter[led[0].index].Trailflag = 0;             // reactive flag
  Trail_Parameter[led[0].index].TrailPWM =  0;             // reactive PWM (0~255)
	after_glow_ready[led[0].index] = 0;                      // reactive timing flag
	TrailTimerCnt[led[0].index] = 0;                         // reactive timer flag
}
void CopyOthersProfileParm(U8 cnt) 
{
//  U8 *ptr = &protocolTransmit[PAYLOAD];                   // Reference first byte of Protocol Payload
//  U8 i;
//  i = ptr[0] - 1;
//  user.profile[i].savedLight[cnt].pendingeffect = ptr[2];
//  user.profile[i].savedLight[cnt].Chroma_effect.flags = ptr[3];
//  user.profile[i].savedLight[cnt].Chroma_effect.rate = ptr[4];
//  user.profile[i].savedLight[cnt].Chroma_effect.colorNO = ptr[5];
//	user.profile[i].savedLight[cnt].Chroma_effect.r = ptr[6];
//	user.profile[i].savedLight[cnt].Chroma_effect.g = ptr[7];
//	user.profile[i].savedLight[cnt].Chroma_effect.b = ptr[8];
//	user.profile[i].savedLight[cnt].Chroma_effect.v = 0xFF;
//  user.profile[i].savedLight[cnt].Chroma_effect.rr = ptr[9];
//	user.profile[i].savedLight[cnt].Chroma_effect.gg = ptr[10];
//	user.profile[i].savedLight[cnt].Chroma_effect.bb = ptr[11];
//	user.profile[i].savedLight[cnt].Chroma_effect.vv = 0xFF;
}


void processMacroCommand(void)
{
	U8 *ptr = &protocolTransmit[PAYLOAD];                   // Reference first byte of Protocol Payload
	U32 tmp=0;
	//Disable WDT
//	SN_WDT->CFG = 0x5AFA0000;    
  switch (protocolTransmit[COMMAND_ID]) {
//		case (GET|MACRO_NUMBER):
//		  tmp=getMacroNumber();                              // Get macro number
//		  ptr[0]=((tmp>>8)&0xff);                            // Hight byte  
//		  ptr[1]=(tmp&0xff);                                 // Low byte 
//			break;
		case (GET|MACRO_ID):
			tmp=protocolTransmit[DATA_SIZE];
			getMacroList(B2W(ptr[0],ptr[1]),&ptr[2],&ptr[4],tmp-4);       
			break;
//    case (SET|MACRO_NEW):
	//		addMacro(B2W(ptr[0],ptr[1]));                       // new macro
//			break;
		case (SET|MACRO_DEL):
			tmp=macroDelete(B2W(ptr[0],ptr[1]));
			if(tmp==RETURN_SUCCESS){                   // Delete macro
				;
//			}else if(tmp==RETURN_WAIT){
//				protocolWait=TRUE;
			}else{
				protocolTransmit[STATUS] = COMMAND_ERROR;             // Not support cmd, return error to SW
			}				
			break;
//		case (SET|MACRO_NAME):
//			macroSetName(B2W(ptr[0],ptr[1]),&ptr[3],ptr[2]);   // ptr[0],[1]:macro id,ptr[2]:size,ptr[3]~...,data 
//		  break;
//		case (GET|MACRO_NAME):	
//			ptr[2]=macroGetName(B2W(ptr[0],ptr[1]),&ptr[3]);  // ptr[2]:size
//			 break;
//		case (SET|MACRO_DATA):
//			macroSetData(B2W(ptr[0],ptr[1]),&ptr[6],B2DW(ptr[2],ptr[3],ptr[4],ptr[5])); // Ptr[2][3][4][5]: macro data size (big-endian),
//			break;
//		case (GET|MACRO_DATA):
//			tmp=macroGetData(B2W(ptr[0],ptr[1]),&ptr[6]); // Ptr[2][3][4][5]: macro data size (big-endian),
//		  ptr[2]=((tmp>>24)&0xff);
//		  ptr[3]=((tmp>>16)&0xff);
//		  ptr[4]=((tmp>>8)&0xff);
//		  ptr[5]=(tmp&0xff);
//			break;
//		case (GET|MACRO_STORAGE):
////		  tmp=getMacroNumber();
//		  ptr[0]=((tmp>>8)&0xff);
//		  ptr[1]=(tmp&0xff);
//		  ptr[2]=(MAX_MACRO>>8);                              // Max # of IDs
//		  ptr[3]=(MAX_MACRO&0xff);
////		  tmp=getLeftMem();                                  // Available macro storage space
//		  ptr[4]=((tmp>>24)&0xff);
//		  ptr[5]=((tmp>>16)&0xff);
//		  ptr[6]=((tmp>>8)&0xff);
//		  ptr[7]=(tmp&0xff);
//	//	  tmp=getMaxMem();
//		  ptr[8]=((tmp>>24)&0xff);
//		  ptr[9]=((tmp>>16)&0xff);
//		  ptr[10]=((tmp>>8)&0xff);
//		  ptr[11]=(tmp&0xff);
//			break;
		case (SET|MACRO_MINIDELAY):
			setMacroMinDelay(ptr[0]);
		  requestVariableUpdate(MACRO_CHANGE,NULL);		
			break;
		case (GET|MACRO_MINIDELAY):
			ptr[0]=getMacroMinDelay();
			break;
		case (GET|MACRO_NEW):
			if(!getMallocMacro(B2W(ptr[0],ptr[1]),&ptr[2])){
				protocolTransmit[STATUS] = COMMAND_ERROR;             // Not support cmd, return error to SW
		  }
			break;
		case (SET|MACRO_NEW):
			if(!setMallocMacro(B2W(ptr[0],ptr[1]),B2DW(ptr[2],ptr[3],ptr[4],ptr[5]))){
				protocolTransmit[STATUS] = COMMAND_ERROR;             // Not support cmd, return error to SW
			}		
			break;
		case (GET|MACRO_DATA):
		  getMacroData(B2W(ptr[0],ptr[1]),B2DW(ptr[2],ptr[3],ptr[4],ptr[5]),&ptr[7],ptr[6]);
			break;
		case (SET|MACRO_DATA):
			tmp=setMacroData(B2W(ptr[0],ptr[1]),B2DW(ptr[2],ptr[3],ptr[4],ptr[5]),&ptr[7],ptr[6]);
		  if(tmp==RETURN_WAIT){
				protocolWait=TRUE;
			}else if(tmp==RETURN_FAIL){
				protocolTransmit[STATUS] = COMMAND_ERROR;             // Not support cmd, return error to SW
				protocolWait=FALSE;
			}else{
				protocolWait=FALSE;
			}
			break;
		case (SET|MACRO_MEM_MANAGE):
			if(ptr[2]==0){               // Start  to check
				switch(ptr[3]){
					case ACTION_NONE:
						memoryManageStaue=FLASH_COMPLETED;
					  break;
					case ACTION_CHECK_DATA:
						macroMemoryCheck=1;         // Start check the macro memory 
					  memoryManageStaue=FLASH_ONGOING;  // Set flag to ongoing 
						break;
					case ACTION_FORMAT:
						memset((void *)&macroTable,0,sizeof(macroTable));                               // Macro setting
					  tmp=MAX_MACRO;
					  while(tmp--){
              requestVariableUpdate(MACRO_CHANGE,tmp);   
						}
				    formatMacroMemory();
					  memoryManageStaue=FLASH_COMPLETED;
						break;
					case ACTION_CHECK_IDDATA:
						if(!checkMacroCRC(B2W(ptr[0],ptr[1]))){
							memoryManageStaue=FLASH_ERROR;
						}else{
					    memoryManageStaue=FLASH_COMPLETED;
						}
						break;
				}
			}
			break;
		case (GET|MACRO_MEM_MANAGE):
			ptr[2]=memoryManageStaue;
			break;
//		case (GET|MACRO_MASS_ID):
//			tmp=protocolTransmit[DATA_SIZE];
//			getMacroList(B2W(ptr[0],ptr[1]),&ptr[2],&ptr[4],tmp-4);
//			break;
//		case (SET|MACRO_MASS_NAME):
//			tmp=protocolTransmit[DATA_SIZE];
//		  tmp=setMacroName(B2W(ptr[0],ptr[1]),B2W(ptr[4],ptr[5]),B2W(ptr[2],ptr[3]),&ptr[6],tmp-6);
//			if(tmp==RETURN_WAIT){
//				protocolWait=TRUE;
//			}else if(tmp==RETURN_FAIL){
//				protocolTransmit[STATUS] = COMMAND_ERROR;             // Not support cmd, return error to SW
//				protocolWait=FALSE;
//			}else{
//				protocolWait=FALSE;
//			}
//			break;
//		case (GET|MACRO_MASS_NAME):
//			tmp=protocolTransmit[DATA_SIZE];
//			if(getMacroName(B2W(ptr[0],ptr[1]),&ptr[4],B2W(ptr[2],ptr[3]),&ptr[6],tmp-6)==RETURN_FAIL){   // -6 is for the payload first 6 bytes
//			  protocolTransmit[STATUS] = COMMAND_ERROR;             // Not support cmd, return error to SW
//			}
//			break;
//		case (GET|MACRO_RAW_DATA):
//			tmp=protocolTransmit[DATA_SIZE];
//			GetRamDataDump(B2DW(ptr[0],ptr[1],ptr[2],ptr[3]),&ptr[4],tmp-4);
//			break;
		case (GET|MACRO_INFO):
			tmp=getMaxMacroIDS();
		  ptr[0]=tmp>>8&0xff;
		  ptr[1]=tmp&0xff;
			tmp=getMaxMemory();
		  ptr[2]=tmp>>24&0xff;		
		  ptr[3]=tmp>>16&0xff;
		  ptr[4]=tmp>>8&0xff;
		  ptr[5]=tmp&0xff;
		  tmp=getFreeMemory();
		  ptr[6]=tmp>>24&0xff;		
		  ptr[7]=tmp>>16&0xff;
		  ptr[8]=tmp>>8&0xff;
		  ptr[9]=tmp&0xff;
		  tmp=getRecycleSize();
		  ptr[10]=tmp>>24&0xff;		
		  ptr[11]=tmp>>16&0xff;
		  ptr[12]=tmp>>8&0xff;
		  ptr[13]=tmp&0xff;
			break;
	}	
//  SN_WDT->CFG |= 0x5AFA0001; 			//Enable 
}
void profileLedComand(U8 id)                        // Deal the profile color change for white blance
{
	U8 *ptr = &protocolTransmit[PAYLOAD];             // Reference first byte of Protocol Payload
	if(id==CHROMA_UNDERGLOW_ID){                      // Profile led case ,it only deal for static command and change color
     ledPwmData[LED_PROFILE].r=ptr[6];
		 ledPwmData[LED_PROFILE].g=ptr[7];
		 ledPwmData[LED_PROFILE].b=ptr[8];
	}
}
void requestVariableUpdate(U8 source,U8 updatIndex)
/*++

Function Description:
  This function retriggers a oneshot which allows several of the protocol commands
  to be receive processed and the Variable Storage to be updated once instead of 
  after each protocol command. In addition, there are two different types of storage:
  one that is for critical information and is updated maybe only once on the 
  manufacturing floor (SPECIAL). And the other is for standard device variables and
  state information.  

Arguments:
  type - GENERIC_DATA (standard variables) or SPECIAL_DATA (manufacturing variables).

Returns: 
  NONE

--*/
{
  if (source == SW_CHANGED) {
    updateStorage |= GENERIC_DATA;
    updateOneshot = BUFFERING_TIME_SW;                     // Gvie longer time to allow multiple changes to updated together
  } else if (source == FACTORY_CHANGED) {
    updateStorage |= SPECIAL_DATA;
  } else if (source == SW_CHANGEDPr){
		updateStorage |= GENERICPr_DATA;
    updateOneshot = BUFFERING_TIME_SW;                     // Gvie longer time to allow multiple changes to updated together
		eepromSave.profile|=1<<updatIndex;                     // The profile need to update to eeprom
	} else if (source == USER_CHANGED){
		updateStorage |= GENERICPr_DATA;
		eepromSave.profile|=1<<updatIndex;                     // The profile need to update to eeprom
		updateOneshot=0;
  } else if (source == DEVICE_CHANGED) {
		updateStorage |= GENERIC_DATA;
		updateStorage |= GENERICPr_DATA;
		eepromSave.profile|=1<<updatIndex;                     // The profile need to update to eeprom
	}else if(source == MACRO_CHANGE){
		updateStorage |=MACRO_DATA_UPDATE;
		updateOneshot = BUFFERING_TIME_SW; 
		eepromSave.macro[updatIndex/8]|=1<<(updatIndex%8);
	}else if(source == BLOCK_ASSIGN_CHANGE){
		updateStorage |=BLOCK_MANAGE_DATA;
		updateOneshot = BUFFERING_TIME_SW; 
	}else if(source == BOOTLOADER_CHANGE){
		updateStorage |=BLOCK_MANAGE_DATA;
		updateStorage |= GENERICPr_DATA;
		eepromSave.profile|=0x1F;                 // All the profile need to save 
		updateOneshot=0;
	}
}


void processVariableUpdate(void)
/*++

Function Description:
  This function saves the designated variables in EEPROM storage. 

Arguments:
  NONE

Returns: 
  NONE

--*/
{
//	U8 i;
//	U16 j;
//	U16 start = 0,end = 0,counter = 0;
//	U32 tmp;
  U16 len =0;//, len1 =0;
//	PROFILE_PARM profileTemp;
	
#if (BASIC_FW == 1)                                       // Not save anything to flash with Basic FW, except switch bootloader mode
  if (deviceMode != BOOTLOADER_MODE) {
    return;
  }
#endif
  usbBusyFlag=0;
  if ((updateStorage != 0) && (updateOneshot == 0)) {     // If variable storage need updating, then...
//    __disable_irq();                                      // Disable interrupts while flashing

//    if (usbBusyFlag != 0) {                             // Check updateOneshot flag again, incase that when disableing the irq,then the USB come to change the updateOneshort value.
//      __enable_irq();
//      return;
//    }
	//Disable WDT
//	SN_WDT->CFG = 0x5AFA0000;
    if ((updateStorage & SPECIAL_DATA) != 0) {                  // If it is a special variable request, then...
      sys.crc = crc16((U8 *)&sys, sizeof(sys)-2);
      sys.storageSize = sizeof(sys);
      if ((sys.storageSize%64) == 0) {
        len = sys.storageSize/64;
      } else {
        len = (sys.storageSize/64)+1;
      }
      if (FLASH_Program(MANUFACTURING_SATRT,len, (void *)&sys) == 0) {
        updateStorage &= ~SPECIAL_DATA;
      }
    }
//    if ((updateStorage & GENERIC_DATA) != 0) {            // If it is a generic variable request, then...
//      user.crc = crc16((U8 *)&user, sizeof(user)-2);
//      user.storageSize = sizeof(user);
//       if ((user.storageSize%64) == 0) {
//        len = user.storageSize/64;
//      } else {
//        len = (user.storageSize/64)+1;
//      }     
//      if (FLASH_Program(USER_SETTING_START, len,(U32 *)&user ) == 0) {
//        updateStorage &= ~GENERIC_DATA;                   // If flash was successful, clear need to update
//      }     
//    }
		 if ((updateStorage & MACRO_DATA_UPDATE) != 0) {            // If it is a generic variable request, then...
			 macroTable.crc = crc16((U8 *)&macroTable, sizeof(macroTable)-2);	
       
//			 for(i=0;i<MAX_MACRO;i++){
//				 if((eepromSave.macro[i/8]>>(i%8))&0x01){
//					 //iap_WriteEEPROM((void*)(MACRO_START+i*sizeof(MACRO)), (void *)&macroTable.macro[i], sizeof(MACRO));
//            if ((sizeof(MACRO)%4) == 0) {
//              len = sizeof(MACRO)/4;
//            } else {
//              len = (sizeof(MACRO)/4)+1;
//            }
//               if (((i*sizeof(MACRO))%4) == 0) {
//                len1 = (i*sizeof(MACRO));
//              } else {
//                len1 = ((i*sizeof(MACRO))-((i*sizeof(MACRO))%4));
//              }           
//            if (FLASH_Program(MACRO_START+len1, len,(U32 *)&macroTable.macro[i]) == 0) {
//            }
//            eepromSave.macro[i/8]&=~(1<<(i%8));
//				 }
//			 }
//       //iap_WriteEEPROM((void*)(MACRO_START+sizeof(macroTable.macro)), (void *)&macroTable.macroNumbers, sizeof(macroTable)-sizeof(macroTable.macro));
//            if ((sizeof(macroTable.macro)%4) == 0) {
//              len = sizeof(macroTable.macro)/4;
//            } else {
//              len = (sizeof(macroTable.macro)/4)+1;
//            } 
//            if ((sizeof(macroTable)%4) == 0) {
//              len1 = sizeof(macroTable)/4;
//            } else {
//              len1 = (sizeof(macroTable)/4)+1;
//            }            
//       if (FLASH_Program(MACRO_START+len, len1-len,(U32 *)&macroTable.macroNumbers) == 0) {
//       }
//         updateStorage &= ~MACRO_DATA_UPDATE;                         // If flash was successful, clear need to update
//[
            if ((sizeof(macroTable)%64) == 0) {
              len = sizeof(macroTable)/64;
            } else {
              len = (sizeof(macroTable)/64)+1;
            }
           if (FLASH_Program(MACRO_START, len,(U32 *)&macroTable) == 0) {
              updateStorage &= ~MACRO_DATA_UPDATE;  
           }
//]
			 
		}
		if((updateStorage&BLOCK_MANAGE_DATA)!=0){                   // Update the block assign
			blockMange.crc=crc16((U8 *)&blockMange, sizeof(blockMange)-2);	
            if ((sizeof(blockMange)%64) == 0) {
              len = sizeof(blockMange)/64;
            } else {
              len = (sizeof(blockMange)/64)+1;
            }       
			//if(iap_WriteEEPROM((void*)BLOCK_MAMAGE_START, (void *)&blockMange, sizeof(blockMange)) == CMD_SUCCESS){
      if (FLASH_Program(BLOCK_MAMAGE_START,len,(U32 *)&blockMange) == 0) {
				updateStorage &= ~BLOCK_MANAGE_DATA;  
			}
		 }
		if ((updateStorage & GENERICPr_DATA) != 0) {      
			  user.storageSize = sizeof(user);
			  user.crc = crc16((U8 *)&user, sizeof(user)-2);
//			  tmp=(U32)&user.profile-(U32)&user;
//            if ((tmp%4) == 0) {
//              len = tmp/4;
//            } else {
//              len = (tmp/4)+1;
//            }       
//			  //if (iap_WriteEEPROM((void*)Profile_SETTING_START, (void *)&user,tmp) == CMD_SUCCESS) {                                       // Write head
//         if (FLASH_Program(Profile_SETTING_START, len,(U32 *)&user) == 0) {
//					for(i=0;i<profileNO;i++){
//						if((eepromSave.profile>>i)&0x01){
//							//iap_ReadEEPROM((U8*)(Profile_SETTING_START+tmp+i*sizeof(PROFILE_PARM)), (U8*)&profileTemp, sizeof(PROFILE_PARM));		
//              memcpy( (U8*)&profileTemp, (U8*)(Profile_SETTING_START+tmp+i*sizeof(PROFILE_PARM)),sizeof(PROFILE_PARM));	              
//							for (j=0; j < sizeof(PROFILE_PARM);j++) {
//								if (*((U8*)&profileTemp+j) != *((U8*)&user.profile[i]+j)) {
//								  if (counter) {
//									  end = j;
//									} else {
//									  start = j;
//									}
//									counter++;
//								}
//							}	
//              if(end<start){
//								end=start;
//							}					
//              if (((tmp+i*sizeof(PROFILE_PARM) + start)%4) == 0) {
//                len = (tmp+i*sizeof(PROFILE_PARM) + start);
//              } else {
//                len = ((tmp+i*sizeof(PROFILE_PARM) + start)-((tmp+i*sizeof(PROFILE_PARM) + start)%4));
//              }
//              if (((end - start)%4) == 0) {
//                len1 = (end - start)/4;
//              } else {
//                len1 = ((end - start)/4)+1;
//              }              
//							//iap_WriteEEPROM((void*)(Profile_SETTING_START+tmp+i*sizeof(PROFILE_PARM) + start), (U8*)&user.profile[i]+start, (end - start+1));  // Write profile
//              FLASH_Program((Profile_SETTING_START+len), len1, (U32*)&user.profile[i]+start);  // Write profile
//							eepromSave.profile&=~(1<<i);
//							start=end=counter=0;						
//						}
//					}
//              if (((sizeof(user)-2)%4) == 0) {
//                len = (sizeof(user)-2);
//              } else {
//                len = ((sizeof(user)-2)-((sizeof(user)-2)%4));
//              }          
//				//iap_WriteEEPROM((void*)(Profile_SETTING_START+sizeof(user)-2), (U8*)&user+sizeof(user)-2,2);                                // Write crc
//        FLASH_Program((Profile_SETTING_START+len), 1,(U32*)&user+len);                                // Write crc  
//				updateStorage &= ~GENERICPr_DATA;                  // Clear the update flag
//              if (((sizeof(user))%4) == 0) {
//                len = (sizeof(user)/4);
//              } else {
//                len = (sizeof(user)/4)+1;
//              }
              if ((user.storageSize%64) == 0) {
                len = (user.storageSize/64);
              } else {
                len = (user.storageSize/64)+1;
              }              
          if (FLASH_Program(Profile_SETTING_START, len,(U32*)&user) == 0) {  
            updateStorage &= ~GENERICPr_DATA;                  // Clear the update flag 
          }            
        } 
			 if (deviceMode == BOOTLOADER_MODE) {  // If switching to Bootloader mode, then...
        *(U32 *)(SW_DFU_FLAG_ADDR) = SW_DFU_SIGNATURE;         // Set soft enter bootloader flag
        NVIC_SystemReset();                                    // Request a MCU soft reset
       }
		}
//    __enable_irq();                                             // Re-enable interrupts
//  }
//    SN_WDT->CFG |= 0x5AFA0001; 			//Enable 
}


