/*++

Copyright (c) 2012-2014.
All rights reserved. This program contains proprietary and confidential information.

Module Name:
  variables.c

Compiler:
  Keil Software ?ision v5.18, with ST Semiconductor STM32F072C8 proccessor support.

Abstract:
  This file provides the definitions for all of the global variables.

--*/
#include <SN32F240B.h>
#include "main.h"
#include "variables.h"
#include "usb_protocol.h"
#include "FlashPrg.h"
#include "LED_RamSetting.h"
#include "LED_Const.h"
#include "SPI.h"
#include "LED.h"
#include "Mouse_Const.h"
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
    if (1) {//)(j == 0) {                                         // Exit if invalid checksum
      protocolTransmit[STATUS] = COMMAND_BUSY;            // Set command packet is being processed
      switch (protocolTransmit[COMMAND_CLASS]) {          // Dispatch to corresponding Command type processing

      case INFO_CMD:
        processDeviceInfoCommand();                       // Dispatch to Device Information command processing
        break;

//      case BUTTON_CMD:
//        processButtonCommand();                           // Dispatch to button command processing
//        break;
      
      case LED_CMD:
////				LEDReceiveFlag = 1;                               // Receive LED command, that not run LED driver in timer.    
        processLedCommand();                              // Dispatch to LED command processing
////			  LEDReceiveFlag = 0;
        break;
      
      default:                                            // Command Classes not support by this device
        protocolTransmit[STATUS] = COMMAND_NOTSUPPORT;    // Not support cmd, return 05 to SW
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
    usbReceiveFlag = 0;                                   // Clear receive flag to aollow next protocol cmd can be enter
  }
}


void processDeviceInfoCommand(void)
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
  U8 *ptr,i,j,n,m,temp,temp1;
  ptr = &protocolTransmit[PAYLOAD];
  switch (protocolTransmit[COMMAND_ID]) {
    case (SET | DEVICE_MODE):
      deviceMode = ptr[0];
//      if (deviceMode == BOOTLOADER_MODE) {
//        GPIO_InitTypeDef GPIO_InitStruct;
//        GPIO_InitStruct.Pin = BOOT_CTR_Pin;
//        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//        GPIO_InitStruct.Pull = GPIO_NOPULL;
//        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
//        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
//        HAL_Delay(2);        
//        HAL_GPIO_WritePin(BOOT_CTR_GPIO_Port, BOOT_CTR_Pin, GPIO_PIN_SET);
//        GPIO_InitStruct.Pin = LED_RESET_Pin;
//        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//        GPIO_InitStruct.Pull = GPIO_NOPULL;
//        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
//        HAL_GPIO_Init(LED_RESET_GPIO_Port, &GPIO_InitStruct);
//        HAL_Delay(2);         
//        HAL_GPIO_WritePin(LED_RESET_GPIO_Port, LED_RESET_Pin, GPIO_PIN_RESET);                  // Low to shut down led controller
//        HAL_Delay(20);
//        NVIC_SystemReset();                               // Request a MCU soft reset        
////        while(1); 
//      }
      break;
    case (GET | DEVICE_MODE):
      ptr[0] = deviceMode;
      break;      
    case (GET | FW_VERSION):
      ptr[0] = firmwareVersion[0];
      ptr[1] = firmwareVersion[1];
      ptr[2] = firmwareVersion[2];
     break;
//    case (GET | MATRIX_INFO):
//      ptr[0] = mX;
//      ptr[1] = mY;
//     break;    
//    case (GET | DEBOUNCE_INFO):
//      ptr[0] = ram.makeDebounce;
//      break;  
//    case (SET | DEBOUNCE_INFO):
//      ram.makeDebounce = ptr[0];
//      requestVariableUpdate(SW_CHANGED);
//      break; 
    case (GET | POLLING_INFO):
      ptr[0] = ram.profile[0].Sensor.pollingRate;
      break;  
    case (SET | POLLING_INFO):
      ram.profile[0].Sensor.pollingRate = ptr[0];
      requestVariableUpdate(SW_CHANGED);
      break; 
//    case (GET | MOUSE_DPI):
//      ptr[0] = (U8)(ram.profile[0].dpiCur.x >> 8);
//      ptr[1] = (U8)(ram.profile[0].dpiCur.x);
//      ptr[2] = (U8)(ram.profile[0].dpiCur.y >> 8);
//      ptr[3] = (U8)(ram.profile[0].dpiCur.y);    
//      break;  
//    case (SET | MOUSE_DPI):
//      ram.profile[0].dpiCur.x  = (ptr[0]<<8) | ptr[1];
//      ram.profile[0].dpiCur.y  = (ptr[2]<<8) | ptr[3]; 
//      SPI_W_2BYTE(SENSOR_RESOLUTION_3325, (ram.profile[0].dpiCur.x/40));    
//      requestVariableUpdate(SW_CHANGED);  
//      break; 
    case (GET | PROFILE_DPI):  
      ptr[0] = ram.aliveProfile;
//      ptr[0] = (U8)(ram.profile[0].dpiCur.x >> 8);
//      ptr[1] = (U8)(ram.profile[0].dpiCur.x);
//      ptr[2] = (U8)(ram.profile[0].dpiCur.y >> 8);
//      ptr[3] = (U8)(ram.profile[0].dpiCur.y);  
      for (i=0; i<ptr[0]; i++) {
        *++ptr = i;
        *++ptr = ram.profile[i].Sensor.maxStages;
        *++ptr = ram.profile[i].Sensor.dpiStage;
        *++ptr = (U8)(ram.profile[i].dpiCur.x >> 8);
        *++ptr = (U8)(ram.profile[i].dpiCur.x );
        *++ptr = (U8)(ram.profile[i].dpiCur.y >> 8);
        *++ptr = (U8)(ram.profile[i].dpiCur.y ); 
        for (j=0; j<ram.profile[i].Sensor.maxStages; j++) {
          *++ptr = (U8)(ram.profile[i].Sensor.stageDPI[j].x >> 8);
          *++ptr = (U8)(ram.profile[i].Sensor.stageDPI[j].x );
          *++ptr = (U8)(ram.profile[i].Sensor.stageDPI[j].y >> 8);
          *++ptr = (U8)(ram.profile[i].Sensor.stageDPI[j].y );    
        }          
      }    
      break;  
    case (SET | PROFILE_DPI):
      for (i=0; i<ptr[0]; i++) {
        n = *++ptr;  
        m = ram.profile[n].Sensor.maxStages = *++ptr;   
        ram.profile[n].Sensor.dpiStage = *++ptr;
        temp =  *++ptr;
        temp1 = *++ptr;
        ram.profile[n].dpiCur.x  = temp << 8 | temp1; 
        temp =  *++ptr;
        temp1 = *++ptr;       
        ram.profile[n].dpiCur.y  = temp << 8 | temp1; 
        for (j=0; j<m; j++) {
          temp =  *++ptr;
          temp1 = *++ptr;
          ram.profile[n].Sensor.stageDPI[j].x = temp << 8 | temp1;
          temp =  *++ptr;
          temp1 = *++ptr;
          ram.profile[n].Sensor.stageDPI[j].y = temp << 8 | temp1;   
        }          
      }      
//      ram.profile[0].dpiCur.x  = (ptr[0]<<8) | ptr[1];
//      ram.profile[0].dpiCur.y  = (ptr[2]<<8) | ptr[3]; 
      SPI_W_2BYTE(SENSOR_RESOLUTION_3325, (ram.profile[0].dpiCur.x/40));    
      requestVariableUpdate(SW_CHANGED);  
      break;     
////    case (GET | LAYOUT_INFO):
////      ptr[0] = sys.editionInfo[0];
////      ptr[1] = sys.editionInfo[1];
////      break;  
//    case (SET | DEVICE_RESET):
//    memoryCopy((void *)&ram, (void *)&dft_generic, sizeof(ram));
//    requestVariableUpdate(SW_CHANGED); 
//    forceLightingReload(); 
//    factoryReset = 0;
//      break;    
    default:
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
//  U8 i,j;
//  LED_BASIC_PARM *led;
////  LED_MAXM *cal;
////  LED_FADE *fx;
	U8 table[5] = {20,30,40,50,80};
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
    ptr[2] = ram.profile[0].led.color.R;
    ptr[3] = ram.profile[0].led.color.G;
    ptr[4] = ram.profile[0].led.color.B;
    break;
  case (SET | LED_RGB_PARAM):                                // Return 3 bytes  [LED Class, LED ID, Effect]
    ram.profile[0].led.color.R = ptr[2];
    ram.profile[0].led.color.G = ptr[3];
    ram.profile[0].led.color.B = ptr[4];
    LED_Mode_ReInit();
     if (ptr[0]) {
       requestVariableUpdate(SW_CHANGED);     
     }  
    break;
//  case (GET | LED_BRIGHTNESS):                                // Return 3 bytes  [LED Class, LED ID, Effect]
//   ptr[2] = ram.effectParm.backlitLevel;
//    break;
//  case (SET | LED_BRIGHTNESS):                                // Return 3 bytes  [LED Class, LED ID, Effect]
//     ram.effectParm.backlitLevel = ptr[2];
//     if (ptr[0]) {
//       requestVariableUpdate(SW_CHANGED);     
//     }
//    break;      
  case (GET | LED_EFFECT):                                // Return 3 bytes  [LED Class, LED ID, Effect]
    ptr[0] = ram.profile[0].led.effect;
    ptr[1] = 0;
    ptr[2] = ram.profile[0].led.speed;  
    ptr[3] = 0;     
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
    
          ram.profile[0].led.effect  = bLED_Mode =  ptr[0];
  				wLED_Status |= mskLED_ModeChange;	
          ram.profile[0].led.speed = bLED_DataRefreshTime_Reload = table[ptr[2]];
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
        requestVariableUpdate(SW_CHANGED);     
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

void requestVariableUpdate(uint8_t source)
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
  } else if (source == LED_CUSTOM_CHANGED){                // If it is custom effect LED matrix data need to save  
    updateStorage |= USER_LED_DATA;		
    updateOneshot = BUFFERING_TIME_SW;                     // Gvie longer time to allow multiple changes to updated together
	} else{  	
    updateStorage |= GENERIC_DATA;
  }
}

//uint8_t flashProgram(uint32_t addr, uint32_t *data, uint32_t size)
///*++

//Function Description:
//  This function retriggers a oneshot which allows several of the protocol commands
//  to be receive processed and the Variable Storage to be updated once instead of 
//  after each protocol command. In addition, there are two different types of storage:
//  one that is for critical information and is updated maybe only once on the 
//  manufacturing floor (SPECIAL). And the other is for standard device variables and
//  state information.  

//Arguments:
//  type - GENERIC_DATA (standard variables) or SPECIAL_DATA (manufacturing variables).

//Returns: 
//  NONE

//--*/
//{
//	FLASH_EraseInitTypeDef EraseInitStruct;
//	uint32_t Address = 0, *Data = 0, PageError = 0, result = 0, length = 0;
//  /* Unlock the Flash to enable the flash control register access *************/
//  HAL_FLASH_Unlock();

//  /* Erase the user Flash area
//    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

//  /* Fill EraseInit structure*/
//  EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
//  EraseInitStruct.PageAddress = addr;
//  EraseInitStruct.NbPages = 1;//(FLASH_USER_END_ADDR - addr) / FLASH_PAGE_SIZE;  

//  if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
//  {
//    /*
//      Error occurred while page erase.
//      User can add here some code to deal with this error.
//      PageError will contain the faulty page and then to know the code error on this page,
//      user can call function 'HAL_FLASH_GetError()'
//    */
//    /* Infinite loop */
//    return result; 
//  }

//  /* Program the user Flash area word by word
//    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

//  Address = addr;
//  Data = data;
////  while (Address < FLASH_USER_END_ADDR)
//  while (length < size)    
//  { 
//    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, (uint64_t)*Data) == HAL_OK)
//    {
//      Address = Address + 4;
//	    Data++;
//      length = length +4;
//    }
//    else
//    {
//      /* Error occurred while writing data in Flash memory.
//         User can add here some code to deal with this error */
//      return result; 
//    }
//  }

//  /* Lock the Flash to disable the flash control register access (recommended
//     to protect the FLASH memory against possible unwanted operation) *********/
//  HAL_FLASH_Lock();
//  return result=1;  
//}
void processVariableUpdate(void)
/*++

Function Description:
  This function saves the designated variables in flash storage. All variables are stored
  in a flash sector (4K), as specified by the STORAGE structure. The size of STORAGE can be
  up to 4K - 256 maximum. If there is a update variable request, the interrupts are disabled;
  since there can be no interruptions during the flash process, nor can we have any of the 
  variables to be stored changing. A checksum is generated to give some validity to the ROM
  based variables. Then the last sector (0x7000) is erased and the variables are written in 
  256 byte blocks until complete. Also contained in this sector is the firmware Signature, 
  which must also be updated. Finally the update request is cleared if the flash update was
  successful and the interrupts are restored again.

  The Signature bits that are '1' can be set to '0' without erasing the sector or upsetting
  the rest of the variables. This technique is used by the Bootloader so it can indicate the
  completion of Firmware Update without knowing the variable definition.

  There are two blocks of variables that can be updated. The 'SPECIAL_DATA' block is reserved
  for manufacturing information: Serial Number, Keyboard Layout, and LED calibration values.
  These manufacturing variables are rarely updated (most likely only once on the line), the
  separating of these variables provides protection against accidentaly erasure of these values.
  The other page 'GENERIC_STORAGE' is for generic type variables and will be updated in the 
  field many times.

  The SPECIAL_DATA in not a flash page as they are 4K in size, this storage is accomplished by
  writing into the unused space at the top of the BootBlock page starting at SPECIAL_STORAGE.
  This uses the same prinpical as mention about the Signature, you can write zeros if the
  values are all ones to begin with. Since this means these variables have a limited number
  of times they can be updated, great care is taken not to waste and update. The data is 
  always checked to see if the current values already match the new values. Other check is
  made to see if the current block can be correctly written over with with the new data. If
  not the block is abandon and the next block on a 64 byte boundary is used.

Arguments:
  NONE

Returns: 
  NONE

--*/
{
  U16 len =0; 
  if ((updateStorage != 0) && (updateOneshot == 0)) {     // If variable storage need updating, then...
//    __disable_irq();                                      // Disable interrupts while flashing, take about 12ms to write the eeprom
//    if (updateOneshot !=0) {__enable_irq();}               // Check updateOneshot flag again, incase that when disableing the irq,then the USB come to change the updateOneshort value.
//  
  SN_CT16B1->TC = 0;	   
  SN_CT16B1->TMRCTRL_b.CEN = 0;
  SN_CT16B1->IC =0x0ffffffff;	  
	//Disable WDT
	//SN_WDT->CFG = 0x5AFA0000;// ??    
    if ((updateStorage & SPECIAL_DATA) != 0) {            // If it is a special variable request, then...
      sys.crc = crc16((U8 *)&sys, sizeof(sys)-2);
      sys.cont.storageSize = sizeof(sys);
      if ((sys.cont.storageSize%4) == 0) {
        len = sys.cont.storageSize/4;
      } else {
        len = (sys.cont.storageSize/4)+1;
      }
      if (FLASH_Program(MANUFACTURING_SATRT,len, (void *)&sys) == 0) {
        updateStorage &= ~SPECIAL_DATA;                   // If flash was successful, clear need to update        
//        updateStorage &= ~CRC_DATA;
      }
    }
    if ((updateStorage & GENERIC_DATA) != 0) {            // If it is a generic variable request, then...
      ram.crc = crc16((U8 *)&ram, sizeof(ram)-2);
      ram.storageSize = sizeof(ram);
       if ((ram.storageSize%4) == 0) {
        len = ram.storageSize/4;
      } else {
        len = (ram.storageSize/4)+1;
      }     
      if (FLASH_Program(USER_SETTING_START, len,(U32 *)&ram ) == 0) {
        updateStorage &= ~GENERIC_DATA;                   // If flash was successful, clear need to update
 //       crc.ram = crc16((U8 *)&ram, sizeof(ram));
 //       updateStorage |= CRC_DATA;
      }
      
    }
	SN_CT16B1->TMRCTRL_b.CEN = 1;		
//    if ((updateStorage & USER_LED_DATA) != 0) {            // If it is a generic variable request(For LED matrix data), then...
//      if (dfu_WriteEEPROM((void*)USER_SETTING_START1, (void *)&user, sizeof(user)) == CMD_SUCCESS) {
//        updateStorage &= ~USER_LED_DATA;                  // If flash was successful, clear need to update
//        crc.user = crc16((U8 *)&user, sizeof(user));
//				updateStorage |= GENERIC_DATA | CRC_DATA;                    // Also need to save the ram.ledEffect
//        updateOneshot = BUFFERING_TIME_SW;                // Apply one short
//      }	
//    }

//    if ((updateStorage & CRC_DATA) != 0) {
//      if (dfu_WriteEEPROM((void*)CRC16_START, (void *)&crc, sizeof(crc)) == CMD_SUCCESS) {
//        updateStorage &= ~CRC_DATA;                  // If flash was successful, clear need to update
//      }
//    }

//    if (deviceMode == BOOTLOADER_MODE) {                // If switching to Bootloader mode, then...
//#if 0     
//      *(U32 *)(0x10000FF8) = 0xAAAAAAAA;                // Set soft enter bootloader flag
//      NVIC_SystemReset();                               // Request a MCU soft reset
//#else
//      dfu_EraseSector(SIGNATURE_ADDR>>12, SIGNATURE_ADDR>>12);
//      NVIC_SystemReset();   
//      for (; ; );
//      //if (dfu_EraseSector(5, 5) == CMD_SUCCESS) {       // Erase the last sector (number 7)
//      //  dfu_InitializeBuffer();                         // Initialize the flash buffer
//      //  flashBuffer[(SIGNATURE_ADDR&0x00FF)>>2] = DFU_SIGNATURE;
//      //  dfu_CopyToFlash((void *)flashBuffer, (void *)(SIGNATURE_ADDR&0xFF00), 256);
//      //  NVIC_SystemReset();    
//      //}
//#endif
//    }

//    __enable_irq();                                       // Re-enable interrupts
  }
//  if (factoryReset == 4) {
//    memoryCopy((void *)&ram, (void *)&dft_generic, sizeof(ram));
//    requestVariableUpdate(SW_CHANGED); 
//    forceLightingReload(); 
////    forceLightingUpdate();   
//    factoryReset = 0;
//  }
}

uint16_t crc16(U8* pData, U16 len)
/*++

Function Description:
  The function update_crc_ccitt calculates  a  new  CRC-CCITT
  value  based  on the previous value of the CRC and the next
  byte of the data to be checked.

Arguments:
  *pData = pointer to the data to caluate crc16
  len    = length of pData

Returns: 
  NONE

--*/
{
  U8 x;
  U16 crc = 0xFFFF;

  while (len--){
    x = crc >> 8 ^ *pData++;
    x ^= x >> 4;
    crc = (crc << 8) ^ ((U16)(x << 12)) ^ ((U16)(x <<5)) ^ ((U16)x);
  }
  return crc;
}


//CRC-8 - based on the CRC8 formulas by Dallas/Maxim
//code released under the therms of the GNU GPL 3.0 license
uint8_t crc8(const U8 *data, U8 len) 
{
  U8 crc, extract, sum, i;

  crc = 0;
  while (len--) {
    extract = *data++;
    for (i = 8; ; i--) {
      sum = (crc ^ extract) & 0x01;
      crc >>= 1;
      if (sum) {
        crc ^= 0x8C;
      }
      extract >>= 1;
    }
  }
  return crc;
}

U32 randomGenerate(U32 lower, U32 upper)
/*++

Function Description:
  This function generates a pseudo random number within the specified range.

Arguments:
  lower - Low side of generated number.
  upper - High side of generated number.

Returns:
  value - Generated random number.

--*/
{
  static S32 rnd_seed = 1;                                  // Random number generator seed

  U32 value, test = 0x80000000, mask = 0xFFFFFFFF;
  U32 hi = upper, lo = lower;

  if (upper < lower) {                                    // If upper is smaller than lower limit, then...
    lo = upper;                                           // Swap the upper and lower limits
    hi = lower;
  }
  value = (hi - lo);                                      // Calculate range of numbers
  if (value != 0) {                                       // If valid range, then...
    while ((value & test) == 0) {                         // Build mask for upper range
      test >>= 1;                                         // Next bit position
      mask >>= 1;                                         // Next mask value
    }
    do {                                                  // Create a random # within the range
      rnd_seed = ((RND_A * (rnd_seed % RND_Q)) - (RND_R * (rnd_seed / RND_Q)));
      if (rnd_seed <= 0) {                                // If the seed is not positive, then...
        rnd_seed += RND_M;                                // Make the seed positive again
      }
      value = ((rnd_seed & mask) + lo);                   // Force range to start above lower
    } while (value > hi);                                 // If range is above upper, redo calculation
  } else {
    return (upper);                                       // Else there is no range (return upper)
  }
  return (value);
}


void memoryCopy(void* dst, void* src, U16 count)
/*++

Function Description:
  This function copies a specified amount of data from one memory location to
  another.

Arguments:
  dst   - Address of memory destination.
  src   - Address of memory source.
  count - Amount memory to copy.

Returns:
  NONE

--*/
{
  U8* a = (U8 *)dst;
  U8* b = (U8 *)src;
  while (count--) {
    *a++ = *b++;
  }
}

void memorySet(void* dst, U8 v, U16 count)
/*++

Function Description:
  This function copies a specified amount of data from one memory location to
  another.

Arguments:
  dst   - Address of memory destination.
  v     - value to set.
  count - Amount memory to copy.

Returns:
  NONE

--*/
{
  U8* a = (U8 *)dst;
  while (count--) {
    *a++ = v;                                          // Copy source memory to destination memory
  }
}
