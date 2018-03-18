/*++

Copyright (c) 2013-2014 Razer Ltd.
All rights reserved. This program contains proprietary and confidential information.

Module Name:
  button.c

Compiler:
  Keil Software µVision v5.11, with NXP Semiconductor LPC11U2x proccessor support.

Abstract:
  This file provides all of the keyboard scanning, debouncing, and processing
  activities.

--*/

#include "main.h" 
#include "gpio.h" 
#include "button.h"
#include "variables.h"
#include "usb_hardware.h"
#include "Mouse.h"
#include "avago_api.h"
#include "macro.h"
#include "profile.h"
#include "keyboard.h"
STANTARD_BUTTON buttonStatus[NUMBER_OF_BUTTONS] = {0};    //  is for the profile button 
BTN_PAR btnAssign[NUMBER_OF_BUTTONS];                                       // Button assignment (Profile 0 = Default Assignment)
U8  currentButtons;                                       // To recorder current actived buttons 
U8 profileButton=0;                                       // For profile switch button flag       
U8 doubleClickEven=0;                                    // For double click even
U8 doubleClickButton=0;                                  // Double click buttons
U8 doubleClickCnt=0;                                     // Count the double click times
//BUTTON_TO_MACRO buttonToMacro[9]={0};                     // Every button save the active macro
TURBO turbo; 
U16 turboTimer=0;                                          // For turbo timer cnt;
U8 onTheFlay=0;                                           // On the fly for DPI
void initialButtons(void)
/*++

Function Description:
  This function will scanning mouse buttonsl. It can debouce button press
  and release event. the debouce time is 10ms by default.
   
Arguments:
  NONE
  
Returns: 
  NONE

--*/

{ 
  U8 i;
	currentButtons = 0;
  memcpy((U8*)&btnAssign,(U8*)&user.profile[user.ProNO].btnAssign,sizeof(btnAssign));
  for (i=0; i<NUMBER_OF_BUTTONS; i++) {  // is for the profile button
    buttonStatus[i].status = 0;
    buttonStatus[i].active = 0;
    buttonStatus[i].debounce = 0;
    buttonStatus[i].nativeDebounce = 0;
		buttonStatus[i].debounceflag = 0;
  }
}
//  U32 currentStatus = 0; 

void scanningButtonStatus(void)
/*++

Function Description:
  This function will scanning mouse buttonsl. It can debouce button press
  and release event. the debouce time is 10ms by default.
   
Arguments:
  NONE
  
Returns: 
  NONE

--*/
{  
  U8 i, makeDeb, makeMsk, breakDeb, breakMsk;
  U32 currentStatus = 0; 

  if (surfaceQual >= 0x40) {  //   on the surface case 
    makeDeb = 0;
    breakDeb = 0;
    makeMsk = user.makeMsk;
    breakMsk = user.breakMsk;
  } else {                   //   lift off case 
    makeDeb = user.makeDebounce;
    breakDeb =user.breakDebounce;
    makeMsk = user.makeMsk-makeDeb;
    breakMsk = user.breakMsk-breakDeb;
    for (i = 0;i < NUMBER_OF_BUTTONS; i++) {
      buttonStatus[i].debounceflag = 1;
    }
  }
  
  if (makeDeb > MAX_DEBOUNCE) {                           // Prevent debounce time being out of limitation 40 ms
     makeDeb = DFT_MAKE_DEBOUNCE;  
  }
  if (breakDeb > MAX_DEBOUNCE) {                          // Prevent debounce time being out of limitation 40 ms
     breakDeb = DFT_BREAK_DEBOUNCE;  
  }
  if (makeMsk > 30) {
    makeMsk = DFT_MAKE_MSK;
  }
  if (breakMsk > 30) {
    breakMsk = DFT_BREAK_MSK;
  }
  
  currentStatus = BTN_INPUT;                              // Total 7 Mouse buttons + 12 keypad key.
  for (i = 0; i < NUMBER_OF_BUTTONS-2; i++) {             // -2 Is for remove scroll up and down 
    if ((buttonStatus[i].status & MASKING) != 0) {        // Masking period for anti-double click. filter out button status during it.
//      if (buttonStatus[i].active == ((currentStatus >> i) & 0x01)) {  // if the button active changed 
//        if (buttonStatus[i].active == BTN_MAKE) {                     // it is on Make
//          buttonStatus[i].debounce = makeMsk;                         // update make masking 
//        } else {                                                      // it is on break
//          buttonStatus[i].debounce = breakMsk;                        // update break masking  
//        }
//      }
      if (buttonStatus[i].debounce == 0) {              // Decrease the timer, if timeout, then
        buttonStatus[i].status &= ~MASKING;               // Clear masking period flag
      } else {
        if (buttonStatus[i].active != ((currentStatus >> i) & 0x01)) {  // if the button active unchanged 
          buttonStatus[i].debounce--;                                   //  continue counter       
          buttonStatus[i].nativeDebounce = 0;                           // reset native counter 
        } else {                                                        // if the button active changed 
          if (++buttonStatus[i].nativeDebounce >= makeMsk) {            // if native masking timout 
            buttonStatus[i].status &= ~MASKING;                         // Clear masking period flag
          }
        }
        continue;                                         // ignore this scanning
      }
    }

    if (((currentStatus >> i) & 0x01) == PRESSE) {        // Low, button in pressed status
      if (buttonStatus[i].active != BTN_MAKE) {
        if ((buttonStatus[i].status & BTN_MAKE) == 0) {   // button isn't in make status in the before
          buttonStatus[i].status |= BTN_MAKE;             // update button status to make status
          buttonStatus[i].debounce = 1;                   // Start debounce time cnt.
        } else {
          
          if (buttonStatus[i].debounceflag == 1) {        // add 2016/11/24 for debounce test by jolie
            makeDeb = user.makeDebounce;
          } else {
            makeDeb = 0;
          }
          
          if (++buttonStatus[i].debounce > makeDeb) { // Debounce time out
						processButtonEvent(BTN_MAKE, i);              // Process this key make event
            buttonStatus[i].active = BTN_MAKE;
// [ Remove first, for this isn't got approval to apply to all mouse projects. 20150617           
            buttonStatus[i].status |= MASKING;            // Start Anti-double click masking filter time period
            buttonStatus[i].debounce = makeMsk;
            buttonStatus[i].debounceflag = 0;
          }
        }
      } else {
        buttonStatus[i].debounce = 0;
      }
    } else {                                              // Button in release status
      if (buttonStatus[i].active != BTN_BREAK) {
        if ((buttonStatus[i].status & BTN_MAKE) != BTN_BREAK) { // button is in press status in the before
          buttonStatus[i].status = BTN_BREAK;               // Update button status to release
          buttonStatus[i].debounce = 1;                     // Set up the filter debounce, this button status will be ignore during it
        } else {
          if (buttonStatus[i].debounceflag == 1) {        // add 2016/11/24 for debounce test by jolie
            breakDeb = user.breakDebounce;
          } else {
            breakDeb = 0;
          }
          if (++buttonStatus[i].debounce > breakDeb) {
						 processButtonEvent(BTN_BREAK, i);                 // Process this key break event
            buttonStatus[i].active = BTN_BREAK;        
            buttonStatus[i].status |= MASKING;            // Start Anti-double click masking filter time period
            buttonStatus[i].debounce = breakMsk;
            buttonStatus[i].debounceflag = 0;
          }
        }
      } else {
        buttonStatus[i].debounce = 0;
      }
    }
  }
}


U8  buttonAmbidextrous(U8 buttonIdx) 
{
	U8 ret=buttonIdx;
//	if(user.ambidextrous==LEFT_HAND){
//		switch(buttonIdx){
//			case INDEX_LEFT:    //  Left click
//				ret=INDEX_RIGHT;   // Right click
//				break;
//			case INDEX_RIGHT:    //Right click
//				ret=INDEX_LEFT;   // Left click
//				break;
//			case INDEX_LBACK:    //Left back
//				ret=INDEX_RBACK;   
//				break;
//			case INDEX_LFWD:    //Left forward
//				ret=INDEX_RFWD;
//				break;
//			case INDEX_RBACK:    //Right back
//				ret=INDEX_LBACK;
//       break;
//      case INDEX_RFWD:    //RIght forward
//				ret=INDEX_LFWD;
//       break;
//			default:
//       break;				
//		}
//	}
	return ret;
}

void processButtonEvent(U8 eventType, U8 btnIdx)
/*++

Function Description:
  This function processes the function of given button by button index and event type
 
Arguments:
  eventType - button make[1] or break[0].
  btnIdx      - indicate which button of this button event. 
                    bit0-bit10 means button0-10, total 11 buttons

Returns: 
  NONE

--*/
{
	U8 i,k;	
//  U16 table[] = {DPI_800,DPI_1800,DPI_4500,DPI_9000,DPI_16000};
	U8 type,repeatTimes;
	U16 macroId;
	U16 dipX,dipY;
	U8 modeData=0;
	static U16 hyperShiftMark=0;
  static U16 normalMark=0;
	static U16 hyperShiftButtons=0;
//	static U8 profileButtonTest=0;
	static bool modeHyperShift=false;
	static U8 modifyKey[8]={0,0,0,0,0,0,0,0};
//	static U8 modifyTurboKey[8]={0,0,0,0,0,0,0,0};
	#if 1
//	btnIdx=buttonAmbidextrous(btnIdx);                             // Get the ambidextrous button index value
	if(deviceMode != DRIVER_MODE){                                   // Normal mode, many case
		modeData=((btnAssign[btnIdx].mode==MODE_HYPERSHIFT)&&modeHyperShift)?SPECIFIC:NROM;
		if(modeData==SPECIFIC){                                      // Hypershift mode, need to record the button 
			if(eventType==BTN_MAKE){ 
				hyperShiftMark|=(1<<btnIdx);                           // Mark the hypershift button
			}else{ 
				hyperShiftMark&=~(1<<btnIdx);                          // Release the hypershift button
			}
		}
    if(modeData==NROM){                                      // Normal mode, need to record the button 
			if(eventType==BTN_MAKE){ 
				normalMark|=(1<<btnIdx);                             // Mark the Normal button
			}else{ 
				normalMark&=~(1<<btnIdx);                            // Release the Normal button
			}
		}
    if((hyperShiftButtons!=0)&&(modeData==SPECIFIC)&&(eventType==BTN_BREAK)&&(normalMark&(1<<btnIdx))){  // If current mode has been hypershift mode , the previous mode is normal and button is make
      modeData=NROM;                                         // Need to release the pressed button for normal mode first
      normalMark&=~(1<<btnIdx);                              // Release the Normal button(which is pressing when hypershif button press)
    }
		switch(btnAssign[btnIdx].type[modeData]) {
			case BTN_OFF:
			break;
			case BTN_MOUSE:
					if (eventType == BTN_MAKE) {
						if(btnAssign[btnIdx].d1[modeData]==SCROLL_UP){      // Scroll up function 
							if(onTheFlay){                                    // On the fly , need to change 100 dpi every scroll 
								if((dpiCurrent.x+100)<16000){
									dpiCurrent.x+=100;
								}else{
									dpiCurrent.x=16000;
								}
								if((dpiCurrent.y+100)<16000){
									dpiCurrent.y+=100;	
								}else{
									dpiCurrent.y=16000;
								}
							 user.profile[user.ProNO].activeDpi.x = dpiCurrent.x;
							 user.profile[user.ProNO].activeDpi.y = dpiCurrent.y;
							 setResolution(dpiCurrent.x,dpiCurrent.y);
							 requestVariableUpdate(SW_CHANGEDPr,user.ProNO);
							}else{                                            // Scroll even 
								zCount++;
								mouseEvent |= EVENT_SCROLLWHEEL;
							}
						}else if(btnAssign[btnIdx].d1[modeData]==SCROLL_DN){ // Scroll down function 
							if(onTheFlay){                                    // On the fly ,decrease 100 dpi
								if((dpiCurrent.x-100)>0){
									dpiCurrent.x-=100;
								}else{
									dpiCurrent.x=0;
								}
								if((dpiCurrent.y-100)>0){
									dpiCurrent.y-=100;	
								}else{
									dpiCurrent.y=0;
								}				 					
							 user.profile[user.ProNO].activeDpi.x = dpiCurrent.x;
							 user.profile[user.ProNO].activeDpi.y = dpiCurrent.y;
							 setResolution(dpiCurrent.x,dpiCurrent.y);
							 requestVariableUpdate(SW_CHANGEDPr,user.ProNO);
							}else{                                           // Scroll even 
								zCount--;
								mouseEvent |= EVENT_SCROLLWHEEL;
							}
						}else{                                            // Mouse button (left , right , middle , 4, 5, 6, 7) 
						  currentButtons |= (1 << (btnAssign[btnIdx].d1[modeData] - 1));
							mouseEvent |= EVENT_BUTTON;                         // Set new Button information to report	
						}
					} else {
						if(btnAssign[btnIdx].d1[modeData]!=SCROLL_UP&&btnAssign[btnIdx].d1[modeData]!=SCROLL_DN){  // DPI button don't need to release
						  currentButtons &= (~(1 << (btnAssign[btnIdx].d1[modeData] - 1)));
						  mouseEvent |= EVENT_BUTTON;                         // Set new Button information to report	
						}
					}
					 			
			break;
			case BTN_DPI:
				switch(btnAssign[btnIdx].d1[modeData]){
					case BTN_STAGE_UP:
						if (eventType == BTN_MAKE){
							 IRSensor.dpiStage = (IRSensor.dpiStage + 1) < (IRSensor.maxStages-1) ? (IRSensor.dpiStage + 1) : (IRSensor.maxStages-1);
							 dpiCurrent.x=IRSensor.stageDPI[IRSensor.dpiStage].x;           //table[IRSensor.dpiStage];	
							 dpiCurrent.y=IRSensor.stageDPI[IRSensor.dpiStage].y; 
							 setResolution(dpiCurrent.x,dpiCurrent.y);
							 user.profile[user.ProNO].sensor.dpiStage = IRSensor.dpiStage;
							 user.profile[user.ProNO].activeDpi.x = dpiCurrent.x;
							 user.profile[user.ProNO].activeDpi.y = dpiCurrent.y;
							 requestVariableUpdate(USER_CHANGED,user.ProNO);
						}
						break;
					case BTN_STAGE_DN:
							if(eventType == BTN_MAKE){
							 IRSensor.dpiStage = (IRSensor.dpiStage - 1) > 0 ? (IRSensor.dpiStage - 1) : 0;
							 dpiCurrent.x=IRSensor.stageDPI[IRSensor.dpiStage].x;           //table[IRSensor.dpiStage];	
							 dpiCurrent.y=IRSensor.stageDPI[IRSensor.dpiStage].y; 
							 setResolution(dpiCurrent.x,dpiCurrent.y);
							 user.profile[user.ProNO].sensor.dpiStage = IRSensor.dpiStage;
							 user.profile[user.ProNO].activeDpi.x = dpiCurrent.x;
							 user.profile[user.ProNO].activeDpi.y = dpiCurrent.y;
							 requestVariableUpdate(USER_CHANGED,user.ProNO);
						}
						break;
					case BTN_STAGE_NN:
							if(eventType == BTN_MAKE){
							 IRSensor.dpiStage = btnAssign[btnIdx].d2[modeData]-1;
							 if(IRSensor.dpiStage<=(IRSensor.maxStages-1)){
								 dpiCurrent.x=IRSensor.stageDPI[IRSensor.dpiStage].x;           //table[IRSensor.dpiStage];	
							   dpiCurrent.y=IRSensor.stageDPI[IRSensor.dpiStage].y; 	
								 setResolution(dpiCurrent.x,dpiCurrent.y);
								 user.profile[user.ProNO].sensor.dpiStage = IRSensor.dpiStage;
								 user.profile[user.ProNO].activeDpi.x = dpiCurrent.x;
								 user.profile[user.ProNO].activeDpi.y = dpiCurrent.y;
								 requestVariableUpdate(USER_CHANGED,user.ProNO);
							}
						}
						break;
					case BTN_STAGE_LPUP:
						if (eventType == BTN_MAKE){
							 if (++IRSensor.dpiStage == IRSensor.maxStages) IRSensor.dpiStage = 0;
							 dpiCurrent.x=IRSensor.stageDPI[IRSensor.dpiStage].x;           //table[IRSensor.dpiStage];	
							 dpiCurrent.y=IRSensor.stageDPI[IRSensor.dpiStage].y; 
							 setResolution(dpiCurrent.x,dpiCurrent.y);
							 user.profile[user.ProNO].sensor.dpiStage = IRSensor.dpiStage;
							 user.profile[user.ProNO].activeDpi.x = dpiCurrent.x;
							 user.profile[user.ProNO].activeDpi.y = dpiCurrent.y;
							 requestVariableUpdate(USER_CHANGED,user.ProNO);
						}
						break;            
					case BTN_ON_FLY:
						if(eventType == BTN_MAKE){
							onTheFlay=true;
						}else{
							onTheFlay=false;
						}
						break;
					case BTN_VALUE:
						if(eventType == BTN_MAKE){
								dipX = 	btnAssign[btnIdx].d2[modeData];
								dipX = (dipX<<8)|btnAssign[btnIdx].d3[modeData];
								dipY= 	btnAssign[btnIdx].d4[modeData];
								dipY = (dipY<<8)|	btnAssign[btnIdx].d5[modeData];							
								setResolutionTemp(dipX,dipY);    // Add by Felix, avoid that dip will not back to current dpi when latch funtion
						}else{
							 setResolution(dpiCurrent.x,dpiCurrent.y);
						}
						break;
				}
				break;
			case BTN_KYBRD:
				if (eventType == BTN_MAKE) {                          // Make
					//hid_key.keyIn[0] |= (btnAssign[btnIdx].d1[modeData]); // Modify key
					for(k=0;k<8;k++){
						if((btnAssign[btnIdx].d1[modeData]>>k)&0x01){
							modifyKey[k]++;
							hid_key.keyIn[0]|=(1<<k);
						}
					}
					for (i = 2; i < REPORT_SIZE_KB; i++) {        // Find the first free key report buffer 
						if (hid_key.keyIn[i] == NOKEY) {
							hid_key.keyIn[i] = btnAssign[btnIdx].d2[modeData];
							mouseEvent |= EVENT_KEY;                        // Set new Button information to report
							break;
						}
					}
				} else {                                              // Break
					//hid_key.keyIn[0] &= ~(btnAssign[btnIdx].d1[modeData]);
					for(k=0;k<8;k++){
						if((btnAssign[btnIdx].d1[modeData]>>k)&0x01){
              if(modifyKey[k]>0)modifyKey[k]--;						
							if(modifyKey[k]==0){
								hid_key.keyIn[0] &= ~(1<<k);
							}
						}
					}
					for (i = 2; i < REPORT_SIZE_KB; i++) {        // Find the Break key in report buffer
						if (hid_key.keyIn[i] == btnAssign[btnIdx].d2[modeData]) {
							hid_key.keyIn[i] = NOKEY;
							mouseEvent |= EVENT_KEY;                        // Set new Button information to report
							break;
						}
					}
				}
			break;
//		 case BTN_PROFILE:                                 // Profile button
//			 if(eventType==BTN_MAKE){
//				 if(deviceMode!=TEST_MODE){ 
//					switch(btnAssign[btnIdx].d1[modeData] ){				
//						case PROFILE_UP:                             // Profile Switch button 					
//						switchProfile(PROFILE_UP,0);
//						requestVariableUpdate(SW_CHANGEDPr,user.ProNO);
//						break;
//						case PROFILE_DOWN:
//							switchProfile(PROFILE_DOWN,0);
//							requestVariableUpdate(SW_CHANGEDPr,user.ProNO);
//						break;
//						case PROFILE_FIXID:
//							switchProfile(PROFILE_NUM,btnAssign[btnIdx].d2[modeData]);
//							requestVariableUpdate(SW_CHANGEDPr,user.ProNO);
//						break;
//					}	
//			 }else{    // Test mode need to show the profile led different color
//				 profileButtonTest++;
//				 profileBtnTest(profileButtonTest%6);
//			 }				
//			}
//			break;
		case BTN_MACRO_I:                                 // Macro type I ,Run the ID macro "0xNN"(D3) times,D1 D2 are macro ID
			if(eventType == BTN_MAKE){
				if(!buttonIsRunning(btnIdx)){                 // Button macro is not running,creat and run
					type=BTN_MACRO_I;                             // Get macro type
					macroId=(btnAssign[btnIdx].d1[modeData]<<8)|btnAssign[btnIdx].d2[modeData]; //Get macro id
					repeatTimes=btnAssign[btnIdx].d3[modeData];  //Get repeatd times
					activeMacro(type,btnIdx,macroId,repeatTimes);        // Active the button macro
				}
//				else{                                        // Button macro is running, need to reset macro times
//					resetRepeattimes(btnIdx);
//				}
			}
			break;
		case BTN_MACRO_II:                                // Macro typ II ,Display macro till button is released
			if(eventType == BTN_MAKE){                      // Make even
				type=BTN_MACRO_II;                            // Get macro type
				macroId=(btnAssign[btnIdx].d1[modeData]<<8)|btnAssign[btnIdx].d2[modeData]; //Get macro id
				repeatTimes=1;                                // Repeatd times 1 means for keep going macro
				if(!buttonIsRunning(btnIdx)){
					activeMacro(type,btnIdx,macroId,repeatTimes);    // Active the macro
				}
			}else{
				deActiveMacro(btnIdx); // Repeatd times 0 to stop macro
			}
			break;
			case BTN_MACRO_III:                                 // Macro typ III ,Display macro till button is pressed again
				if(eventType == BTN_MAKE){                        // Make even
					 if(!buttonIsRunning(btnIdx)){       // Current key is not active, Then active
						 type=BTN_MACRO_II;                           // Get macro type
						 macroId=(btnAssign[btnIdx].d1[modeData]<<8)|btnAssign[btnIdx].d2[modeData]; //Get macro id			
						 repeatTimes=1;                               // Repeatd times 1 means for keep going macro
						 activeMacro(type,btnIdx,macroId,repeatTimes);      // Active the macro		
					 }else{
						 deActiveMacro(btnIdx);                   // Repeatd times 0 to stop macro
					 }
				}
			break;
//			case BTN_MACRO_IV:                                 // Macro type IV , every make even come out an upload even
//       	if(eventType == BTN_MAKE){                        // Make even
//					 if(!buttonIsRunning(btnIdx)){       // Current key is not active, Then active
//						 type=BTN_MACRO_IV;                           // Get macro type
//						 macroId=(btnAssign[btnIdx].d1[modeData]<<8)|btnAssign[btnIdx].d2[modeData]; //Get macro id			
//						 repeatTimes=1;
//						 activeMacro(type,btnIdx,macroId,repeatTimes);      // Active the macro		
//					 }else{
//						 macroStepGo(btnIdx);
//					 }
//				}
//				break;
//			case BTN_LIGHTING:
//				if(btnAssign[btnIdx].d1[modeData]==BTN_SCHEME_UP){
//					
//				}else if(btnAssign[btnIdx].d1[modeData]==BTN_SCHEME_DOWN){
//				}
//					
//				break;
			case BTN_POWER:
				if(eventType == BTN_MAKE){
					Hid_Ep2_Report_In[0]=2;//0x03;         // Media report id :0x02
					Hid_Ep2_Report_In[1]=btnAssign[btnIdx].d1[modeData];  // Get two bytes media hid usage id
				}else{
					Hid_Ep2_Report_In[0]=2;//0x03;         // Media report id :0x02
					Hid_Ep2_Report_In[1]=0;  // Get two bytes media hid usage id
				}
				 macroEven|=EVENT_POWER;      // Update Even 
				break;
			case BTN_MEDIA:
				if(eventType == BTN_MAKE){
					Hid_Ep2_Report_In[0]=1;//0x02;         // Media report id :0x02
					Hid_Ep2_Report_In[1]=btnAssign[btnIdx].d2[modeData];  // Get two bytes media hid usage id
					Hid_Ep2_Report_In[2]=btnAssign[btnIdx].d1[modeData];
				}else{
					Hid_Ep2_Report_In[0]=1;//0x02;         // Media report id :0x02
					Hid_Ep2_Report_In[1]=0;  // Get two bytes media hid usage id
					Hid_Ep2_Report_In[2]=0;
				}
				 macroEven|=EVENT_MEDIA;      // Update Even 
				break;
			case BTN_XCLICK:
				if(eventType == BTN_MAKE){
					doubleClickButton=1<<(btnAssign[btnIdx].d1[modeData]-1);
					doubleClickCnt=btnAssign[btnIdx].d2[modeData]*2;//0;
					doubleClickEven=1;
				}
				break;
//			case BTN_HYPESHIFT:
//			if(eventType==BTN_MAKE){
//				hyperShiftButtons|=(1<<btnIdx);         // Record the buttons which are hypeshift
//				modeHyperShift=true;
//			}else{
//				hyperShiftButtons&=~(1<<btnIdx);
//				if(hyperShiftButtons==0){               // If all the hypershift buttons are released 
//					for(i=0;i<NUMBER_OF_BUTTONS;i++){
//						if(hyperShiftMark&(1<<i)){          // Button had press in hypershift mode and not be relased
//							processButtonEvent(BTN_BREAK,i);	// Release the pressed button						
//						}
//					}
//					hyperShiftMark=0;                
//					modeHyperShift=false;                 // Disable hypershift
//				}
//			}
//				break;
//			case BTN_TURBO_KEY:
//			 	if (eventType == BTN_MAKE) {                          // Make
//					//hid_key.turboIn[0]|= (btnAssign[btnIdx].d1[modeData]); // Modify key
//					for(k=0;k<8;k++){
//						if((btnAssign[btnIdx].d1[modeData]>>k)&0x01){
//							modifyTurboKey[k]++;
//							hid_key.turboIn[0]|=(1<<k);
//						}
//					}
//					for (i = 2; i < REPORT_SIZE_KB; i++) {        // Find the first free key report buffer 
//						if (hid_key.turboIn[i] == NOKEY) {
//							hid_key.turboIn[i] = btnAssign[btnIdx].d2[modeData];
//							turbo.kb.scratch[btnIdx]=btnAssign[btnIdx].d3[modeData];
//							turbo.kb.scratch[btnIdx]=(turbo.kb.scratch[btnIdx]<<8)|btnAssign[btnIdx].d4[modeData];
//							turbo.kb.timer[btnIdx]=0;
//							turbo.kb.turboButtons++;
//							turbo.kb.stage=TURBO_MAKE;						
//							break;
//						}
//					}
//				} else {                                              // Break
//					//hid_key.turboIn[0] &= ~(btnAssign[btnIdx].d1[modeData]);
//					for(k=0;k<8;k++){
//						if((btnAssign[btnIdx].d1[modeData]>>k)&0x01){
//							if(modifyTurboKey[k]>0)modifyTurboKey[k]--;
//							if(modifyTurboKey[k]==0){
//							  hid_key.turboIn[0]&=~(1<<k);
//							}
//						}
//					}
//					for (i = 2; i < REPORT_SIZE_KB; i++) {        // Find the Break key in report buffer
//						if (hid_key.turboIn[i] == btnAssign[btnIdx].d2[modeData]) {
//							hid_key.turboIn[i] = NOKEY;
//							turbo.kb.scratch[btnIdx]=0;
//							if(turbo.kb.turboButtons>0)turbo.kb.turboButtons--;
//							break;
//						}
//					}
//					if(turbo.kb.turboButtons==0){
//					  turbo.kb.stage=TURBO_END;	
//					}			
//				}
//				break;
//			case BTN_TURBO_BUTTON:
//				if (eventType == BTN_MAKE) {                          // Make
//          //[  slash add for DTS 17_108-60
//            if(btnAssign[btnIdx].d1[modeData]==SCROLL_UP){
//              turbo.btn.zCountcode = 0x01;
//            }else if (btnAssign[btnIdx].d1[modeData]==SCROLL_DN) {
//              turbo.btn.zCountcode = 0xFF;
//            }else {
//              turbo.btn.buttonValue |=(1 << (btnAssign[btnIdx].d1[modeData] - 1));
//            }
//           //]          
////						turbo.btn.buttonValue |= btnAssign[btnIdx].d1[modeData];
//						turbo.btn.scratch[btnIdx]=btnAssign[btnIdx].d2[modeData];
//						turbo.btn.scratch[btnIdx]=(turbo.btn.scratch[btnIdx]<<8)|btnAssign[btnIdx].d3[modeData];
//						turbo.btn.timer[btnIdx]=0;
//						turbo.btn.turboButtons++;
//						turbo.btn.stage=TURBO_MAKE;
//				} else {                                              // Break
//          //[ slash add for DTS 17_108-60
//          if((btnAssign[btnIdx].d1[modeData]==SCROLL_UP) ||(btnAssign[btnIdx].d1[modeData]==SCROLL_DN)){
//            turbo.btn.zCountcode = 0x00;
//          } else {
//            turbo.btn.buttonValue &= (~(1 << (btnAssign[btnIdx].d1[modeData] - 1))); 
//          }       
//          //]          
//					turbo.btn.scratch[btnIdx]=0;
//					if(turbo.btn.turboButtons>0)turbo.btn.turboButtons--;
//					if(turbo.btn.turboButtons==0){
//					  turbo.btn.stage=TURBO_END;
//					}			
//				}
//				break;
		}
 }else{                                                     // Driver mode 
//	 if(btnIdx<=4){                                          // Left ,Right,Middle,Back ,Forwd button
		 	if (eventType == BTN_MAKE) {
				currentButtons |= (1 << btnIdx);// << (dft_generic2.profile[user.ProNO].btnAssign[btnIdx].d1[modeData] - 1));
			} else {
				currentButtons &= (~(1 << btnIdx));// << (dft_generic2.profile[user.ProNO].btnAssign[btnIdx].d1[modeData] - 1)));
			}
			mouseEvent |= EVENT_BUTTON;                         // Set new Button information to report
//	 }else{                                                 //
//		 Hid_Ep2_Report_In[0] = REPORT_ID_RAZER;
//		 mouseEvent|=EVENT_RZ_KEY;                            // DMK ,razer key 
//	 }
	 
 }
	#endif
	
	
	
	// reactive
	if(BTN_MAKE == eventType) {
    for (i = 0; i < NUMBER_OF_LEDS; i++) {
      if ((lighting[i].effect == CHROMA_EFX_REACTIVE) && ((usbReceiveFlag == 0))) {
			  after_glow_ready[lighting[i].index] =0;
				lighting[i].tmr = 0;
        Trail_Parameter[i].TrailPWM =  255;
			}
    }
	}	else {
		for (i = 0; i < NUMBER_OF_LEDS; i++) {
			if ((lighting[i].effect == CHROMA_EFX_REACTIVE) && ((usbReceiveFlag == 0))) {
          after_glow_ready[lighting[i].index] =1;
			    lighting[i].tmr = 0;
					Trail_Parameter[i].Trailflag = 1;
          Trail_Parameter[i].TrailPWM =  255;
					TrailTimerCnt[i] = 0;
			}
    }
	}
}

void GetBtnAssgin(U8 profile ,U8 mode,U8 offset, U8* pData,U8 size)
{
	U8 i;
	for(i=offset;(i<offset+size)&&(i<NUMBER_OF_BUTTONS);i++){
		*(pData++)=user.profile[profile-1].btnAssign[i].id;
		//*(pData++)=user.profile[profile].btnAssign[i].mode;
		*(pData++)=user.profile[profile-1].btnAssign[i].type[mode];
		*(pData++)=user.profile[profile-1].btnAssign[i].size[mode];
		*(pData++)=user.profile[profile-1].btnAssign[i].d1[mode];
		*(pData++)=user.profile[profile-1].btnAssign[i].d2[mode];
		*(pData++)=user.profile[profile-1].btnAssign[i].d3[mode];
		*(pData++)=user.profile[profile-1].btnAssign[i].d4[mode];
		*(pData++)=user.profile[profile-1].btnAssign[i].d5[mode];
	}
}

