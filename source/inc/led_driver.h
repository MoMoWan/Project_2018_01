/*++

Copyright (c) 2009-2010 Razer Ltd.
All rights reserved. This program contains proprietary and confidential information.

Module Name:
  led_driver.h

Compiler:
  Keil Software ?ision v4.03q, with NXP Semiconductor LPC1343 proccessor support.

Abstract:
  This file contains the definitions for led_driver.c, which provides all of the
  lighting effects and LED drive support.

--*/

#ifndef _LED_DRIVER_H_
#define _LED_DRIVER_H_

#include "main.h"
#include "IS31FL3731.h"


#define IDX_U4    0
#define IDX_U5    1
#define IDX_U6    2
#define IDX_U7    3
#define IDX_UN  0xFF 


#define INIT_LED_V            0x00                        // Init led brightness value
#define INIT_LED_CURRENT      0xF0                        // Init led current value: 30mA and instant change mode
#define BK                    0xFF

#define OFF                   0
#define ON                    1                           // LED logic values
#define PWM                   2

#define DIMMER_STEPS          4                           // Number of brightness steps in Dimmer Table

//#define LED_ALL_ID            0x00                        // Protocol LED_ID definitions
//#define LED_WHEEL_ID          0x01
//#define LED_DPI_ID            0x02
//#define LED_BATTERY_ID        0x03
//#define LED_LOGO_ID           0x04
//#define LED_BCKLIT_ID         0x05
//#define LED_APM_ID            0x06
//#define LED_MACRO_ID          0x07
//#define LED_GAME_ID           0x08
//#define LED_CONNECTED_ID      0x09
//#define LED_UNDERGLOW_ID      0x0A
//#define LED_STRIPE_ID         0x0B
//#define LED_R_MAP_ID          0x0C
//#define LED_G_MAP_ID          0x0D
//#define LED_B_MAP_ID          0x0E
//#define LED_KEY_MATRIX_ID     0x05

//#define STATIC               (0 << 1)                      // LED effects: Static, no effect
////#define BLINKING             (1 << 1)                      // LED effects: Blinking
////#define PULSATING            (2 << 1)                      // LED effects: Pulsating
////#define CONTROLLED           (3 << 1)                      // LED effects: Software Controlled
//#define SPECTRUM             (4 << 1)                      // LED effects: R-G-B Cycle Spectrum
//#define DISSOLVE             (5 << 1)                      // LED effects: R-G-B Dissolving
//#define SPECTRUM_PULSATING   (6 << 1)                      // LED effects: Changing color when plusating
//#define FADEOUT              (6 << 1)                     // LED effects: fadeout 
//#define FADEIN               (7 << 1)                     // LED effects: fadein


#define NO_EFFECT            0x00
#define FADEIN               0x01                     // LED effects: fadein
#define FADEOUT              0x02                     // LED effects: fadeout 
//#define DISSOLVE             0x03                          // LED effects: R-G-B Cycle Spectrum
#define MATRIXDISSOLVE       0x04                     // LED effects: any effect transisfer to wave




#define CHROMA_ALL_ID              0x00                   // Chroma Region ID: all region
#define CHROMA_WHEEL_ID            0x01                   // Chroma Region ID: scroll wheel LED
#define CHROMA_DPI_ID              0x02                   // Chroma Region ID: DPI LED
#define CHROMA_BATTERY_ID          0x03                   // Chroma Region ID: Battery LED
#define CHROMA_LOGO_ID             0x04                   // Chroma Region ID: Logo LED
#define CHROMA_BCKLIT_ID           0x05                   // Chroma Region ID: Backlight LED
#define CHROMA_APM_ID              0x06                   // Chroma Region ID: Actions/Minute LED
#define CHROMA_MACRO_ID            0x07                   // Chroma Region ID: Macro Recording LED
#define CHROMA_GAME_ID             0x08                   // Chroma Region ID: Game Mode LED
#define CHROMA_CONNECTED_ID        0x09                   // Chroma Region ID: Wirelessly Connected LED
#define CHROMA_UNDERGLOW_ID        0x0A                   // Chroma Region ID: Underglow LED
#define CHROMA_STRIPE_ID           0x0B                   // Chroma Region ID: Side Stripe LED
#define CHROMAR_MAP_ID             0x0C                   // Chroma Region ID: Keymap Red LED
#define CHROMAR_G_MAP_ID           0x0D                   // Chroma Region ID: Keymap Green LED
#define CHROMAR_B_MAP_ID           0x0E                   // Chroma Region ID: Keymap Blue LED
#define CHROMAR_DOCKED_ID          0x0F                   // Chroma Region ID: Docked LED
#define CHROMAR_RS_ID              0x10                   // Chroma Region ID: Right Side LED
#define	CHROMAR_LS_ID              0x11                   // Chroma Region ID: Left Side LED
#define CHROMAR_AL_ID							 0x12                   // Chroma Region ID: Alternate Logo LED
#define CHROMAR_POWER_ID           0x13                   // Chroma Region ID: Power LED
#define CHROMAR_SUSPEND_ID         0x14                   // Chroma Region ID: Susped LED

#define LED_SCROLL_ID                   0x01                   // Chroma Region ID: scroll wheel LED
#define LED_LOGO_ID                     0x02                   // Chroma Region ID: DPI LED
#define LED_TAIL_ID                     0x03                   // Chroma Region ID: Battery LED


#define CHROMA_SINGLE_COLOR        0x01                   // Chroma Region Data Type: Single-Color GPIO LED
#define CHROMA_TWO_COLOR           0x02                   // Chroma Region Data Type: Two-Color LED
#define CHROMA_RGB                 0x03                   // Chroma Region Data Type: RGB LED(RGB format)
#define CHROMA_RGB_HSL             0x04                   // Chroma Region Data Type: RGB LED(HSL format)
#define CHROMA_SC_PWM              0x05                   // Chroma Region Data Type: Single-Color PWM Led

#define CHROMA_EFX_OFF             0x00                   //Effect ID
#define CHROMA_EFX_STATIC          0x01
#define CHROMA_EFX_BREATHE         0x02
#define CHROMA_EFX_SPECTRUM        0x03
#define CHROMA_EFX_WAVE            0x04
#define CHROMA_EFX_REACTIVE        0x05
#define CHROMA_EFX_BASIC_RIPPLE    0x06
#define CHROMA_EFX_STARLITE        0x07
#define CHROMA_EFX_CUSTOM_FRAME    0x08

#define LED_ON                0x01                         // LED_PARM state: LED ON
#define EFFECT                0x0E                         // LED_PARM state: LED EFFECT
#define BLINK_ON              0x10                         // LED_PARM state: Blink "On Period"
#define RISING                0x10                         // LED_PARM state: Pulse is "Rising"
#define CHANGE                0x20                         // LED_PARM state: State change, update interrupt
#define LIMITED               0x40                         // LED_PARM state: Limited LED Effect
#define PENDING               0x80                         // LED_PARM state: Transition Pending

#define LIGHTING_DN           (0x00 << 0)
#define LIGHTING_UP           (0x01 << 0)
#define LIGHTING_LOOP         (0x01 << 1)

#define LED_R                 (0)
#define LED_G                 (1)
#define LED_B                 (2)

#define NUMBER_OF_FRAME         (2)                 // Use to run 
#define NUMBER_OF_FRAME_DEFINE   7                      // Defined in flash
#define shift_counter           13

#define    spectrum_effect                           0x01
#define    static_effect                             0x02
#define    pulsating_effect                          0x03

#define  B2T                   (0)  // Bottom to Top
#define  T2B                   (1)  // Top to Buttom

#define MIX                    (1<<0)    //  FOR mix in stage
#define NEW                    (1<<1)    //  For new in stage
#define OLD                    (1<<2)    //  For old in stage
#define RGB                    (1<<5)    //  For RGB in stage      
#define V                      (1<<6)    //  For V in stage
#define ACOLOR                 (1<<7)    //  For all color in stage


//[ Led information
#define LED_NUMBERS             18
#define LOGO_LED                0     // Logo led index
#define SCROLL_LED1             1     // Scroll led index
#define SCROLL_LED2             2
#define LED_L_NUM1              3     // Left side led index
#define LED_L_NUM2              4
#define LED_L_NUM3              5
#define LED_L_NUM4              6
#define LED_L_NUM5              7
#define LED_L_NUM6              8
#define LED_L_NUM7              9
#define LED_R_NUM1              10    // Right side led index
#define LED_R_NUM2              11
#define LED_R_NUM3              12
#define LED_R_NUM4              13
#define LED_R_NUM5              14
#define LED_R_NUM6              15
#define LED_R_NUM7              16
#define LED_PROFILE             17
//]


typedef __packed struct _SCRIPT {                         // LED Dissolve Script structure
  U16 faderTime;                                           // When to step
  U16 faderStep;                                           // Amount to step
  U8  r;                                                   // Target Red value
  U8  g;                                                   // Target Green value
  U8  b;                                                   // Target Blue value
  U8  v;                                                   // Target Brightness value
} SCRIPT;

typedef __packed struct _COLOR {                          // LED Color structure
  U8  r;                                                   // Target Red value
  U8  g;                                                   // Target Green value
  U8  b;                                                   // Target Blue value
  U8  v;                                                   // Target Brightness value
} COLOR;

typedef __packed struct _RANDOMCOLOR {                          // LED Color structure
  U8  r;                                                   // Target Red value
  U8  g;                                                   // Target Green value
  U8  b;                                                   // Target Blue value
  U8  v;                                                   // Target Brightness value
} RANDOMCOLOR;

typedef __packed struct _EFFECT_PARM {
  U8  flags;                                               // 
  U8  rate;
	U8  colorNO;
	U8  r;                                                   // Current Red "on" value
  U8  g;                                                   // Current Green "on" value
  U8  b;                                                   // Current Blue "on" valu
	U8  v;
  U8  rr;
	U8  gg;
	U8  bb;
	U8  vv;
} EFFECT_PRAM;

typedef __packed struct _LED_PARM {                       // LED Special Effect structure
  U8  id;                                                  // LED ID value
  U8  state;                                               // B7=Pending, B6=Limited, B5=Change, B4=Flag, B3-1=Effect, B0=On
  EFFECT_PRAM Chroma_effect;
	U8  effect;                                           //
	U8  extendeffect;
  U8  pendingeffect;
  U8  extendlevel;
	U16 extendTimer; 
  U8  level;                                               // level: Overall brightness
  U8  blinkOn;                                             // Length of "On Period" for Blinking (100ms)
  U8  blinkOff;                                            // Length of "Off Period" for Blinking (100ms)
//  U16 pulseOn;                                             // Length of "On Period" for Pulsating (1ms)
//  U16 pulseOff;                                            // Length of "Off Period" for Pulsating (1ms)
//  U16 pulseRiseTime;                                       // Length of "Step Rising step Time" for Pulsating (1ms)
//  U16 pulseRise1Step;                                      // Size of "Rise 1 Step" in magnitude for Pulsating
//  U16 pulseRise2Step;                                      // Size of "Rise 2 Step" in magnitude for Pulsating
//  U16 pulseFallTime;                                       // Length of "Fall Step Time" for Pulsating (1ms)
//  U16 pulseFall1Step;                                      // Size of "Fall 1 Step" in magnitude for Pulsating
//  U16 pulseFall2Step;                                      // Size of "Fall 2 Step" in magnitude for Pulsating
//  U16 pulseNeckLine;                                       // Size of "Neck line" of Rise or Fall 1 and 2
  U16 transTime;                                           // Length of "Step Time" for Transitioning (1ms)
  U16 transStep;                                           // Size of "Step" in magnitude for Transitioning
  U16 faderTime;                                           // Length of "Step Time" for Dissolving (1ms)
  U16 faderStep;                                           // Size of "Step" in magnitude for Dissolving
  U16 duration;                                            // Effect duration target time, if active (state.6 = 1)
  U16 scratch;                                             // Effect temporary variable to be used as needed
  U16 script;                                              // Effect temporary index to be used as needed
  U16 pwm;                                                 // Pulse Width Modulator temporary variable
  U16 tmr;                                                 // tmr recorder variables
  U8  index;                                               // LED_ID to Index value
} CHROMA_PARM;

typedef __packed struct _LED_MAXM {                       // LED Calibration structure
  COLOR max;                                               // Maximum color value
} LED_MAXM;

typedef __packed struct _LED_FADE {                       // LED Transitioning structure
  COLOR new;                                               // New color values
  COLOR mix;                                               // Mix color values
  COLOR old;                                               // Old color values
} LED_FADE;


typedef __packed struct _TRAIL_EFFECT_RAM {
  U8      TrailPWM;
  U8      Trailflag;          //begin to fade out or not  
} TRAIL_EFFECT_RAM;

typedef __packed struct _LED_REG{
	U8 num;
	U8 raddr;
	U8 gaddr;
	U8 baddr;
}LED_REG;

//-------------------------
extern COLOR ledPwmData[LED_NUMBERS];                                       // Pwm data to write to led driver
extern U8 wave_color_buffer_R[24];
extern U8 wave_color_buffer_G[24];
extern U8 wave_color_buffer_B[24];
//extern U8 Logo_R_led_value;                                          //led r g b value
//extern U8 Logo_G_led_value;
//extern U8 Logo_B_led_value;

//extern U8 Scroll_R_led_value;                                          //led r g b value
//extern U8 Scroll_G_led_value;
//extern U8 Scroll_B_led_value;
extern U8 ReactivePeriod[2];

extern U8  after_glow_ready[NUMBER_OF_LEDS];
extern U8 IICStates;
extern CHROMA_PARM lighting[NUMBER_OF_LEDS];
//extern U8 WaveLoopColor[4][3];
extern U8 LED_REFRESH_TIME;
//extern U8 ledEffect[NUMBER_OF_LEDS];
extern LED_MAXM calibrating[];
extern U8  TrailTimerCnt[NUMBER_OF_LEDS];
extern U8  TrailFadeoutTime[NUMBER_OF_LEDS];
extern U16 basicFWTimer;
extern COLOR CHROMAFRAME[16];
extern const RANDOMCOLOR randomList[16];
extern U8 breathType[NUMBER_OF_LEDS];
extern const U8 BreathingRandomColor[7][3];
extern U8 BreathingColor[2][3];
extern U8 flag_Scroll_Wheel_Led_Blink;
extern U16 Profiletime;
extern U8 ProfilePress;
extern U8 Profiledebounce;
extern U8 SUSPEND_LED_fadoutflag;
extern U8 randomcolorIdex;
extern U8 randomFlag;
extern U8 randomCompFlag;
extern U8 previousEffect[NUMBER_OF_LEDS];
extern TRAIL_EFFECT_RAM Trail_Parameter[NUMBER_OF_LEDS];            // ivan add for trail effect use
extern LED_FADE matrixStaging[NUMBER_OF_LEDS][MAXNUMBER_LEDS];           // Stage of matrix value
extern const U8 brightnesstable[64];
U8 RandomValue(U8 seed, U8 max_limit);
extern COLOR waveCycleBuffer[NUMBER_OF_LEDS][24];
extern const COLOR waveBuffer[24];

void 			initLedRgbSetting(U8 frame, U8 r, U8 g, U8 b);
void      led_initialization    (void);
void      led_driver_reconfig   (void);
void      updateLedHardwareLEDMatrix (void);
void      updateLightingEffects (void);
void      staticLedEffect       (CHROMA_PARM *led);
void      blinkingLedEffect     (CHROMA_PARM *led);
void      pulsatingLedEffect    (CHROMA_PARM *led);
void      setSpectumTransitionTarget   (CHROMA_PARM *led);
void      setTransitionTarget (CHROMA_PARM *led, COLOR* C);
void      decodeExternEffect (CHROMA_PARM *led,LED_FADE *fx); 
void      transitionLedEffect   (CHROMA_PARM *led);
void      setEffectParameters   (CHROMA_PARM *led, LED_FADE *fx);
void      forceLightingUpdate   (void);
void      forceLightingReload   (void);
void      copyLighting2Active   (CHROMA_PARM *led);
//void      copyActive2Lighting   (CHROMA_PARM *led);
CHROMA_PARM *ledParameters         (U8 profile_id, U8 led_id);
void      ledDriverWrite        (U8 device, U8 reg, U8 value);
void      ledDriverRead         (U8 device, U8 reg, U8 *data, U8 length);
void      initLedStateSetting   (void);
void      WaveEffect            (void);
void      Load_LED_Effect(CHROMA_PARM *led);
void      TrailClearPWM(void);
void      TrailAllSetPWM(void);
extern void BasicFwLedEffect(void);
extern   void initialfx(CHROMA_PARM *led);


extern void SUSPEND_led_fadout(void);

void matrixDissolve(CHROMA_PARM *led);
void updateMatrixHardware(U8 led_id,U8 state);
void waveEffect(CHROMA_PARM *led);
void setStageOneColor(LED_FADE *fx,U8 dis,COLOR c);
void ledTiming(void);
#endif // _LED_DRIVER_H_

