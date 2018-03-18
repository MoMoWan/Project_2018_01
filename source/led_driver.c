/*++

Copyright (c) 2009-2010 Razer Ltd.
All rights reserved. This program contains proprietary and confidential information.

Module Name:
  led_driver.c

Compiler:
  Keil Software ?ision v4.72.10, with NXP Semiconductor LPC11U23 proccessor support.

Abstract:
  This file provides the lighting effects and LED drive support.

Color:
   Tri-color LEDs are not balanced in the 3 color legs (R, G, B). In fact Blue is
   much dimmer than the other guns. Green has the highest intensity. Thereofore the
   'spectrum' tries to dim (lower brightness) some of the color endpoints 
   and 'push' (fill in - example Blue is lighten by adding both Green and Red) others. 

--*/

#include "main.h" 
#include "type.h"
#include "led_driver.h"
#include "timer.h" 
#include "protocol.h"
#include "variables.h"
#include "Gpio.h"
#include "mouse.h"
#include "variables.h"
#include "i2c_driver.h"
#include "button.h"
#include "profile.h"

#define IDX_U1                  1
#define L_button_debounce_time  10  

U32 testflag = 0;



COLOR CHROMAFRAME[16];

U8 IICStates;
U16 basicFWTimer =0;

U8  breathType[NUMBER_OF_LEDS];

U8 Change_Scroll_LED_Dim =0;
U8 wave_color_buffer_R[24]; 
U8 wave_color_buffer_G[24]; 
U8 wave_color_buffer_B[24];

//U8 Logo_R_led_value;                                          //led r g b value
//U8 Logo_G_led_value;
//U8 Logo_B_led_value;

//U8 Scroll_R_led_value;                                          //led r g b value
//U8 Scroll_G_led_value;
//U8 Scroll_B_led_value;

U8  after_glow_ready[NUMBER_OF_LEDS];
U8  TrailTimerCnt[NUMBER_OF_LEDS];
U8  TrailFadeoutTime[NUMBER_OF_LEDS];

U8  LED_REFRESH_TIME;																			// Time to refersh the LED matrix    at leaset need 25-30ms to finish I2C writing
U8 ReactivePeriod[2];

CHROMA_PARM lighting[NUMBER_OF_LEDS] = {0};              // Active Lighting state parameters
LED_MAXM calibrating[NUMBER_OF_LEDS] = {0};           // Active Lighting calibrating values
LED_FADE matrixStaging[NUMBER_OF_LEDS][MAXNUMBER_LEDS];   // Stage of matrix value

U8 flag_Scroll_Wheel_Led_Blink =0;
U8 ProfilePress = 0;
U16 Profiletime = 0;
U8 Profiledebounce = 0;

U8 SUSPEND_LED_fadoutflag = 0;
TRAIL_EFFECT_RAM Trail_Parameter[NUMBER_OF_LEDS] = {0};            // ivan add for trail effect use

U8 randomcolorIdex = 0;
U8 randomFlag = 0;
U8 randomCompFlag = 0;
U8 ProfileNo = 0;
U8 previousEffect[NUMBER_OF_LEDS]={0,0,0};                                      // Prevous effect 
const SCRIPT spectrum[] = {{34,  1, 0xC0, 0x38, 0xFF, 0xFF},  // Blue
                           {10,  1, 0xFF, 0x18, 0xFF, 0xFF},  // Purple
                           {17,  1, 0xFF, 0x10, 0x10, 0xEF},  // Red
                           {15,  1, 0xFF, 0x80, 0x00, 0xDF},  // Orange
                           {10,  1, 0xEF, 0xFF, 0x00, 0xD7},  // Yellow
                           {10,  1, 0xDF, 0xFF, 0x00, 0xD7},  // Yellow
                           {10,  1, 0x00, 0xFF, 0x00, 0xCF},  // Green
                           { 5,  1, 0x00, 0xFF, 0x00, 0xCF},  // Green
                           {17,  1, 0x00, 0xFF, 0xFF, 0xDF},  // Cyan
                           {20,  1, 0x18, 0x58, 0xFF, 0xFF}}; // Blue
//this table is be used brightness map																			 
const U8 brightnesstable[64]={
	0,1,2,3,4,5,6,7,
	8,10,12,14,16,18,20,22,
	24,26,29,32,35,38,41,44,
	47,50,53,57,61,65,69,73,
	77,81,85,89,94,99,104,109,
	114,119,124,129,134,140,146,152,
	158,164,170,176,182,188,195,202,
	209,216,223,230,237,244,251,255
	
};

const RANDOMCOLOR randomList[16] = {                     // Color table for random lighting effect
  { 0xFF, 0xFF, 0xFF   },
  { 0xFF, 0x00, 0x00   },
  { 0xFF, 0x7F, 0x7F   },
  { 0x00, 0xFF, 0x00   },
  { 0x7F, 0x7F, 0xFF   },
  { 0x00, 0x00, 0xFF   },
  { 0x7F, 0xFF, 0x7F   },
  { 0xFF, 0xFF, 0x00   },
  { 0x7F, 0x00, 0xFF   },           
  { 0x00, 0xFF, 0xFF   },
  { 0xFF, 0x00, 0xFF   },
  { 0xFF, 0x7F, 0x00   },
  { 0xFF, 0x00, 0x7F   },
  { 0x00, 0xFF, 0x7F   },
  { 0x7F, 0xFF, 0x00   },
  { 0x00, 0x7F, 0xFF   },
};
const LED_REG ledInfor[LED_NUMBERS]={
														{LOGO_LED    ,0x84,0xA4,0x94},//Logo        // Const every led pwm register address
														{SCROLL_LED1 ,0x85,0X95,0xA5},//Scroll 1
														{SCROLL_LED2 ,0X86,0X96,0XA6},//SCROLL 2
														{LED_L_NUM1  ,0x24,0x34,0x44},// Left Bar 1
														{LED_L_NUM2  ,0x25,0x35,0x45},// Left Bar 2
														{LED_L_NUM3  ,0x26,0x36,0x46},// Left Bar 3
														{LED_L_NUM4  ,0x27,0x37,0x47},// Left Bar 4
														{LED_L_NUM5  ,0x28,0x38,0x48},// Left Bar 5
														{LED_L_NUM6  ,0x29,0x39,0x49},// Left Bar 6
														{LED_L_NUM7  ,0x2A,0x3A,0x4A},// Left Bar 7																																																								
														{LED_R_NUM1  ,0x5A,0x6A,0x7A},// Right Bar 7
														{LED_R_NUM2  ,0x59,0x69,0x79},// Right Bar 6
														{LED_R_NUM3  ,0x58,0x68,0x78},// Right Bar 5
														{LED_R_NUM4  ,0x56,0x66,0x76},// Right Bar 4
														{LED_R_NUM5  ,0x57,0x67,0x77},// Right Bar 3
														{LED_R_NUM6  ,0x55,0x65,0x75},// Right Bar 2
														{LED_R_NUM7  ,0x54,0x64,0x74},// Right Bar 1
														{LED_PROFILE ,0x87,0xA7,0x97},// Profile indicate led
													};
COLOR ledPwmData[LED_NUMBERS];                                       // Pwm data to write to led driver
													
													
//[ Wave buffer variables													
const COLOR waveBuffer[24]={{255,0,0,255},{255,64,0,255},{255,127,0,255},{255,191,0,255},{255,255,0,255},{191,255,0,255},{127,255,0,255},{64,255,0,255},
                            {0,255,0,255},{0,255,64,255},{0,255,127,255},{0,255,191,255},{0,255,255,255},{0,191,255,255},{0,127,255,255},{0,64,255,255},														
														{0,0,255,255},{64,0,255,255},{127,0,255,255},{191,0,255,255},{255,0,255,255},{255,0,191,255},{255,0,127,255},{255,0,64,255},
                            };
COLOR waveCycleBuffer[NUMBER_OF_LEDS][24]=
                            { 
                            {{255,0,0,255},{255,64,0,255},{255,127,0,255},{255,191,0,255},{255,255,0,255},{191,255,0,255},{127,255,0,255},{64,255,0,255},
                            {0,255,0,255},{0,255,64,255},{0,255,127,255},{0,255,191,255},{0,255,255,255},{0,191,255,255},{0,127,255,255},{0,64,255,255},														
														{0,0,255,255},{64,0,255,255},{127,0,255,255},{191,0,255,255},{255,0,255,255},{255,0,191,255},{255,0,127,255},{255,0,64,255},
                            },
															 
													  {{255,0,0,255},{255,64,0,255},{255,127,0,255},{255,191,0,255},{255,255,0,255},{191,255,0,255},{127,255,0,255},{64,255,0,255},
                            {0,255,0,255},{0,255,64,255},{0,255,127,255},{0,255,191,255},{0,255,255,255},{0,191,255,255},{0,127,255,255},{0,64,255,255},														
														{0,0,255,255},{64,0,255,255},{127,0,255,255},{191,0,255,255},{255,0,255,255},{255,0,191,255},{255,0,127,255},{255,0,64,255},
                            },
															 
														{{255,0,0,255},{255,64,0,255},{255,127,0,255},{255,191,0,255},{255,255,0,255},{191,255,0,255},{127,255,0,255},{64,255,0,255},
                            {0,255,0,255},{0,255,64,255},{0,255,127,255},{0,255,191,255},{0,255,255,255},{0,191,255,255},{0,127,255,255},{0,64,255,255},														
														{0,0,255,255},{64,0,255,255},{127,0,255,255},{191,0,255,255},{255,0,255,255},{255,0,191,255},{255,0,127,255},{255,0,64,255},
                            },
															 
//														{{255,0,0,255},{255,64,0,255},{255,127,0,255},{255,191,0,255},{255,255,0,255},{191,255,0,255},{127,255,0,255},{64,255,0,255},
//                            {0,255,0,255},{0,255,64,255},{0,255,127,255},{0,255,191,255},{0,255,255,255},{0,191,255,255},{0,127,255,255},{0,64,255,255},														
//														{0,0,255,255},{64,0,255,255},{127,0,255,255},{191,0,255,255},{255,0,255,255},{255,0,191,255},{255,0,127,255},{255,0,64,255},
//                            },
                            };

#define waveColorSize    (24)
#define waveStartIndex   (0)
#define waveEndIndex     (23)
														
void led_initialization(void)
/*++

Function Description:
  This function initializes all LED variables and hardware. This function mainly
  concerns itself with configuring mcu pin to pwm module of led.

  PIN     IO         led       moduel
  -------------------------------
  35   PIO0_14  Scroll    CT32B1_MAT1
  11   PIO1_26  Keypad CT32B0_MAT2
  32   PIO0_11  Logo     CT32B0_MAT3
 
Arguments:
  NONE

Returns: 
  NONE

--*/
{
   U8 state[MAX_LED_NUM >> 3];
	 U8 iniZero[MAX_LED_NUM];
	 U8 i;
   gpio_SetValue(1, 26, 0);                                        // Low to shut down led controller
   delayMilliseconds(10);
	 gpio_SetValue(1, 26, 1);                                        // Hight to power up led controller
   delayMilliseconds(10);
	
	 memset((void*)iniZero,(int)0,(int)sizeof(iniZero));             // clear lighting
   memset((void*)state,(int)0,(int)sizeof(state));                 // clear li
	 for(i=0;i<LED_NUMBERS;i++){
		 state[(ledInfor[i].raddr-0x24)/8]|=1<<((ledInfor[i].raddr-0x24)%8);		 
		 state[(ledInfor[i].gaddr-0x24)/8]|=1<<((ledInfor[i].gaddr-0x24)%8);	
		 state[(ledInfor[i].baddr-0x24)/8]|=1<<((ledInfor[i].baddr-0x24)%8);	
	 }
	 	  
	 //Clear all the buffer
//	   for (i=1; i <= 8; i++) {                              // 1-8 frame
//      ledDriverWrite(LED_CTLR2_ADDR, REG_FRAME_SEL, FRAME_NUM(i));      // set frame j to OFF
//      i2cWrite(LED_CTLR2_ADDR, REG_CTRL_S, (U8 *)&iniZero, (MAX_LED_NUM >> 3));  // write 0 to LED CTRL register
//      i2cWrite(LED_CTLR2_ADDR, REG_PWM_S, (U8 *)&iniZero, MAX_LED_NUM);                  // write 0 to PWM register  
//    }  
//			 
//	  ledDriverWrite(LED_CTLR2_ADDR, REG_FRAME_SEL, FRAME_NUM(1));      // set frame j to OFF
//	  i2cWrite(LED_CTLR2_ADDR, REG_CTRL_S, (U8 *)&state, (MAX_LED_NUM >> 3)); 	 	 
//	  ledDriverWrite(LED_CTLR2_ADDR, REG_FRAME_SEL, FRAME_NUM(1));      // set frame 1 to OFF
//    ledDriverWrite(LED_CTLR2_ADDR, REG_FRAME_SEL, FRAME_NUM_9);       // Select frame 9
//    ledDriverWrite(LED_CTLR2_ADDR, REG_CFG,       0);                 // Set to Picture mode   
//		ledDriverWrite(LED_CTLR2_ADDR, REG_HALF_VCC,  0x10);              // Set output level to 1/2 vcc
//    ledDriverWrite(LED_CTLR2_ADDR, REG_DISPLAY,   FRAME_NUM(1));      // Picture frame select to frame 1
//    ledDriverWrite(LED_CTLR2_ADDR, REG_AUTOPLAY1, 0);                 //   
//    ledDriverWrite(LED_CTLR2_ADDR, REG_AUTOPLAY2, 0);                 //
//    ledDriverWrite(LED_CTLR2_ADDR, REG_OPT,       0);                 // Independent intensity of each frame, blk disable
//    ledDriverWrite(LED_CTLR2_ADDR, REG_AUDIO,     0);                 //   
//    ledDriverWrite(LED_CTLR2_ADDR, REG_BREATH1,   0);                 //
//    ledDriverWrite(LED_CTLR2_ADDR, REG_BREATH2,   0);                 //
//    ledDriverWrite(LED_CTLR2_ADDR, REG_SHUT,      1);                 // Default is shutdown mode, set to normal operation mode
//    ledDriverWrite(LED_CTLR2_ADDR, REG_AGC,       0);                 //   
//    ledDriverWrite(LED_CTLR2_ADDR, REG_ADCRATE,   0);                 //
//    ledDriverWrite(LED_CTLR2_ADDR, REG_FRAME_SEL, FRAME_NUM(1));      // Select frame 1                                     // PWM value clear
}


//void initialfx(U8 led_id) {
void initialfx(CHROMA_PARM *led) {  
	memset((U8 *)&matrixStaging[led[0].index][0],0,sizeof(matrixStaging[led[0].index]));  // Set the matrix case 
	updateMatrixHardware(led[0].id, ON);                    // Update corresponding Pulse Width Modulator
}


void Load_LED_Effect(CHROMA_PARM *led)
/*++

Function Description:
  load the first LED effect
 
Arguments:
  NONE

Returns: 
  NONE

--*/
{
//  CHROMA_PARM *comp;
//  U8 i;
//	switch (led[0].pendingeffect) {
//		case CHROMA_EFX_OFF:                                  // OFF
//			decodeExternEffect(led, &matrixStaging[led[0].index][0]);              // extendeffect = NO_EFFECT, effect = NO_EFFECT
//		break;
//		case CHROMA_EFX_STATIC:                               // Static
//		  led[0].effect = NO_EFFECT;
//      setTransitionTarget(led,(COLOR *)&led[0].Chroma_effect.r);    // set extendtime / pwm / fx 
//		  decodeExternEffect(led, &matrixStaging[led[0].index][0]);              // extendeffect = dissolve, reset extendtime pwm and extendlevel
//		break;
//		
//		case CHROMA_EFX_BREATHE:                              // breathe
//			led[0].effect = NO_EFFECT;
//      if (led[0].Chroma_effect.colorNO == 0) {                      // if it a ramdom color selected
//        randomcolorIdex = randomGenerate(0,15);
//        comp = ledParameters(1, user.profile[user.ProNO].savedLight[0].id);// get the address of the first lighting index
//        for (i=0; i< NUMBER_OF_LEDS; i++) {                         // check for sync LED
//         if (comp[i].pendingeffect ==  led[0].pendingeffect) {      // if effect synced
//            led[0].Chroma_effect.r = randomList[randomcolorIdex].r; // byte6: r
//            led[0].Chroma_effect.g = randomList[randomcolorIdex].g; // byte7: g
//            led[0].Chroma_effect.b = randomList[randomcolorIdex].b; // byte8: b
//           randomFlag |= 1<<i;                                      // update corresponding sync flag
//         } else {                                                   // effect not in sync 
//           randomFlag &= ~(1<<i);                                   // update correcsponding unsync flag    
//         }
//       } 
//      } else {
//        randomFlag &= ~(1<<led[0].index);                           // update correspoding flag if effect change from breathing to others
//      }
//      setTransitionTarget(led,(COLOR *)&led[0].Chroma_effect.r);    // set extendtime / pwm / fx 
//		  decodeExternEffect(led, &matrixStaging[led[0].index][0]);              // extendeffect = dissolve
//		  breathType[led[0].index] = 0;                                 // reset breathe flag
//      randomCompFlag = 0;
//		break;
//		
//    case CHROMA_EFX_SPECTRUM:                             // Spectrum
//			led[0].effect = NO_EFFECT;
//			led[0].script = 0; 
//			setSpectumTransitionTarget(led);                              // initial faderTime/scratch/faderStep/fx.new/pwm/tmr
//      decodeExternEffect(led, &matrixStaging[led[0].index][0]);              // extendeffect = dissolve, reset extendtime pwm and extendlevel
//		break;
//	 case CHROMA_EFX_WAVE:
//		  led[0].effect = NO_EFFECT;
//			setTransitionTarget(led,(COLOR *)&led[0].Chroma_effect.r);    // set extendtime / pwm / fx 
//		  decodeExternEffect(led, &matrixStaging[led[0].index][0]);              // extendeffect = dissolve, reset extendtime pwm and extendlevel
//			break;
//		case CHROMA_EFX_REACTIVE:                             // reactive
//      if (led[0].Chroma_effect.colorNO == 0) {                      // if it a ramdom color selected
//        randomcolorIdex = randomGenerate(0,15);
//        comp = ledParameters(1, user.profile[user.ProNO].savedLight[0].id);// get the address of the first lighting index
//        for (i=0; i< NUMBER_OF_LEDS; i++) {                         // check for sync LED
//         if (comp[i].pendingeffect ==  led[0].pendingeffect) {      // if effect synced
//            led[0].Chroma_effect.r = randomList[randomcolorIdex].r; // byte6: r
//            led[0].Chroma_effect.g = randomList[randomcolorIdex].g; // byte7: g
//            led[0].Chroma_effect.b = randomList[randomcolorIdex].b; // byte8: b
//           randomFlag |= 1<<i;                                      // update corresponding sync flag
//         } else {                                                   // effect not in sync 
//           randomFlag &= ~(1<<i);                                   // update correcsponding unsync flag    
//         }
//       } 
//      } else {
//        randomFlag &= ~(1<<led[0].index);                           // update correspoding flag if effect change from breathing to others
//      }
//			led[0].effect = led[0].pendingeffect;
//      led[0].extendeffect = NO_EFFECT;
//      setTransitionTarget(led,(COLOR *)&led[0].Chroma_effect.r);    // initial faderTime/scratch/faderStep/fx.new/pwm/tmr

//      Trail_Parameter[led[0].index].Trailflag = 0;                  // reactive flag
//      Trail_Parameter[led[0].index].TrailPWM =  0;                  // reactive PWM (0~255)
//	    after_glow_ready[led[0].index] = 0;                           // reactive timing flag
//	    TrailTimerCnt[led[0].index] = 0;                              // reactive timer flag
//      if (led[0].Chroma_effect.rate == 1)     {TrailFadeoutTime[led[0].index]= 2;}// Trail time 2*255 = 0.5s   
//      else if(led[0].Chroma_effect.rate == 2) {TrailFadeoutTime[led[0].index]= 4;}// Trail time 4*255 = 1s
//      else if(led[0].Chroma_effect.rate == 3) {TrailFadeoutTime[led[0].index]= 6;}// Trail time 6*255 = 1.5s
//      else if(led[0].Chroma_effect.rate == 4) {TrailFadeoutTime[led[0].index]= 8;}// Trail time 8*255 = 2s  
//		break;
//			
//		case CHROMA_EFX_CUSTOM_FRAME:                         // Custom frame
//			led[0].effect = CHROMA_EFX_CUSTOM_FRAME;
//		  led[0].extendeffect = NO_EFFECT;
//		break;
//  }
}


void pulsatingLedEffect(CHROMA_PARM *led)
/*++

Function Description:
  This function creates the pulsating or breathing lighting effect. This functionality
  is connected to any LED that has a separate Pulse Width Modulator which will control
  it. 
 
Arguments:
  led   = Pointer to the specified LED parameters.

Returns: 
  NONE

--*/
{
//  U16 pulseStep;
//  U8 randomIndex = 0,i;
//	LED_FADE *fx = &matrixStaging[led[0].index][0];                  // Reference LED transitioning storage
//  COLOR c={0,0,0,0};
//  if (led[0].tmr >= led[0].scratch)  {                    // If time to update Pulse or Stage, then...
//    led[0].tmr = 0;                                       // Restar time counter.
//		if (led[0].extendeffect != FADEOUT) {                 // if fade out , only unpdate color
//      if ((led[0].state & RISING) != 0) {                 // If the pulse is rising, then...
//        if (led[0].pwm < user.breathParam.pulseNeckLine) {
//          pulseStep = user.breathParam.pulseRise1Step;
//        } else {
//          pulseStep = user.breathParam.pulseRise2Step;
//        }
//        if ((0xFFFF - led[0].pwm) > pulseStep) {            // If we haven't reached the top, then...
//          led[0].pwm += pulseStep;                          // Add in a full step value
//          led[0].scratch = user.breathParam.pulseRiseTime;            // Next processing after 'Step Period'
//        } else {                                            // Less than a full step from top
//          led[0].pwm = 0xFFFF;                              // Set top value
//          led[0].state &= ~RISING;                          // Prepare for falling pulse after 'On Period'
//          led[0].scratch = user.breathParam.pulseOn;                  // Next processing after 'On Period'
//        }
//      } else {                                              // If the pulse is falling, then...
//        if (led[0].pwm < user.breathParam.pulseNeckLine) {
//          pulseStep = user.breathParam.pulseFall1Step;
//        } else {
//          pulseStep = user.breathParam.pulseFall2Step;
//        }
//        if (led[0].pwm > pulseStep) {                       // If we haven't reached the bottom, then...
//          led[0].pwm -= pulseStep;                          // Substract off a full step value
//          led[0].scratch = user.breathParam.pulseFallTime;            // Next processing after 'Step Period'
//        } else {                                            // Less than a full step from bottom
//          led[0].pwm = 0x0000;                              // Set bottom value
//          led[0].state |= RISING;                           // Prepare for falling pulse after 'Off Period'
//          led[0].scratch = user.breathParam.pulseOff;                 // Next processing after 'Off Period'	
//		      if (led[0].Chroma_effect.colorNO == 2) {          // Two color
//		  	    if (breathType[led[0].index] == 0 ) {
//							c.r=led[0].Chroma_effect.rr;                  // Get r 
//							c.g=led[0].Chroma_effect.gg;                  // Get g
//							c.b=led[0].Chroma_effect.bb;                  // Get b
//							setStageOneColor(fx,MIX|RGB,c);               // Set RGB in mix
//		  			  breathType[led[0].index] = 1;                 // flage
//		  		  } else {
//							c.r=led[0].Chroma_effect.r;                   // The first r/g/b
//							c.g=led[0].Chroma_effect.g;                   // The first r/g/b
//							c.b=led[0].Chroma_effect.b;                   // The first r/g/b
//							setStageOneColor(fx,MIX|RGB,c);               // Set RGB in mix
//		  		  	breathType[led[0].index] = 0;
//	     	    }
//          } else if (led[0].Chroma_effect.colorNO == 0) {
//            if (randomFlag == (randomCompFlag |= (1<<led[0].index))) {  // if acheive the last sync led 
//              do {                            
//               randomIndex= randomGenerate(0,15);                       // get new random color index
//              } while (randomcolorIdex == randomIndex);                 // avoid 
//              randomcolorIdex = randomIndex;
//              randomCompFlag = 0;
//              for (i = 0; i<NUMBER_OF_LEDS; i++ ) {                     // to solve ramdom color will keep in first time
//                if (randomFlag & (1<<i)) {
//									c.r=randomList[randomcolorIdex].r;     // update color 
//									c.g=randomList[randomcolorIdex].g;     // update color 
//									c.b=randomList[randomcolorIdex].b;     // update color 
//									setStageOneColor((LED_FADE*)&matrixStaging[i][0],MIX|RGB,c);               // Set RGB in mix
//                }                
//              }              
//            }       
//          }
//        }
//      }
//	  }
//    updateMatrixHardware(led[0].id, PWM);                    // Update corresponding Pulse Width Modulator
//  }
}
void waveEffect(CHROMA_PARM *led)
{
//	 LED_FADE *fx = &matrixStaging[led[0].index][0];
//	 U8 i,indexS,indexE;  //indexS: start cycle index,indexE:the last one cycle data
//	 COLOR temp={0,0,0,0};
//	 U8 waveRefreshTimer;
//	 waveRefreshTimer=led[0].Chroma_effect.rate;   // 1=80,2=60,3=40 
//	 if(led[0].tmr>waveRefreshTimer){
//		 led[0].tmr=0;
//	   temp=(led[0].Chroma_effect.flags==T2B?waveCycleBuffer[led[0].index][waveStartIndex]:waveCycleBuffer[led[0].index][waveEndIndex]);// Get the first or last colour
//	   indexS=led[0].Chroma_effect.flags==T2B?waveStartIndex:waveEndIndex;                                   
//		 indexE=led[0].Chroma_effect.flags==T2B?waveEndIndex:waveStartIndex;
//	   for(i=0;i<waveColorSize-1;i++){
//		   if(led[0].Chroma_effect.flags==T2B){
//		     waveCycleBuffer[led[0].index][indexS+i]=waveCycleBuffer[led[0].index][indexS+i+1];
//		   }else{
//			   waveCycleBuffer[led[0].index][indexS-i]=waveCycleBuffer[led[0].index][indexS-i-1];
//		   }
//	   }
//	   waveCycleBuffer[led[0].index][indexE]=temp;
//	   for(i=0;i<7;i++){
//		   fx[i].mix.r=waveCycleBuffer[led[0].index][i].r;
//		   fx[i].mix.g=waveCycleBuffer[led[0].index][i].g;
//		   fx[i].mix.b=waveCycleBuffer[led[0].index][i].b;
//		   fx[i].mix.v=waveCycleBuffer[led[0].index][i].v;		
//	   }
//     updateMatrixHardware(led[0].id,ON);	
//   }
}
void setSpectumTransitionTarget(CHROMA_PARM *led)
/*++

Function Description:
  This function indexes the next target color value and initailizes the color values
  as well as saving the current color values.
 
Arguments:
  led = Pointer to the specified LED parameters.
  fx  = Pointer to the corresponding Disolve parameters.

Returns: 
  NONE

--*/
{
//  LED_FADE *fx = &matrixStaging[led[0].index][0];
//  COLOR c={0,0,0,0};
//  if (++led[0].script >= (sizeof(spectrum)) / (sizeof (SCRIPT))) {
//    led[0].script = 0;                                    // Initialize Script index value
//  }
//  led[0].pwm = 0;                                         // Initialize accumulated step value
//  led[0].tmr = 0;                                         // Initialize accumuulated time value
//  led[0].faderTime = spectrum[led[0].script].faderTime;   // fader time
//  led[0].scratch =  led[0].faderTime;                     // Retrigger when to transition percentage
//  led[0].faderStep = spectrum[led[0].script].faderStep;   // fader step
//	c.r=spectrum[led[0].script].r;
//	c.g=spectrum[led[0].script].g;
//	c.b=spectrum[led[0].script].b;
//	c.v=spectrum[led[0].script].v;
//  setStageOneColor(fx,NEW|ACOLOR,c);               // Set RGB in mix
}


void after_glow_effect(CHROMA_PARM  *led)
/*++.

Function Description:
  reactive effect
 
Arguments:
   tmr
   
   TrailPWM/ brightness/led[0].level/led calibration r g g

Returns: 
  NONE

--*/
{
//	U8 i, randomIndex;
//  LED_FADE *fx = &matrixStaging[led[0].index][0];
//  COLOR c={0,0,0,0,};
//	if (led[0].tmr > LED_REFRESH_TIME) {
//    led[0].tmr = 0;
//		if (led[0].extendeffect != FADEOUT) {                          // if fade out ,only update LED
//		  if ((led[0].Chroma_effect.colorNO == 0) && (ReactivePeriod[led[0].index] ==1)) {  // for random    
//        if (randomFlag == (randomCompFlag |= (1<<led[0].index))) {  // if acheive the last sync led 
//          do {                            
//           randomIndex= randomGenerate(0,15);                       // get new random color index
//          } while (randomcolorIdex == randomIndex);                 // avoid 
//          randomcolorIdex = randomIndex;
//          randomCompFlag = 0;
//          for (i = 0; i<NUMBER_OF_LEDS; i++ ) {                     // to solve ramdom color will keep in first time
//            if (randomFlag & (1<<i)) {
//							c.r=randomList[randomcolorIdex].r;     // update color
//							c.g=randomList[randomcolorIdex].g;     // update color
//							c.b=randomList[randomcolorIdex].b;     // update color
//							setStageOneColor(&matrixStaging[i][0],MIX|RGB,c);               // Set RGB in mix
//            }                
//          }              
//        }         
//        ReactivePeriod[led[0].index] =0 ;
//      }
//      //Because the after glow effect all matrix stage is the same, so it can use fx[0].new for the pararmer			
//      c.r = fx[0].new.r* Trail_Parameter[led[0].index].TrailPWM / 255;
//      c.g = fx[0].new.g* Trail_Parameter[led[0].index].TrailPWM / 255;
//      c.b = fx[0].new.b* Trail_Parameter[led[0].index].TrailPWM / 255;
//      c.v = 0xFF;
//			setStageOneColor(fx,MIX|ACOLOR,c);               // Set RGB in mix
//    }
//    updateMatrixHardware(led[0].id, ON);
//  }
}

 
void custom_frame_effect(CHROMA_PARM  *led) 
/*++

Function Description:
  custom frame effect
 
Arguments:
  NONE

Returns: 
  NONE

--*/
{
//	U8 i;
//	U32 target;
//	LED_FADE *fx =  &matrixStaging[led[0].index][0];        // Reference LED transitioning storage
//	LED_MAXM *cal = &calibrating[led[0].index];             // Reference 'active' LED calibration
//	target=brightnesstable[led[0].level/4];
//  //target = (((((U32)(fx[0].mix.v) << 8) + 0xFF) * (((U32)(brightnesstable[led[0].level/4]) << 8) + 0xFF)) >> 16); 
//  //target = (target * led[0].extendlevel) >> 16;
//  switch(led[0].id){
//    case CHROMA_LOGO_ID:
//			ledPwmData[LOGO_LED].r=(CHROMAFRAME[1].r*target*cal[0].max.r)>>16;       // Logo custom led
//		  ledPwmData[LOGO_LED].g=(CHROMAFRAME[1].g*target*cal[0].max.g)>>16;
//		  ledPwmData[LOGO_LED].b=(CHROMAFRAME[1].b*target*cal[0].max.b)>>16;
//      for(i=0;i<7;i++){
//        fx[i].mix.r=ledPwmData[LOGO_LED].r;
//        fx[i].mix.g=ledPwmData[LOGO_LED].g;
//        fx[i].mix.b=ledPwmData[LOGO_LED].b;
//        fx[i].mix.v=0xff;
//      } 	
//      
//   break;
//   case CHROMA_WHEEL_ID:
//			ledPwmData[SCROLL_LED1].r=ledPwmData[SCROLL_LED2].r=(CHROMAFRAME[0].r*target*cal[0].max.r)>>16;       // Logo custom led
//		  ledPwmData[SCROLL_LED1].g=ledPwmData[SCROLL_LED2].g=(CHROMAFRAME[0].g*target*cal[0].max.g)>>16;
//		  ledPwmData[SCROLL_LED1].b=ledPwmData[SCROLL_LED2].b=(CHROMAFRAME[0].b*target*cal[0].max.b)>>16;
//	    for(i=0;i<7;i++){
//        fx[i].mix.r=ledPwmData[SCROLL_LED1].r;
//        fx[i].mix.g=ledPwmData[SCROLL_LED1].g;
//        fx[i].mix.b=ledPwmData[SCROLL_LED1].b;
//        fx[i].mix.v=0xff;
//      } 	
//   break;
//	 case CHROMAR_LS_ID:
//		 for(i=0;i<7;i++){
//			 ledPwmData[LED_L_NUM1+i].r=fx[i].mix.r=(CHROMAFRAME[2+i].r*target*cal[0].max.r)>>16;
//			 ledPwmData[LED_L_NUM1+i].g=fx[i].mix.g=(CHROMAFRAME[2+i].g*target*cal[0].max.g)>>16;
//			 ledPwmData[LED_L_NUM1+i].b=fx[i].mix.b=(CHROMAFRAME[2+i].b*target*cal[0].max.b)>>16;
//			 fx[i].mix.v=0xff;
//		 }
//	  break;
//	case CHROMAR_RS_ID:
//		 for(i=0;i<7;i++){
//			 ledPwmData[LED_R_NUM1+i].r=fx[i].mix.r=(CHROMAFRAME[9+i].r*target*cal[0].max.r)>>16;
//			 ledPwmData[LED_R_NUM1+i].g=fx[i].mix.g=(CHROMAFRAME[9+i].g*target*cal[0].max.g)>>16;
//			 ledPwmData[LED_R_NUM1+i].b=fx[i].mix.b=(CHROMAFRAME[9+i].b*target*cal[0].max.b)>>16;
//			 fx[i].mix.v=0xff;
//		 }
//	 break;
//  }
}

void updateMatrixHardware(U8 led_id,U8 state)
{
//	 U8 i;
//	 U32 target;
//	 CHROMA_PARM *led = ledParameters(0, led_id);            // Reference 'active' LED parameters
//	 LED_MAXM *cal = &calibrating[led[0].index];             // Reference 'active' LED calibration
//	 LED_FADE *fx =  &matrixStaging[led[0].index][0];        // Reference LED transitioning storage
//	 COLOR c={0,0,0,0};
//   if (state == PWM) {
//		c.v= (U8)(led[0].pwm >> 8);
//		setStageOneColor(fx,MIX|V,c);                         //             
//	 } 
//	 target = (((((U32)(fx[0].mix.v) << 8) + 0xFF) * (((U32)(brightnesstable[led[0].level/4]) << 8) + 0xFF)) >> 16); 
//   target = (target * led[0].extendlevel) >> 16;
//	  switch (led[0].id) {                                       // Dispatch to corresponding LED ID
//			case CHROMA_LOGO_ID:
//				ledPwmData[LOGO_LED].r = ((state == OFF) ? 0x00 : (U8)(((target * fx[6].mix.r) * cal[0].max.r) >> 16));   // set scroll wheel LED R PWM register to 0xFF,Use the 6th led for wave the same as manba
//        ledPwmData[LOGO_LED].g = ((state == OFF) ? 0x00 : (U8)(((target * fx[6].mix.g) * cal[0].max.g) >> 16));   // set scroll wheel LED G PWM register to 0xFF,Use the 6th led for wave the same as manba
//        ledPwmData[LOGO_LED].b = ((state == OFF) ? 0x00 : (U8)(((target * fx[6].mix.b) * cal[0].max.b) >> 16));   // set  scroll wheel LED B PWM register to 0xFF ,Use the 6th led for wave the same as manba
//				break;
//			case CHROMA_WHEEL_ID:
//			  ledPwmData[SCROLL_LED1].r =ledPwmData[SCROLL_LED2].r= ((state == OFF) ? 0x00 : (U8)(((target * fx[0].mix.r) * cal[0].max.r) >> 16)); // set LOGO LED R PWM register to 0xFF
//			  ledPwmData[SCROLL_LED1].g =ledPwmData[SCROLL_LED2].g =((state == OFF) ? 0x00 : (U8)(((target * fx[0].mix.g) * cal[0].max.g) >> 16)); // set LOGO LED G PWM register to 0xFF
//			  ledPwmData[SCROLL_LED1].b =ledPwmData[SCROLL_LED2].b =((state == OFF) ? 0x00 : (U8)(((target * fx[0].mix.b) * cal[0].max.b) >> 16)); // set LOGO LED B PWM register to 0xFF  
//				break;
//			case CHROMAR_LS_ID:
//				for(i=0;i<7;i++){
//					ledPwmData[LED_L_NUM1+i].r=((state == OFF) ? 0x00 : (U8)(((target * fx[i].mix.r) * cal[0].max.r) >> 16));
//					ledPwmData[LED_L_NUM1+i].g=((state == OFF) ? 0x00 : (U8)(((target * fx[i].mix.g) * cal[0].max.g) >> 16));
//					ledPwmData[LED_L_NUM1+i].b=((state == OFF) ? 0x00 : (U8)(((target * fx[i].mix.b) * cal[0].max.b) >> 16));
//				}
//				break;
//			case CHROMAR_RS_ID:
//				for(i=0;i<7;i++){
//					ledPwmData[LED_R_NUM1+i].r=((state == OFF) ? 0x00 : (U8)(((target * fx[i].mix.r) * cal[0].max.r) >> 16));
//					ledPwmData[LED_R_NUM1+i].g=((state == OFF) ? 0x00 : (U8)(((target * fx[i].mix.g) * cal[0].max.g) >> 16));
//					ledPwmData[LED_R_NUM1+i].b=((state == OFF) ? 0x00 : (U8)(((target * fx[i].mix.b) * cal[0].max.b) >> 16));
//				}
//				break;
//	  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void updateLightingEffects(void)
/*++

Function Description:
  This function is periodically called every 1ms to update all of the lighting
  effects. Even changes of static state are performed here, whereas the foreground
  code only modifies the LED parameter state along with setting the "changed" flag.

  Special treatment for protocol commnands the setting of LED_RGB_PARAM and
  LED_RGB_BRIGHTNESS features. These values are staged in LED_FADE 'new' values. 
  The interrupt handler checks pending transition and if the current effect = STATIC.
  If so, a dissolve effect is set up from the current (the 'old') values to the 'new'
  values and the pending transition flag is cleared. As soon as the dissolve has reach
  the target value, the effect is set to STATIC. If the Transition Step and Time equal
  '0', the dissolve effect is skipped.
 
Arguments:
  NONE

Returns: 
  NONE

--*/
{
#if 0  
	U8 i;
	static U8 iicIndex;
	for (i = 0; i < NUMBER_OF_LEDS; i++) {
		if (lighting[i].state == OFF) {
		  updateMatrixHardware(lighting[i].id, OFF);             // Turn corresponding LED "Off"
		} else {
		  switch (lighting[i].effect) {
			  case NO_EFFECT:
				  break;
			  case CHROMA_EFX_STATIC:                           // static
				  updateMatrixHardware(lighting[i].id, ON);          // Turn corresponding LED "On"
				  break;
				case CHROMA_EFX_BREATHE:                          // breathe
					pulsatingLedEffect(&lighting[i]);
				break;
			  case CHROMA_EFX_SPECTRUM:                         // spectrum
				  transitionLedEffect(&lighting[i]);              // Process Transitioning LEDs
				break;
				case CHROMA_EFX_WAVE:                             // wave effect
					waveEffect(&lighting[i]);                       // 
				break;
				case CHROMA_EFX_REACTIVE:                         // reactive
	        after_glow_effect(&lighting[i]);
				break;
        
				case CHROMA_EFX_CUSTOM_FRAME:                     // custom frame
					custom_frame_effect(&lighting[i]);
				break;
				
        default:
          break;
		 }
			
		 switch (lighting[i].extendeffect) {
			case NO_EFFECT:
        break;	
			case FADEIN:
      break;
			case FADEOUT:
				if (lighting[i].extendTimer > 10) {
				  if (lighting[i].extendlevel > 0) {              
				    lighting[i].extendlevel--;                    // fade out time = extendlevel * extendTimer = 255 * 10 = 2.55S
				  } else {
				    lighting[i].extendlevel= 0;
				  	lighting[i].extendeffect = NO_EFFECT;         //  reset extendeffect
				  	lighting[i].effect = lighting[i].pendingeffect;// restore effect
            if (lighting[i].pendingeffect == CHROMA_EFX_REACTIVE) {
              lighting[i].extendlevel = 0xFF;
              initialfx(&lighting[i]);
            }
						if (lighting[i].pendingeffect ==  CHROMA_EFX_OFF) {
              initialfx(&lighting[i]);                    // reinitial the fx variable to 0 for next fade in from dark.
            }
					  if (i >= (NUMBER_OF_LEDS - 1)) {                // to check all LED was fade out then
						  SUSPEND_LED_fadoutflag = 0;                 // when suspend, two led is turn off, then sleep
            }
				  }	
				  lighting[i].extendTimer = 0;
			  }				
      break;	
			case MATRIXDISSOLVE:     
        matrixDissolve(&lighting[i]);		
			break;
      default:
      break;		
		}	
	 }
  }

//	if (ProfilePress == 0) {                                          // when profile assign,ProfilePress = 1
//	  if (flag_Scroll_Wheel_Led_Blink) {                              // Auto calibration
//	  	if ((CalibrationTimer / 500) & 1) {
//	  		 ledPwmData[SCROLL_LED1].r =ledPwmData[SCROLL_LED2].r=0;                // scroll red
//	       ledPwmData[SCROLL_LED1].g =ledPwmData[SCROLL_LED2].g=255;                // scroll blue
//	       ledPwmData[SCROLL_LED1].b =ledPwmData[SCROLL_LED2].b=0;               // scroll green
//	  	} else {                                                      
//	  		ledPwmData[SCROLL_LED1].r =ledPwmData[SCROLL_LED2].r=0;                // scroll red
//	      ledPwmData[SCROLL_LED1].g =ledPwmData[SCROLL_LED2].g=0;                // scroll blue
//	      ledPwmData[SCROLL_LED1].b =ledPwmData[SCROLL_LED2].b=0;               // scroll green
//	  	}
//	  } 
//	}
//	
//	iicIndex=(iicIndex>=(LED_NUMBERS*3-1))?0:(iicIndex+1);                                      // every cycle write one byte value to iic               
//	if((iicIndex%3)==0){
////		ledDriverWrite(LED_CTLR2_ADDR,ledInfor[iicIndex/3].raddr,ledPwmData[iicIndex/3].r);
//	}else if((iicIndex%3)==1){
////		ledDriverWrite(LED_CTLR2_ADDR,ledInfor[iicIndex/3].gaddr,ledPwmData[iicIndex/3].g);
//	}else{
////		ledDriverWrite(LED_CTLR2_ADDR,ledInfor[iicIndex/3].baddr,ledPwmData[iicIndex/3].b);
//	}	
	//RollerLED
	SN_CT16B1->MR3 = ledPwmData[0].r;
	SN_CT16B1->MR4 = ledPwmData[0].g;
	SN_CT16B1->MR5 = ledPwmData[0].b;
	//DPI LED
	SN_CT16B1->MR0 =  DPICOLOR[user.profile[user.ProNO].sensor.dpiStage].r;//ledPwmData[1].r;
	SN_CT16B1->MR1 =  DPICOLOR[user.profile[user.ProNO].sensor.dpiStage].g;//ledPwmData[1].g;
	SN_CT16B1->MR2 =  DPICOLOR[user.profile[user.ProNO].sensor.dpiStage].b;//ledPwmData[1].b;
	//Logo LED
	SN_CT16B1->MR21 = ledPwmData[2].r;
	SN_CT16B1->MR22 = ledPwmData[2].g;
	SN_CT16B1->MR23 = ledPwmData[2].b;	
	//LED1
	SN_CT16B1->MR11 = ledPwmData[3].r;
	SN_CT16B1->MR19 = ledPwmData[3].g;
	SN_CT16B1->MR20 = ledPwmData[3].b;
	//LED2
	SN_CT16B1->MR14 = ledPwmData[4].r;
	SN_CT16B1->MR15 = ledPwmData[4].g;
	SN_CT16B1->MR16 = ledPwmData[4].b;
	//LED3
	SN_CT16B1->MR17 = ledPwmData[5].r;
	SN_CT16B1->MR18 = ledPwmData[5].g;
	SN_CT16B1->MR10 = ledPwmData[5].b; 
#endif   
}


void setTransitionTarget (CHROMA_PARM *led, COLOR *C)
/*++

Function Description:
  This function indexes the next target color value and initailizes the color values
  as well as saving the current color values.
 
Arguments:
  led = Pointer to the specified LED parameters.
  fx  = Pointer to the corresponding Disolve parameters.

Returns: 
  NONE

--*/
{
	COLOR c={0,0,0,0};
	U8 i;
  LED_FADE *fx =&matrixStaging[led[0].index][0];
  led[0].extendTimer = 0;
	if(led[0].pendingeffect==CHROMA_EFX_WAVE){       // Wave color is hard code value not from protocol command
    for(i=0;i<7;i++){
			fx[i].new.r=waveBuffer[i].r;
			fx[i].new.g=waveBuffer[i].g;
			fx[i].new.b=waveBuffer[i].b;
			fx[i].new.v=waveBuffer[i].v;
		}
		memcpy(waveCycleBuffer[led[0].index],waveBuffer,sizeof(waveBuffer));
	}else {
		c.r=(*C).r;
	  c.g=(*C).g;
	  c.b=(*C).b;
	  c.v=(*C).v;
	  setStageOneColor(fx,NEW|ACOLOR,c);               // Set RGB in mix
	}
}
 int getStagingV(LED_FADE *fx,U8 dis)
/*
 Function Description:
     This function get the matrixstaging v value to judge 
 if need to fadeout or dissolve.
 Arguments:
     fx: matrixstaing pointer
    dis: 0 get mix.v
    dis: 1 get new.v
    dis: 2 get old.v
 retrun 
    if one of the v is not equit 0 ,return 1
 otherwise 0
*/
 {
	 U8 i;
	 U8 ret=0;
	 for(i=0;i<MAXNUMBER_LEDS;i++){
		 if((dis&MIX)&&(fx[i].mix.v!=0)){// Get the fx[0].mix.v stage
			 ret=1;break;
		 }else
		 if((dis&NEW)&&(fx[i].new.v!=0)){// Get the fx[0].new.v stage
			 ret=1;break;
		 }else
		 if((dis&OLD)&&(fx[i].old.v!=0)){// Get the fx[0].new.v stage
			 ret=1;break;
		 }
	 }
	 return ret;
 }
 void decodeExternEffect (CHROMA_PARM *led,LED_FADE *fx) 
{
	U8 i;
	U8 mixV=getStagingV(fx,MIX);
	U8 newV=getStagingV(fx,NEW);
  if (mixV != 0) {                                 // if current was "ON" then 
		 if (newV != 0) {                               // if new target was "ON" then       
		   for(i=0;i<MAXNUMBER_LEDS;i++){
				fx[i].mix.v = ((U32)(fx[i].mix.v * lighting[led[0].index].extendlevel)) >> 8; // fast switching fade out and dissolve 
			 }
       led[0].extendeffect = MATRIXDISSOLVE;                    // DISSOLVE to the new target 
			 led[0].pwm = 0;
		 } else {                                                  // From on to off
			 for(i=0;i<MAXNUMBER_LEDS;i++){
				fx[i].mix.v = ((U32)(fx[i].mix.v * lighting[led[0].index].extendlevel)) >> 8; // others effect to reactive,when fade out, set OFF, aviod the led brightness saltation
			 }
       if (led[0].effect == CHROMA_EFX_SPECTRUM) {
         led[0].pwm = 0;                                 // spectrum to fadeout ,clear the pwm,insure fadeout 2.55s
       }
       if (led[0].effect == CHROMA_EFX_BREATHE) {
         if (lighting[led[0].index].extendeffect == MATRIXDISSOLVE) {
           led[0].pwm = lighting[led[0].index].pwm << 8;  // breathe run tranfer, set to reactive, setting pwm
         } else {
           led[0].pwm = lighting[led[0].index].pwm;       // when breathe set fade out, the lighting pwm copy to user
         }
       }
       led[0].extendeffect = FADEOUT;                     // FADE Out to the new target 
		 }
	 } else {                                               // if  the new target was "OFF" then 
	   if (newV != 0) {                                  // Off to on 
       led[0].extendeffect = MATRIXDISSOLVE;          // transision to target color
			 led[0].pwm = 0;
		 } else {                                             // if preivous target was "OFF" then 
		   led[0].extendeffect = NO_EFFECT;                   // do nothing 
			 led[0].effect = led[0].pendingeffect;              // restore effect
       if (led[0].effect != CHROMA_EFX_REACTIVE) {
         initialfx(led);                                  // Set off
       }
		 }
	 }  
   led[0].extendlevel = 255;                              // level, fade out and dissolve time = extendTimer * extendlevel = 10 * 255 = 2550ms
   led[0].extendTimer = 0;                                // timer, 1 * 10 = 10ms
	 for(i=0;i<MAXNUMBER_LEDS;i++){                         // Copy mix to old	 
		 fx[i].old.r = fx[i].mix.r;
	   fx[i].old.g = fx[i].mix.g;
	   fx[i].old.b = fx[i].mix.b;
	   fx[i].old.v = fx[i].mix.v;
	 }
   if(previousEffect[led[0].index]==CHROMA_EFX_WAVE){                  // Need to copy wave current buffer because it is not using stating
		 for(i=0;i<MAXNUMBER_LEDS;i++){
			matrixStaging[led[0].index][i].old.r=waveCycleBuffer[led[0].index][i].r;
			matrixStaging[led[0].index][i].old.g=waveCycleBuffer[led[0].index][i].g;
			matrixStaging[led[0].index][i].old.b=waveCycleBuffer[led[0].index][i].b;
			matrixStaging[led[0].index][i].old.v=waveCycleBuffer[led[0].index][i].v;
		 }
		 
	 }
}
void matrixDissolve(CHROMA_PARM *led)
{
//	U8 i;
//	LED_FADE *fx = &matrixStaging[led[0].index][0];
//	if(led[0].extendTimer >= 10){
//		led[0].extendTimer=0;
//		led[0].pwm += 1; 
//		if(led[0].pwm<0xff){
//	    for(i=0;i<7;i++){
//	      fx[i].mix.r=(U8)((((U16)fx[i].old.r << 8) + (led[0].pwm * (fx[i].new.r - fx[i].old.r)) + 0x80) >> 8);
//		    fx[i].mix.g=(U8)((((U16)fx[i].old.g << 8) + (led[0].pwm * (fx[i].new.g - fx[i].old.g)) + 0x80) >> 8);
//		    fx[i].mix.b=(U8)((((U16)fx[i].old.b << 8) + (led[0].pwm * (fx[i].new.b - fx[i].old.b)) + 0x80) >> 8);
//		    fx[i].mix.v=(U8)((((U16)fx[i].old.v << 8) + (led[0].pwm * (fx[i].new.v - fx[i].old.v)) + 0x80) >> 8);
//	    }
//	  }else{
//			for(i=0;i<7;i++){
//			  fx[i].mix.r=matrixStaging[led[0].index][i].old.r=fx[i].new.r;
//			  fx[i].mix.g=matrixStaging[led[0].index][i].old.g=fx[i].new.g;
//			  fx[i].mix.b=matrixStaging[led[0].index][i].old.b=fx[i].new.b;
//			  fx[i].mix.v=matrixStaging[led[0].index][i].old.v=fx[i].new.v;			
//			}
//			led[0].tmr = 0;
//	    led[0].effect = led[0].pendingeffect;
//      led[0].extendeffect = NO_EFFECT;
//			if (led[0].effect ==  CHROMA_EFX_BREATHE ) {        // breathing effect will start from on to breath out after fade in. 
//			   led[0].pwm = 0xFFFF;                              // Set top value
//         led[0].state &= ~RISING;                          // Prepare for falling pulse after 'On Period'
//         led[0].scratch = user.breathParam.pulseOn + 1490;                  // Next processing after 'On Period
//			 }
//		}
//		updateMatrixHardware(led[0].id ,ON);
//  }
}


void transitionLedEffect(CHROMA_PARM *led)
/*++

Function Description:
  This function dissolves between the current color and the specified designation 
  color. The color value includes the brightness value. Once the designation is 
  reached, a new target is specified and the procress starts all over again. This
  function supports two different effects the Cycling Spectrum and Dissolve. The
  Dissolve feature is automatically invoked between colors if the effect is set to
  Static and the Transition Step and Time are non-zero.
 
Arguments:
  led = Pointer to the specified LED parameters.

Returns: 
  NONE

--*/
{
//  LED_FADE *fx = &matrixStaging[led[0].index][0];
//  U8 i;
//  if (led[0].tmr >= led[0].scratch) {                   // If time to update Dissolve or Stage, then...
//    led[0].tmr = 0;                                     // Restar time counter.
//    led[0].pwm += led[0].faderStep;                     // Update accumulated step value
//	  if (led[0].pwm < 0xFF) {                            // If not at 100% of transition, then...
//			  for(i=0;i<MAXNUMBER_LEDS;i++){
//          fx[i].mix.r = (U8)((((U16)fx[0].old.r << 8) + (led[0].pwm * (fx[i].new.r - fx[i].old.r)) + 0x80) >> 8);
//          fx[i].mix.g = (U8)((((U16)fx[0].old.g << 8) + (led[0].pwm * (fx[i].new.g - fx[i].old.g)) + 0x80) >> 8);
//          fx[i].mix.b = (U8)((((U16)fx[0].old.b << 8) + (led[0].pwm * (fx[i].new.b - fx[i].old.b)) + 0x80) >> 8);
//          fx[i].mix.v = (U8)((((U16)fx[0].old.v << 8) + (led[0].pwm * (fx[i].new.v - fx[i].old.v)) + 0x80) >> 8);
//					
//				}
//	  } else {
//			  for(i=0;i<MAXNUMBER_LEDS;i++){
//          fx[i].mix.r=fx[i].old.r = fx[i].new.r;
//          fx[i].mix.g=fx[i].old.g = fx[i].new.g;
//          fx[i].mix.b=fx[i].old.b = fx[i].new.b;
//          fx[i].mix.v=fx[i].old.v = fx[i].new.v;
//				}  
//		  setSpectumTransitionTarget(led);                  // reset spectrum parameter
//    }
//    updateMatrixHardware(led[0].id, ON);                   // Update corresponding to the 'on' value
//  }
}

void forceLightingReload(void)
/*++

Function Description:
  This function forces a update of all lighting by copying the current profile's
  'saved' lighting structure to the 'active' lighting structure and then forcing
  an update on the next interrupt. The 'index' value is initialized, to be used 
  by all lighting effect routines. The Pending transition flag is also cheared.
  After initializing all of the LED state, the 'lightingMode' is also processed
  leaving the lighting scheme the user has set.
 
Arguments:
  NONE

Returns: 
  NONE

--*/
{
//  U8 i;
//  LED_REFRESH_TIME =  user.profile[user.ProNO].Runing_light_LED_REFRSH_Time; 
//  for (i = 0; i < NUMBER_OF_LEDS; i++) {
//	  user.profile[user.ProNO].savedLight[i].index = i;                   // make sure the index is correct
//    Load_LED_Effect(&user.profile[user.ProNO].savedLight[i]); 	        // load LED effect
//    copyLighting2Active(&user.profile[user.ProNO].savedLight[i]);       // copy user to light		
//  }
//	loadProfileLed();   // Load the profile indicate led
}
void copyLighting2Active(CHROMA_PARM *led)
/*++

Function Description:
  This function copies the specified LED 'saved' state to the 'active' lighting
  structure. This includes the state parameters and the calibration values. Then
  this function forces and update to the specified LED on the next interrupt.

Arguments:
  led_id - LED ID of data to copy.

Returns: 
  NONE

--*/
{
//  U8 i, j;  
//  U8 *d, *s;
//                                                         // in lily project since all parameters are used we will copy all by setting id & index position
//  s = (U8 *)led;                                         // Source = 'saved' LED parameters
//  d = (U8 *)&lighting[led[0].index];                     // Destination = 'active' LED parameters
//  j = (&lighting[0].index + 1) - &lighting[0].id;        // Do not override the stratch registers 
//	for (i = 0; i < j; i++) {                
//    *d++ = *s++;                                         // Copy variables saved in Flash Storage
//  }

//  s = (U8 *)&sys.maxLight[led[0].index];             // Source = 'saved' calibration values
//  d = (U8 *)&calibrating[led[0].index];                   // Destination = 'active' LED calibration
//  for (i = 0; i < sizeof(LED_MAXM); i++) {                
//    *d++ = *s++;                                          // Copy variables saved in Flash Storage
//  }
}


CHROMA_PARM *ledParameters(U8 profile_id, U8 led_id)
/*++

Function Description:
  This function finds the address of the corresponding LED parameters. If the profile
  ID value is zero, the address is returned from the active "lighting" structure that
  are current active structures and are not saved. If profile is not zero the address
  is of the LED ID in the corresponding profile (saved sturctures). If there no match
  to the LED ID, the return is a placeholder at the end of the 'active' list of LEDs.
 
Arguments:
  profile_id - Profile ID value.
  led_id     - LED ID value.

Returns: 
  address    - Pointer to corresponding LED parameters.

--*/
{
//  U8 i = 0;
//  while ((user.profile[user.ProNO].savedLight[i].id != led_id) && (i < NUMBER_OF_LEDS)) {    
//    i++;                                                  // Look for parameters with matching LED_ID
//  }
//  if ((profile_id == 0) || (i == NUMBER_OF_LEDS)) {       // If Class #0 or invalid LED ID, then...
//    return (&lighting[i]);                                // Return 'active' LED parameter address
//  } else {
//    return (&user.profile[profile_id-1].savedLight[i]);  // Return 'saved' LED parameter address
//  }
  return 0;
}


void BasicFwLedEffect(void)
{
#if 0  
static U8 r, g, b;
	static U8 BasicFwLedstatus =0;
	U8 i;
	if (basicFWTimer > 500) {
		if (BasicFwLedstatus == 0) {
			 r = 0xff;
			 g = 0x00;
			 b = 0x00;
			 BasicFwLedstatus ++;
		 } else if (BasicFwLedstatus == 2) {
			 r = 0x00;
			 g = 0xff;
			 b = 0x00;
			 BasicFwLedstatus ++;
		 } else if (BasicFwLedstatus == 4) {
			 r = 0x00;
			 g = 0x00;
			 b = 0xff;
			 BasicFwLedstatus++;
		 } else if (BasicFwLedstatus == 6) {
			 r = 0xff;
			 g = 0xff;
			 b = 0xff;
			 BasicFwLedstatus++;
		 } else {
			 if (BasicFwLedstatus >= 7)
				 BasicFwLedstatus=0;
			 else 
				 BasicFwLedstatus++;
			 r = 0x00;
			 g = 0x00;
			 b = 0x00;
		 }
     for(i=0;i<LED_NUMBERS;i++){                             // Write basic color to buffer
//			 ledDriverWrite(LED_CTLR2_ADDR,ledInfor[i].raddr,r);
//			 ledDriverWrite(LED_CTLR2_ADDR,ledInfor[i].gaddr,g);
//			 ledDriverWrite(LED_CTLR2_ADDR,ledInfor[i].baddr,b);
		 }		 
		 basicFWTimer = 0;
	}
#endif
}




void ledDriverWrite(U8 device, U8 reg, U8 value)
/*++

Function Description:
  This function writes the specified LED Driver's register and does not wait for the transmission
  to complete. This allows the setting of LED states to occur in the background without taking up
  time in the foreground (or interrupt handler).
 
Arguments:
  device - Index of which PCA9633 to Write (0xFF = all).
  reg    - Register to Write.
  value  - Value to to Write.

Returns: 
  NONE

--*/
{
//  i2cWrite(device, reg, &value, 1);
//	if (IICStates == I2C_TIME_OUT){                       // ivan add to reset the i2c bus if timeout
//		i2c_initialization(MASTER);
//		led_initialization();
//	}
}

void ledDriverRead(U8 device, U8 reg, U8 *data, U8 length)
/*++

Function Description:
  This function reads the specified LED Driver's register and waits until the
  communication is complete. Reading with the index equal to '0xFF' (All), will 
  give indetermine results.
 
Arguments:
  device - Index of which PCA9633 to read (cannot use 'all' index).
  reg    - Register to read.

Returns: 
  NONE.

--*/
{
//  i2cRead(device, reg, data, length);
}

void SUSPEND_led_fadout()
{
  U8 i;
    COLOR c={0,0,0,0};
  for (i = 0; i < NUMBER_OF_LEDS; i++) {
    lighting[i].Chroma_effect.r = 0;
    lighting[i].Chroma_effect.g = 0;
    lighting[i].Chroma_effect.b = 0;
    lighting[i].Chroma_effect.v = 0;
		if(lighting[i].effect!=CHROMA_EFX_WAVE){  // Not for wave effect
      lighting[i].effect = lighting[i].pendingeffect;
      setTransitionTarget(&lighting[i],(COLOR *)&lighting[i].Chroma_effect.r); 
      c.v = 0xFF;
		  setStageOneColor(&matrixStaging[i][0],MIX|V,c);
	    decodeExternEffect(&lighting[i], &matrixStaging[i][0]);
		}else{
		  lighting[i].extendeffect = FADEOUT;                     // FADE Out to the new target
		  lighting[i].extendlevel = 255;                              // level, fade out and dissolve time = extendTimer * extendlevel = 10 * 255 = 2550ms
      lighting[i].extendTimer = 0;                                // timer, 1 * 10 = 10ms
		}
  }
	SUSPEND_LED_fadoutflag = 1;
}

void setStageOneColor(LED_FADE *fx,U8 dis,COLOR c)
/*
Function Description:
    Function is to set the matrix stage to one color ,
can set rgb value or v value or both of them chose by dis bit :5~7
Arguments:
    fx   : The matrix staging to set
    dis 0: Set .mix color
    dis 1: Set .new color
    dis 2: Set .old color
  dis 3~4: Resered
    dis 5: Just RGB value
    dis 6: Just V value
    dis 7: RGB+V value    
    C    : Target color
Return:
    None
*/
{
	U8 i;
  if(dis&ACOLOR){	                          // Set all color ,include rgb value and v value
		for(i=0;i<MAXNUMBER_LEDS;i++){
			if(dis&MIX){
				 fx[i].mix=c;
			}
			if(dis&NEW){
				 fx[i].new=c;
			}
			if(dis&OLD){
				 fx[i].old=c;
			}				
		}			
  }else if(dis&V){                         // Set V		
		for(i=0;i<MAXNUMBER_LEDS;i++){
			if(dis&MIX){
				 fx[i].mix.v=c.v;
			}
			if(dis&NEW){
				 fx[i].new.v=c.v;
			}
			if(dis&OLD){
				 fx[i].old.v=c.v;
			}				
		}		
	}else if(dis&RGB){                      // Set RGB
   	for(i=0;i<MAXNUMBER_LEDS;i++){
			if(dis&MIX){
				 fx[i].mix.r=c.r;
				 fx[i].mix.g=c.g;
				 fx[i].mix.b=c.b;
			}
			if(dis&NEW){
				 fx[i].new.r=c.r;
				 fx[i].new.g=c.g;
				 fx[i].new.b=c.b;
			}
			if(dis&OLD){
				 fx[i].old.r=c.r;
				 fx[i].old.g=c.g;
				 fx[i].old.b=c.b;
			}				
		}	
	}
}
void ledTiming(void)
{
	U8 i;
	   for (i = 0; i < NUMBER_OF_LEDS; i++) {
        lighting[i].tmr++;
				lighting[i].extendTimer++;
     }
		for (i = 0; i < NUMBER_OF_LEDS; i++) {
      if (lighting[i].pendingeffect == CHROMA_EFX_REACTIVE){
			  if(after_glow_ready[lighting[i].index] == 1){
			  	++TrailTimerCnt[lighting[i].index];
			  	if (TrailTimerCnt[lighting[i].index] >= TrailFadeoutTime[lighting[i].index]) {
			  		TrailTimerCnt[lighting[i].index] = 0;
			  	  if((Trail_Parameter[lighting[i].index].Trailflag == 1) &&(Trail_Parameter[lighting[i].index].TrailPWM > 0)) {
			  			Trail_Parameter[lighting[i].index].TrailPWM--;
			  	  } else{
              ReactivePeriod[i] = 1;
			  		  Trail_Parameter[lighting[i].index].Trailflag =0;
			  		  after_glow_ready[lighting[i].index]=0;
			  	  }
				  }
			  } else {
				  TrailTimerCnt[lighting[i].index]=0;
			  }
		  }
    }
		 for(i=0;i<MAX_ACTIVE_MACRO;i++){
		  if(macroActive[i].flag==TRUE){    // It is not active now
				if(macroActive[i].timer>0)macroActive[i].timer--;
		  }
	  }
		if(turbo.kb.stage){ // If the turbo key is  running
			for(i=0;i<NUMBER_OF_BUTTONS;i++){
				turbo.kb.timer[i]++;
			}
		}
		if(turbo.btn.stage){ // If the turbo key is  running
			for(i=0;i<NUMBER_OF_BUTTONS;i++){
				turbo.btn.timer[i]++;
			}
		}
		if(scrollTimer.upTimer>0){
			scrollTimer.upTimer--;
		}
		if(scrollTimer.dnTimer>0){
			scrollTimer.dnTimer--;
		}
}
