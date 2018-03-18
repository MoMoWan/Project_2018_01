/*++

Copyright (c) 2013-2014 Razer Ltd.
All rights reserved. This program contains proprietary and confidential information.

Module Name:
  mouse.c

Compiler:
  Keil Software µVision v5.11, with NXP Semiconductor LPC11U2x proccessor support.

Abstract:
  This file provides the function implement about mouse cursor, scroll decoder.

--*/


#include "main.h"
#include "mouse.h"
//#include "usb_hardware.h"
//#include "usb_core.h"
#include "led_driver.h"
#include "variables.h"
#include "gpio.h"
#include "wdt.h"
#include "button.h"

#include "avago_api.h"
//#include "spi0_driver.h"
//#include "usb_costom.h"

#include "macro.h"
#include "keyboard.h"
#include "SPI.h"
#include "usbuser.h" 
#include "usbepfunc.h"

S16 xCount;
S16 yCount;
S8  zCount;

U8  mouseEvent;
U8  razerEvent;
U8  optical;
  
U8  prevScroll;
U8  currScroll;
U8  prevRotate;
U8  currRotate;



U8  Hid_Ep1_Report_In[MAX_EP_SIZE]__attribute__((aligned(4)));
U8  Hid_Ep2_Report_In[MAX_EP_SIZE]__attribute__((aligned(4)));
U8  Hid_Ep3_Report_In[MAX_EP_SIZE]__attribute__((aligned(4)));




SENSOR_PAR IRSensor;
DPI_PAR dpiCurrent;

U8 sampleCountSqual;
U16 averageSqual;
U16 averageDeltaXY;
//U16 liftOffThreshold;
//U16 MEASURERESULT;
//U8 minValueSqual;
U8 RazerKey;
U8 AutoCalibrationFalg;
U8 mouseLifted;								//Mouse lift Flag
U8 FilterState;
U8 ResetSucessFlag;

U32 SumSqual =0;
U32 SumDeltaXY = 0;
U8 OverSpeedMode = 0;
U16 averageDeltaXY = 0;
U16 HaltTimer1 = 0;
U32 HaltTimer2 = 0;
U16 CalibrationTimer = 0;
U8 ResetHalFlag = 0;
U8 changeDPI;
U16 FilterCount;
U8 minValueSqual2;
U8 startFlag = 0x55;
U8 maxShutterHi = 0;
U8 minShutterLo = 0;
U8 manualState = 0;
U8 clearFlag;
U8 MouseButtonBuf=0;

U8 CALSTAT = 0;
U8 DisableCalibraiton = 0;
//[ sesor PWM3389
U8 CAL_LiftConfig = 0;                                    // Reg 0x63
U8 CAL_LiftCal2 = 0;                                      // Reg 0x65
U8 CAL_LiftCal3 = 0;                                      // Reg 0x41
U8 CAL_Threshold = 0;                                     // Reg 0x76
U8 CAL_LiftCal4 = 0;                                      // Reg 0x77
//]
SCROLL scrollTimer;                                       // Use for scroll release even triger
void CalcAverageSQUAL(void) ;



U8 getBtnTransKey(void)
{
 U8 i = 1;
 
  if (Hid_Ep2_Report_In[0] == REPORT_ID_RAZER) {          // Razer key event, we only has 3 key total, assign them to idx 1 2 3 to EP buffer
    if (buttonStatus[BTN_DPIDN_IDX].active) {
      Hid_Ep2_Report_In[i++] = DPI_DN_KEY;     
    }
    if (buttonStatus[BTN_DPIUP_IDX].active) {
      Hid_Ep2_Report_In[i++] = DPI_UP_KEY;
    }
//		if(user.ambidextrous==RIGHT_HAND){                   // Thumb button for right hand
//			if(buttonStatus[BTN_RBCK_RH].active){
//				 Hid_Ep2_Report_In[i++] = BTN_RBCK_KEY;
//			}
//			if(buttonStatus[BTN_RFWD_RH].active){
//				 Hid_Ep2_Report_In[i++] = BTN_RFWD_KEY;
//			}
//	 }else{                                             // Thumb button for left hand
		 	if(buttonStatus[BTN_RBCK_LH].active){
				 Hid_Ep2_Report_In[i++] = BTN_RBCK_KEY;
			}
			if(buttonStatus[BTN_RFWD_LH].active){
				 Hid_Ep2_Report_In[i++] = BTN_RFWD_KEY;
			}
//	 }
//		if(buttonStatus[BTN_PROFILE_IDX].active){
//			 Hid_Ep2_Report_In[i++] = BTN_PROFILE_KEY;
//		}
  }
  return i;
}  

void initializeMouseVarialbes(void)
/*++

Function Description:
  This function init the variables used by mouse XYZ and button scanning and report buffers
   
Arguments:
  NONE
  
Returns: 
  NONE

--*/
{
  U8 i;
	
//  SSP0_Enable();                                          // Enable SPI for sensor
  SPI0_Enable();
  xCount = yCount = zCount = 0;                           // displacement values
  mouseEvent = 0;                                         // Mouse event
  optical = 0;                                            // polling rate timing 
  sensor_X = sensor_Y = 0;
  
	sampleCountSqual = 0;
	averageSqual = 0;
	OverSpeedMode = 0;
	
	AutoCalibrationFalg = 0;
	clearFlag = 0;
	
//	minValueSqual = 0xA9;
//	MEASURERESULT = 0xA9;
	
	mouseLifted = UFALSE;

  for (i = 0; i < sizeof(Hid_Ep1_Report_In); i++) {       // report
    Hid_Ep1_Report_In[i] = 0x00;
    Hid_Ep2_Report_In[i] = 0x00;
		Hid_Ep3_Report_In[i] = 0x00;
  }
	memset((U8*)&turbo,0,sizeof(turbo));
	memset((U8*)&scrollTimer,0,sizeof(scrollTimer));
  prevScroll = currScroll = 0;                            // Initialize Scroll Wheel state variables
  prevRotate = currRotate = 0; 
//	__disable_irq();                                      // Disable interrupts while flashing
  updateSensorFirmware();                                //
//	__enable_irq(); 
}




void copyLiftoff2Active(U8 function)
/*++

Function Description:
  This function copy lift off parameter
Arguments:
  NONE

  Profile 0 : ramP0.irsensor
  Profile 1/2/3: user.profile[user.PClass].sensor
  
 result: IRSensor

Returns: 
  NONE

--*/
{
  if(function == 0x01) {                                    // copy user function to user that will be require save
	  user.profile[user.ProNO].sensor = IRSensor;
		user.profile[user.ProNO].activeDpi = dpiCurrent;
  } else if(function == 0x00) {                              // copy the saved function from user to active
	 	IRSensor = user.profile[user.ProNO].sensor;
	  dpiCurrent = user.profile[user.ProNO].activeDpi;
  }
}



void opticalSensorPolling(void)
/*++
	Function Description:
  		This function first checks if it is time to poll the optical sensor (pollingRate),
  		then serially communicates with the optical sensor to get the current X & Y delta
  		information. If there is any activity it is so tagged and the USB report will be
  		build and sent accordingly.
 
	Arguments:
  		NONE

	Returns:
  		NONE
--*/
{	
  if (optical >= user.pollingrate) {
    optical = 0;                                          // Reset optical sensor polling rate
    sensorProgramAPI();                                   // Give sensor time to update config & displacement
		if((mouseLifted == FALSE) ||(mouseLifted == UFALSE)) {
				xCount += sensor_X;                               // Get new X value
				yCount += sensor_Y;                               // Get new Y value
     }
    sensor_X = sensor_Y = 0;                              // Clear accumulative displacement values
    
		CalcAverageSQUAL();
 
		if(clearFlag == 0x01) {
      xCount = yCount = 0;          
      clearFlag = 0x0;
		}
		if ((xCount != 0) || (yCount != 0)) { 				        // If there is any data to report, then...
		  mouseEvent |= EVENT_POSITION; 
	  }
  }
}

void requestNtfEvent(U8 type, U8 value) 
/*++

Function Description:
  This function for notification event processing. 
  It will save the notification event values and set the notification event flag.
 
Arguments:
  type  - notification event type
  value - notification evet value

Returns: 
  NONE

--*/
{
  ntfValue    = value;
  ntfType     = type;
  mouseEvent |= EVENT_NTF;
}



void scrollWheelHandling(void)
/*++

Function Description:
  This function is result of a Falling or Rising Edge on both the ENCODER_UP and 
  ENCODER_DN pins, and manages the Scroll Wheel both in count and direction. All
  noise generated by the encoder is eliminated, by only allowing valid states in
  the current direction. Two lines defines 4 states of which only two of them can the
  direction be determined (0, 3, -3, 0). By using the previous state information we
  can determine Clockwise (zCount++) or Counter Clockwise based on 4 different sets
  of this basic grouping (0, 3, -3, 0).
 
Arguments:
  NONE

Returns: 
  NONE

--*/
{
  U8 dir;
  U8 table[16] = {0x00,0xFD,0x03,0x00,0x03,0x00,0x00,0xFD,0xFD,0x00,0x00,0x03,0x00,0x03,0xFD,0x00};

  currScroll = (ENCODER_DN << 1) | ENCODER_UP;            // Get current state of ENCODER_DN and ENCODER_UP
  dir = table[((prevScroll << 2) | currScroll)];          // Get Encoder rotational value

  prevScroll = currScroll;                                // Save current Scroll Wheel state
  if (dir != 0) {                                         // If there is movement, validate it
    currRotate = dir;                                     // Save new rotational direction                                          
    dir = currRotate + prevRotate;                        // Isolate rotation in same direction
    prevRotate = currRotate;                              // Save current direction
    prevScroll = currScroll;                              // Save current state
    if (dir != 0) {                                       // If there is a net of movement, then...
      if ((dir & 0x01) == 0) {
        if (dir == 0xFA) {                                // Counter Clockwise (0xFD + 0xFD), Scoll Dn
				 if(deviceMode != DRIVER_MODE){                   // Driver mode , upload the default value , scroll value
					processButtonEvent(BTN_MAKE, SCROLL_DN_IDX);    // Scroll even deal 
					scrollTimer.dnTimer=2;                          // 2ms later will release the scroll even 
					scrollTimer.dnFlag=true;                        // Flag
				}else{                                            // Driver mode , default behave
					  zCount--;
            mouseEvent |= EVENT_SCROLLWHEEL;
				}
        } else if (dir == 0x06) {                         // Clockwise (0x03 + 0x03), Scroll Up
				if(deviceMode != DRIVER_MODE){                    // Driver mode , upload the default value , scroll value
					processButtonEvent(BTN_MAKE,SCROLL_UP_IDX);     // Scroll even deal 
					scrollTimer.upTimer=2;                          // 2ms later will release the scroll even 
					scrollTimer.upFlag=true;                        // Flag
				}else{                                            // Driver mode , default behave
					  zCount++;
            mouseEvent |= EVENT_SCROLLWHEEL;
				}
        }
        prevRotate = 0;
      }
    }
  }
}


void setResolution(U16 x,U16 y)
/*++

Function Description:
  This function sets sensor resolution. The Sensor handles settings 800 cpi and 
  1800 cpi. The lowest setting of 400 cpi is achieved mathematically by dividing
  800 cpi displacement values by two.
 
Arguments:
  NONE

Returns: 
  NONE

--*/
{
	 dpiCurrent.x = x;
   dpiCurrent.y = y;
	
//[ for PWM3389 Sensor  
  // must first high then low, Jolie 20160824
  SPI_W_2BYTE(REG_RESOLUTION_H, (y/50)>> 8); 
	SPI_W_2BYTE(REG_RESOLUTION_L, (U8)(y/50));
  
  SPI_W_2BYTE(REG_CONFIG5_H,(x/50)>> 8);
	SPI_W_2BYTE(REG_CONFIG5_L,(U8)(x/50));

//]  
  requestNtfEvent(NTF_DPI, 0);

//  mouseEvent = 0;                                         // Clear mouse event make the write there are no motion data
//  xCount = yCount = 0;                                    // The motion data will infulence the  x y
}

void setResolutionTemp(U16 x,U16 y)
/*++

Function Description:
  This function sets sensor resolution. The Sensor handles settings 800 cpi and 
  1800 cpi. The lowest setting of 400 cpi is achieved mathematically by dividing
  800 cpi displacement values by two.
 
Arguments:
  NONE

Returns: 
  NONE

--*/
{
//	 dpiCurrent.x = x;
//   dpiCurrent.y = y;
	
//[ for PWM3389 Sensor  
  // must first high then low, Jolie 20160824
  SPI_W_2BYTE(REG_RESOLUTION_H, (y/50)>> 8); 
	SPI_W_2BYTE(REG_RESOLUTION_L, (U8)(y/50));
  
  SPI_W_2BYTE(REG_CONFIG5_H,(x/50)>> 8);
	SPI_W_2BYTE(REG_CONFIG5_L,(U8)(x/50));

//]  
  requestNtfEvent(NTF_DPI, 0);

//  mouseEvent = 0;                                         // Clear mouse event make the write there are no motion data
//  xCount = yCount = 0;                                    // The motion data will infulence the  x y
}

void manualCalibration(void)
{
 U8 temp = 0;
//  if ((calEnable == ENABLE) && (calStatus != CAL_S_OFF) ) {
//    switch (calStatus) {
//      case CAL_S_PREPARE:                                 // noraml step1 click to start... 
//        startFlag = 0;                                  // stop mouse
//        requestNtfEvent(NTF_CAL_ID, NTF_CAL_START);       // Notify driver USB that sensor mini calibration finish, Z data reporting start
//        SPI_W_2BYTE(REG_LIFT_CONFIG,0x02);               // Enable Present Cal and set lift to Universal 3mm		                                     
//        CAL_LiftConfig = 0x02;
//        SPI_W_2BYTE(REG_PIXEL_THRESHOLD,0x0A);           // Set Pixel Threshold to default Pixel Threshold
//        SPI_W_2BYTE(REG_LIFTCUTOFF_CAL1, 0x80);          // start the calibration procedure
//        calStatus = CAL_S_START;
//      break;
//      case CAL_S_START:
//				break;
//      case CAL_S_END:
//        CALSTAT = SPI_R_BYTE(REG_LIFTCUTOFF_CAL1) & 0x07;
//				if (( CALSTAT == 0x02) || (CALSTAT == 0x03)) {
//					SPI_W_2BYTE(REG_LIFTCUTOFF_CAL1, 0);              // stop the calibration procedure
//					delayMilliseconds(2);                              // wait 1msec
//					CAL_LiftCal2 = (SPI_R_BYTE(REG_LIFTCUTOFF_CAL2) & 0x7F);   // Reg 0x65
//					CAL_LiftCal3 = SPI_R_BYTE(REG_LIFTCUTOFF_CAL3);   // Reg 0x41
//					CAL_LiftCal4 = SPI_R_BYTE(REG_LIFTCUTOFF_CAL4);   // Reg 0x77
//					SPI_W_2BYTE(REG_CONFIGURATION_II,0x04);
//					WriterMotionBurstflag = 0;
//					sensorProgramAPI();                                // Give sensor time to update config & displacement
//					xCount = yCount  = 0;                              // Reset displacement values 
//					sensor_X = 0;
//					sensor_Y = 0;
//					setResolution(dpiCurrent.x,dpiCurrent.y);
//					requestNtfEvent(NTF_CAL_ID, NTF_CAL_END);          // Notify driver USB for stop calibration
//					calStatus = CAL_S_ZDATA;
//				} else if (CALSTAT == 0x04) {
//					calStatus = CAL_S_ABORT;                        // calibration fail
//				} else if (PIN_LFT == 0) {
//					calStatus = CAL_S_ABORT;                        // when manual calibration,click to stop
//				}          
//      break;      
//      case CAL_S_ZDATA:
//        startFlag = 0x55;
//        calStatus = CAL_S_OFF;
//      break;
//      case CAL_S_ABORT:
//        SPI_W_2BYTE(REG_LIFTCUTOFF_CAL1, 0);              // stop the calibration procedure
//        SPI_W_2BYTE(REG_LIFT_CONFIG,0x02);               // To disable Preset Calibration, and set lift to Universal 2mm
//        SPI_W_2BYTE(REG_PIXEL_THRESHOLD,0x0A);
//        SPI_W_2BYTE(REG_MIN_SQUAL_RUN,0x10);             // reset manual calibration
//        SPI_W_2BYTE(REG_CONFIGURATION_II,0x04);
//        requestNtfEvent(NTF_CAL_ID, NTF_CAL_HALT);        // Halt Notify
//        calStatus = CAL_S_OFF;
//        startFlag = 0x55;
//      break;
//      default:
//        calStatus = CAL_S_OFF;
//      break;
//    }
//  }
  if ((calEnable == ENABLE) && (calStatus < 2) ) {
   
     SPI_W_2BYTE(REG_LIFTCUTOFF_CAL1, 0x80);          // start the calibration procedure  
     do {
       temp = (SPI_R_BYTE(REG_LIFTCUTOFF_CAL1) & 0x07);
       wdt_resetTimer();                                   // If Watchdog Timer expires the MCU resets
       delayMilliseconds(1);                              // wait 1msec
     } while(temp < 2);
     if (temp == 4){
       calStatus = 3;                                    // calibration fail
     } else {
       calStatus = 2;                                    // calibration sucess 
     }
     SPI_W_2BYTE(REG_LIFTCUTOFF_CAL1, 0);              // stop the calibration procedure
     delayMilliseconds(2);                              // wait 1msec
     CAL_LiftCal2 = (SPI_R_BYTE(REG_LIFTCUTOFF_CAL2) & 0x7F);   // Reg 0x65
     CAL_LiftCal3 = SPI_R_BYTE(REG_LIFTCUTOFF_CAL3);   // Reg 0x41 
     CalibrationDate(CAL_LiftCal2,CAL_LiftCal3,CAL_LiftConfig,CAL_Threshold);   
  }
}

void CalcAverageSQUAL(void) 
/*--
  Function descriptor: Function  is for  power on auto calibrate liftoff SQUAL value
--*/
{
  U8 tempX, tempY;
	SumSqual += surfaceQual;
	tempX = (U8)(xCount);
	tempY = (U8)(yCount);
	tempX = (U8)ABSD(tempX);
	tempY = (U8)ABSD(tempY);
	SumDeltaXY += (tempX * ((dpiCurrent.x >> 2) + 1) + tempY *((dpiCurrent.y >> 2) + 1));
	if (++sampleCountSqual == 16){
		averageSqual = (U16)( SumSqual >> 4);
		averageDeltaXY = (U16)(SumDeltaXY >> 4);
		if(averageDeltaXY > 318) {
			OverSpeedMode = 0x01;                               // OverSpeed
		} else {
		  OverSpeedMode = 0x00;                               // Normal Speed
		}
		sampleCountSqual = 0;
		SumSqual = 0;
		SumDeltaXY = 0;
	}
}


void AutoCalibrationReset(void)
{
	U8 reset = FALSE;
  if (calEnable == ENABLE) { 
		// calibration state enable
	  if (deviceMode == DRIVER_MODE) {                      // diver mode
      if ((PIN_MID == 0) && (PIN_RGT == 0) && (PIN_LFT == 0)) {
        if(ResetHalFlag==0){
          CalibrationTimer = 0;
          ResetHalFlag = 1;
          flag_Scroll_Wheel_Led_Blink = 0;
        }
        if ( CalibrationTimer >= 5000) {                  // press 5S
				  reset = TRUE;
				  flag_Scroll_Wheel_Led_Blink = 0;
			  } else {
				  flag_Scroll_Wheel_Led_Blink = 1;
			  }
      } else if (DisableCalibraiton == 1) {
        reset  = TRUE;
      }else if(CalibrationTimer<5000){
				flag_Scroll_Wheel_Led_Blink = 0;
				ResetHalFlag=0;
			}
    } else {                                              // not driver , disable calibration...
      reset  = TRUE;
    }
		if (reset == TRUE) {
      calStatus = CAL_S_OFF;
      startFlag = 0x55;
      SPI_W_2BYTE(REG_LIFTCUTOFF_CAL1, 0);              // stop the calibration procedure
      CAL_LiftCal2 = 0x0A;
      CAL_LiftCal3 = 0x10;
      CAL_LiftConfig = 0x02;
      CAL_Threshold = 0x10;
      CAL_LiftCal4 = 0;
      CalibrationDate(CAL_LiftCal2,CAL_LiftCal3,CAL_LiftConfig,CAL_Threshold);
     
      CalibrationTimer = 0;
      ResetHalFlag = 0;
      if (DisableCalibraiton == 0) {
		    requestNtfEvent(NTF_CAL_ID, NTF_CAL_RESET);       // Notify driver USB
        Hid_Ep2_Report_In[0] = REPORT_ID_NTF;
        razerEvent |= EVENT_RESET;
      }
			calEnable = DISABLE;
      DisableCalibraiton = 0;
    }
  }
} 

void CalibrationDate(U8 cal2,U8 cal3,U8 config, U8 cal4) 
{
//  SPI_W_2BYTE(REG_LIFT_CONFIG,config);                   // write to register 0x63
//  SPI_W_2BYTE(REG_PIXEL_THRESHOLD,cal2);                 // write register 0x65 data to register 0x2C
//  SPI_W_2BYTE(REG_MIN_SQUAL_RUN,cal3);                   // write register 0x41 data to register 0x2B
//  SPI_W_2BYTE(REG_SQUAL_THRESHOLD,cal4);                 // write to register 0x76
//  SPI_W_2BYTE(REG_CONFIGURATION_II,0x04);
  SPI_W_2BYTE(REG_PIXEL_THRESHOLD,cal2);                 // write register 0x65 data to register 0x2C
  SPI_W_2BYTE(REG_MIN_SQUAL_RUN,cal3);                   // write register 0x41 data to register 0x2B  
}
void processTurboTasks(void)
{
	U8 i;
	U8 static makeFlag=0;
	switch(turbo.kb.stage){
		case TURBO_MAKE:
		for(i=0;i<NUMBER_OF_BUTTONS;i++){
			if((turbo.kb.timer[i]>=turbo.kb.scratch[i])&&(turbo.kb.scratch[i]!=0)){
				if (isEpBusy(3)==0){
          memset((U8*)&turbo.kb.timer,0,sizeof(turbo.kb.timer));	
				  fillKeyHid(Hid_Ep3_Report_In,KEY_IN|MACRO_IN|TURBO_IN);
//         USB_WriteEP(USB_ENDPOINT_IN(3), Hid_Ep3_Report_In, REPORT_SIZE_KB);
          if (USB_EPnINFunction(USB_EP3,(U32*)&Hid_Ep3_Report_In[0], REPORT_SIZE_KB) == EPn_RETURN_ACK_OK) { 
            turbo.kb.stage=TURBO_BREAK;		
            makeFlag|=0x01;		         			
  //				 setEpBusy(3);
          }
				}					
				break;
			}
		}
			break;
		case TURBO_BREAK:
			if (isEpBusy(3)==0){
				fillKeyHid(Hid_Ep3_Report_In,KEY_IN|MACRO_IN);
//        USB_WriteEP(USB_ENDPOINT_IN(3), Hid_Ep3_Report_In, REPORT_SIZE_KB);
        if (USB_EPnINFunction(USB_EP3,(U32*)&Hid_Ep3_Report_In[0], REPORT_SIZE_KB) == EPn_RETURN_ACK_OK) {
          turbo.kb.stage=TURBO_MAKE;		
          makeFlag&=~0x01;		    		
//          setEpBusy(3);
        }
			}
			break;
		case TURBO_END:
			if(makeFlag&0x01){
				if(isEpBusy(3)==0){
				  fillKeyHid(Hid_Ep3_Report_In,KEY_IN|MACRO_IN);
//          USB_WriteEP(USB_ENDPOINT_IN(3), Hid_Ep3_Report_In, REPORT_SIZE_KB);
          if (USB_EPnINFunction(USB_EP3,(U32*)&Hid_Ep3_Report_In[0], REPORT_SIZE_KB) == EPn_RETURN_ACK_OK) {
            turbo.kb.stage=TURBO_NONE;			
//            setEpBusy(3);
          }
				}
			}else{        // The previous stage is release
				turbo.kb.stage=TURBO_NONE;	 
			}
			break;
	}
	switch(turbo.btn.stage){
		case TURBO_MAKE:
		for(i=0;i<NUMBER_OF_BUTTONS;i++){
			if((turbo.btn.timer[i]>=turbo.btn.scratch[i])&&(turbo.btn.scratch[i]!=0)){		
         if(isEpBusy(1)==0){				
				   memset((U8*)&turbo.btn.timer,0,sizeof(turbo.btn.timer));		
					 Macro_Ep1_In[BTNS]=(turbo.btn.buttonValue|macroButton|(currentButtons&0x1f)|doubleClickButton);
					 Macro_Ep1_In[Z_SM] = turbo.btn.zCountcode; 
           
//           USB_WriteEP(USB_ENDPOINT_IN(1),Macro_Ep1_In, REPORT_SIZE_MOUSE); 
           if (USB_EPnINFunction(USB_EP1,(U32*)&Macro_Ep1_In[0], REPORT_SIZE_MOUSE) == EPn_RETURN_ACK_OK) {
             turbo.btn.stage=TURBO_BREAK;
             makeFlag|=0x02;
//					 setEpBusy(1);
           }
         }					 
				 break;
			}
		}
			break;
		case TURBO_BREAK:
			 if(isEpBusy(1)==0) {	
				 Macro_Ep1_In[BTNS]=(macroButton|(currentButtons&0x1f)|doubleClickButton);
				 Macro_Ep1_In[Z_SM] = 0; 
				  if ((turbo.btn.buttonValue !=0) || (Macro_Ep1_In[Z_SM] != 0))
//         USB_WriteEP(USB_ENDPOINT_IN(1),Macro_Ep1_In, REPORT_SIZE_MOUSE); 
//				 setEpBusy(1);
        if (USB_EPnINFunction(USB_EP1,(U32*)&Macro_Ep1_In[0], REPORT_SIZE_MOUSE) == EPn_RETURN_ACK_OK) {  
				  makeFlag&=~0x02;
				  turbo.btn.stage=TURBO_MAKE;
        }
			 }
			break;
		case TURBO_END:
			if(makeFlag&0x02){	
				if(isEpBusy(1)==0){
				 Macro_Ep1_In[BTNS]=(macroButton|(currentButtons&0x1f)|doubleClickButton);
				 Macro_Ep1_In[Z_SM] = 0; 
//         USB_WriteEP(USB_ENDPOINT_IN(1),Macro_Ep1_In, REPORT_SIZE_MOUSE); 
//				 setEpBusy(1);
         if (USB_EPnINFunction(USB_EP1,(U32*)&Macro_Ep1_In[0], REPORT_SIZE_MOUSE) == EPn_RETURN_ACK_OK) {
           mouseEvent |= EVENT_BUTTON; 
				   turbo.btn.stage=TURBO_NONE;
         }
				}
			}else{
				turbo.btn.stage=TURBO_NONE;
			}
			break;
	}
}
void processDoubleClickTasks(void)
{
  static U8 doubleClickflag = 0;
		if((doubleClickEven==1)&&(isEpBusy(1)==0)){    // Double click even
		//doubleClickCnt++;
//		switch(doubleClickCnt){
//			case 1:                                    // First click even
//				memset(&Hid_Ep1_Report_In[0],0,REPORT_SIZE_MOUSE);
//			  Hid_Ep1_Report_In[0]=doubleClickButton|(turbo.btn.buttonValue|macroButton|(currentButtons&0x1f));
////			  USB_WriteEP(USB_ENDPOINT_IN(1), &Hid_Ep1_Report_In[0], REPORT_SIZE_MOUSE);
////			  setEpBusy(1);
//				break;
//			case 2:                                    // Release click even
//				  Hid_Ep1_Report_In[0]=(turbo.btn.buttonValue|macroButton|(currentButtons&0x1f));
////			  USB_WriteEP(USB_ENDPOINT_IN(1), &Hid_Ep1_Report_In[0], REPORT_SIZE_MOUSE);
////			  setEpBusy(1);
//				break;
//			case 3:                                    // Second click even
//				Hid_Ep1_Report_In[0]=doubleClickButton|(turbo.btn.buttonValue|macroButton|(currentButtons&0x1f));
////			  USB_WriteEP(USB_ENDPOINT_IN(1), &Hid_Ep1_Report_In[0], REPORT_SIZE_MOUSE);
////			  setEpBusy(1);
//				break;
//			case 4:                                    // Release click even
//				Hid_Ep1_Report_In[0]=(turbo.btn.buttonValue|macroButton|(currentButtons&0x1f));
////			  USB_WriteEP(USB_ENDPOINT_IN(1), &Hid_Ep1_Report_In[0], REPORT_SIZE_MOUSE);
////			  setEpBusy(1);
//				doubleClickEven=0;
//			  doubleClickCnt=0;
//			  doubleClickButton=0;
//				break;
//		}
      if (doubleClickCnt) {
//        Hid_Ep1_Report_In[0] = 0; 
        if (doubleClickflag) {
				  Hid_Ep1_Report_In[0] = (turbo.btn.buttonValue|macroButton|(currentButtons&0x1f));
          
        } else {
          Hid_Ep1_Report_In[0] = doubleClickButton|(turbo.btn.buttonValue|macroButton|(currentButtons&0x1f));
        }
//        mouseEvent |= EVENT_BUTTON;
        if (USB_EPnINFunction(USB_EP1,(U32*)&Hid_Ep1_Report_In[0], REPORT_SIZE_MOUSE) == EPn_RETURN_ACK_OK) {        
          doubleClickCnt--;
          doubleClickflag ^= 1;
        }
      } else {
        doubleClickflag = 0;
 				doubleClickEven=0;
			  doubleClickCnt=0;
			  doubleClickButton=0;       
      }
	}
}
void processMacroTasks(void)
{
	if((macroEven&MACRO_BUTTON_EVEN)&&(isEpBusy(1)==0)){          // Power even
//		macroEven&=~MACRO_BUTTON_EVEN;
		Macro_Ep1_In[BTNS] = (turbo.btn.buttonValue|macroButton|(currentButtons&0x1f)|doubleClickButton);                   // Fill in current Button state, Only L+M+R buttons
    Macro_Ep1_In[Z_SM] = macroScroll;                    // Fill in Z displacement (byte)	
		
		if (macroXCursor < -127) {
      Macro_Ep1_In[X_SM] = (U8)-127;               // Fill in max X negative displacement
    } else if (macroXCursor > 127) {
      Macro_Ep1_In[X_SM] = 127;                    // Fill in max X positive displacement
    } else {
      Macro_Ep1_In[X_SM] = (U8)(macroXCursor);           // Fill in X displacement (byte)
    }
    if (macroXCursor < -127) {
      Macro_Ep1_In[Y_SM] = (U8)-127;               // Fill in max Y negative displacement
    } else if (macroYCursor > 127) {
      Macro_Ep1_In[Y_SM] = 127;                    // Fill in max Y positive displacement
    } else {
      Macro_Ep1_In[Y_SM] = (U8)(macroYCursor);           // Fill in Y displacement (byte)
    } 
		Macro_Ep1_In[X_LO] = macroXCursor;   // Fill in X displacement (word)
    Macro_Ep1_In[X_HI] = macroXCursor>>8; 
    Macro_Ep1_In[Y_LO] = macroYCursor;    // Fill in Y displacement (word)
    Macro_Ep1_In[Y_HI] = macroYCursor>>8; 
		macroYCursor=0;
		macroXCursor=0;
		macroButton=0;
		macroScroll=0;
//		USB_WriteEP(USB_ENDPOINT_IN(1), &Macro_Ep1_In[0], MACRO_EP1_SIZE);
//		setEpBusy(1);
    if (USB_EPnINFunction(USB_EP1,(U32*)&Macro_Ep1_In[0], MACRO_EP1_SIZE) == EPn_RETURN_ACK_OK) {
      macroEven&=~MACRO_BUTTON_EVEN;
    }    
	}
	if((macroEven&MACRO_KEY_EVEN)&&(isEpBusy(3)==0)){              // Macro key even
//		macroEven&=~MACRO_KEY_EVEN;
		fillKeyHid(&Hid_Ep3_Report_In[1],KEY_IN|MACRO_IN|TURBO_IN);
    Hid_Ep3_Report_In[0] = 3;
//		USB_WriteEP(USB_ENDPOINT_IN(3), &Hid_Ep3_Report_In[0], MACRO_EP3_SIZE);  
//		setEpBusy(3);
    if (USB_EPnINFunction(USB_EP2,(U32*)&Hid_Ep3_Report_In[0], MACRO_EP3_SIZE) == EPn_RETURN_ACK_OK) {
      macroEven&=~MACRO_KEY_EVEN;
    }    
	}
	if((macroEven&MACRO_MEDIA_EVEN)&&(isEpBusy(2)==0)){            // Macro media even
//		macroEven&=~MACRO_MEDIA_EVEN;
//		USB_WriteEP(USB_ENDPOINT_IN(2), &Macro_Ep2_In[0], MACRO_EP2_SIZE);
//		setEpBusy(2);
    if (USB_EPnINFunction(USB_EP2,(U32*)&Macro_Ep2_In[0], MACRO_EP3_SIZE) == EPn_RETURN_ACK_OK) {//MACRO_EP2_SIZE
      macroEven&=~MACRO_MEDIA_EVEN;
    }    
	}
	if((macroEven&MACRO_MEDIA_EVEN)&&(isEpBusy(2)==0)){          // Power even
		macroEven&=~MACRO_MEDIA_EVEN;
//		USB_WriteEP(USB_ENDPOINT_IN(2), &Macro_Ep2_In[0], MACRO_EP2_SIZE);
//		setEpBusy(2);
    if (USB_EPnINFunction(USB_EP2,(U32*)&Macro_Ep2_In[0], MACRO_EP3_SIZE) == EPn_RETURN_ACK_OK) {  //MACRO_EP2_SIZE
      macroEven&=~MACRO_MEDIA_EVEN;
    }    
	}
}
void processMouseTasks(void)
{
	U8 i;
  if (((mouseEvent & (EVENT_BUTTON | EVENT_POSITION | EVENT_SCROLLWHEEL)) != 0)&&(isEpBusy(1)==0)) { // If there is something to report, then...	 
//    mouseEvent &= ~(EVENT_BUTTON | EVENT_POSITION | EVENT_SCROLLWHEEL);
//		MouseButtonBuf = currentButtons; 
//		MouseButtonBuf &= 0x1f;
		MouseButtonBuf=(turbo.btn.buttonValue|macroButton|(currentButtons&0x1f)|doubleClickButton);
    Hid_Ep1_Report_In[BTNS] = MouseButtonBuf;           // Fill in current Button state, Only L+M+R buttons
    Hid_Ep1_Report_In[Z_SM] = zCount;                   // Fill in Z displacement (byte)
    if (xCount < -127) {
      Hid_Ep1_Report_In[X_SM] = (U8)-127;               // Fill in max X negative displacement
    } else if (xCount > 127) {
      Hid_Ep1_Report_In[X_SM] = 127;                    // Fill in max X positive displacement
    } else {
      Hid_Ep1_Report_In[X_SM] = (U8)(xCount);           // Fill in X displacement (byte)
    }
    if (yCount < -127) {
      Hid_Ep1_Report_In[Y_SM] = (U8)-127;               // Fill in max Y negative displacement
    } else if (yCount > 127) {
      Hid_Ep1_Report_In[Y_SM] = 127;                    // Fill in max Y positive displacement
    } else {
      Hid_Ep1_Report_In[Y_SM] = (U8)(yCount);           // Fill in Y displacement (byte)
    } 
    Hid_Ep1_Report_In[X_LO] = (U8)(xCount);             // Fill in X displacement (word)
    Hid_Ep1_Report_In[X_HI] = (U8)(xCount >> 8);
    Hid_Ep1_Report_In[Y_LO] = (U8)(yCount);             // Fill in Y displacement (word)
    Hid_Ep1_Report_In[Y_HI] = (U8)(yCount >> 8);
    xCount = yCount = zCount = 0;                       // Reset displacement values 
#if (EP_OVERWRITE_PROTECT)    
//    setEpBusy(1);
#endif
//    USB_WriteEP(USB_ENDPOINT_IN(1), &Hid_Ep1_Report_In[0], REPORT_SIZE_MOUSE);
    if (USB_EPnINFunction(USB_EP1,(U32*)&Hid_Ep1_Report_In[0], REPORT_SIZE_MOUSE) == EPn_RETURN_ACK_OK) {
      mouseEvent &= ~(EVENT_BUTTON | EVENT_POSITION | EVENT_SCROLLWHEEL);
    }
  }
	
  if ((mouseEvent & EVENT_NTF) && (isEpBusy(2) == 0) ) {
//    mouseEvent &= ~(EVENT_NTF);
//    Hid_Ep2_Report_In[0] = REPORT_ID_NTF;
//    Hid_Ep2_Report_In[1] = ntfType;
//    if (ntfType == NTF_DPI) {
//      Hid_Ep2_Report_In[2] = (U8)(dpiCurrent.x >> 8);
//      Hid_Ep2_Report_In[3] = (U8)(dpiCurrent.x);
//      Hid_Ep2_Report_In[4] = (U8)(dpiCurrent.y >> 8);
//      Hid_Ep2_Report_In[5] = (U8)(dpiCurrent.y);
//    } else if ((ntfType == NTF_CAL_ID) || (ntfType == NTF_PROFILE)) {
//      Hid_Ep2_Report_In[2] = ntfValue;
//      Hid_Ep2_Report_In[3] = 0;
//      Hid_Ep2_Report_In[4] = 0;
//      Hid_Ep2_Report_In[5] = 0;
//    }
////    USB_WriteEP(USB_ENDPOINT_IN(2), &Hid_Ep2_Report_In[0], REPORT_SIZE_RAZER);
////    setEpBusy(2);
//    if (USB_EPnINFunction(USB_EP2,(U32*)&Hid_Ep2_Report_In[0], REPORT_SIZE_KB) == EPn_RETURN_ACK_OK) {  //REPORT_SIZE_RAZER
//      mouseEvent &= ~(EVENT_NTF);   
//      for (i = 0; i < MAX_EP_SIZE; i++) {
//          Hid_Ep2_Report_In[i] = 0;
//      }
//    }
  }
	
	 if ((mouseEvent & EVENT_RZ_KEY)&&(isEpBusy(2)==0)) {                         // Mark add it test 
      getBtnTransKey();
//      USB_WriteEP(USB_ENDPOINT_IN(2), &Hid_Ep2_Report_In[0], MAX_EP_SIZE);
//			setEpBusy(2);
     if (USB_EPnINFunction(USB_EP2,(U32*)&Hid_Ep2_Report_In[0], REPORT_SIZE_KB) == EPn_RETURN_ACK_OK) { //REPORT_SIZE_RAZER
        mouseEvent &= ~(EVENT_RZ_KEY);
        for (i = 0; i < MAX_EP_SIZE; i++) {
          Hid_Ep2_Report_In[i] = 0;
        }   
     }
  } 
	 
	if ((mouseEvent & EVENT_KEY)&&(isEpBusy(3)==0)) {
//    mouseEvent &= ~(EVENT_KEY);
		fillKeyHid(&Hid_Ep3_Report_In[1],KEY_IN|MACRO_IN|TURBO_IN);
//    USB_WriteEP(USB_ENDPOINT_IN(3), &Hid_Ep3_Report_In[0], REPORT_SIZE_KB);  
//		setEpBusy(3);
    Hid_Ep3_Report_In[0] = 3;
    if (USB_EPnINFunction(USB_EP2,(U32*)&Hid_Ep3_Report_In[0], REPORT_SIZE_KB) == EPn_RETURN_ACK_OK) {
      mouseEvent &= ~(EVENT_KEY);
    }  
  }
	if((macroEven&EVENT_MEDIA)&&(isEpBusy(2)==0)){            // Macro media even
//		macroEven&=~EVENT_MEDIA;
//		USB_WriteEP(USB_ENDPOINT_IN(2), &Hid_Ep2_Report_In[0], MAX_EP_SIZE);
//		setEpBusy(2);
    if (USB_EPnINFunction(USB_EP2,(U32*)&Hid_Ep2_Report_In[0], REPORT_SIZE_KB) == EPn_RETURN_ACK_OK) {  // MAX_EP_SIZE
      macroEven&=~EVENT_MEDIA;
    }     
	}
	if((macroEven&EVENT_POWER)&&(isEpBusy(2)==0)){          // Power even
//		macroEven&=~EVENT_POWER;
//		USB_WriteEP(USB_ENDPOINT_IN(2), &Hid_Ep2_Report_In[0], MAX_EP_SIZE);
//		setEpBusy(2);
    if (USB_EPnINFunction(USB_EP2,(U32*)&Hid_Ep2_Report_In[0], REPORT_SIZE_KB) == EPn_RETURN_ACK_OK) {  //MAX_EP_SIZE
      macroEven&=~EVENT_POWER;
    }    
	}
}
void processScrollTask(void)
{
	if(scrollTimer.upFlag&&(scrollTimer.upTimer==0)){       // Scroll up has trigered  and time to release
		processButtonEvent(BTN_BREAK, SCROLL_UP_IDX);         // Release 
		scrollTimer.upFlag=false;                             // Flag
	}
	if(scrollTimer.dnFlag&&(scrollTimer.dnTimer==0)){      // Scroll up has trigered  and time to release
		processButtonEvent(BTN_BREAK, SCROLL_DN_IDX);        // Release 
		scrollTimer.dnFlag=false;                            // Flag
	}
}
void processPendingTasks(void)
/*++

Function Description:
  This function checks that if there is a new data to report, build up the 
  Mouse Report packet and set to transmit to the system the next time the 
  endpoint in polled for activity.
 
Arguments:
  type   - 0: new keyboard data to report; 1: new mouse data to report
  *data - pointer to the data to be report.

Returns: 
  NONE

--*/
{
//  scrollWheelHandling();
  processMouseTasks();          // Normal mouse task
	processMacroTasks();          // Macro mouse task
  processDoubleClickTasks();    // Double click task  
//	processTurboTasks();          // Turbo tasts 
	processScrollTask();          // Scroll assgin as button even 
}


