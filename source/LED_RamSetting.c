
#include "LED_RamSetting.h"
#include "LED_Const.h"
#include "variables.h"
//===========================//
//** LED Variable     		 //
//===========================//
uint32_t 	wLED_RandomValue;
uint32_t 	wLED_RandomValue_N_1;
uint32_t 	wLED_RandomValue_N_2;


uint8_t 	bLED_DataRefreshTime_Reload = 0;
uint8_t 	bLED_ReativeJustMake = 0;

//** LED Effect: Spectrum Variable
uint8_t bLED_StepPWM = 0;
uint8_t bLED_MaxPWM = 0;
 
uint8_t b_aryLED_LEDM2StepCx[21] = {0};


LED_Profile	unionLED_Profile;
COLOR 	structLEDxCn[2];
BreathEffect	structLED_BreathEffect;

uint8_t bLED_Mode;

//0-RollerLED 1-DPILED 2-LogoLED
//3-5 LightLED 
uint8_t rPWM_Buf[6];
uint8_t gPWM_Buf[6];
uint8_t bPWM_Buf[6];

//PWM MR Reload
uint8_t bReload_MR[18];

uint8_t rPWM_Value;
uint8_t gPWM_Value;
uint8_t bPWM_Value;

uint8_t rBREATH_Tab_Index;
uint8_t rPWM_BREATH_Value;
uint8_t gPWM_BREATH_Value;
uint8_t bPWM_BREATH_Value;

uint8_t rSPECT_Tab_Index;
uint8_t rPWM_SPECT_Value;
uint8_t gPWM_SPECT_Value;
uint8_t bPWM_SPECT_Value;

uint8_t rREAT_Tab_Index;
uint8_t rPWM_REAT_Value;
uint8_t gPWM_REAT_Value;
uint8_t bPWM_REAT_Value;

uint8_t rPWM_BLINK_Value;
uint8_t gPWM_BLINK_Value;
uint8_t bPWM_BLINK_Value;

uint8_t bLED_Rbuf[6];
uint8_t bLED_Gbuf[6];
uint8_t bLED_Bbuf[6];

uint8_t rTab_Index;
uint8_t bLED_Light_Param = LIGHT_LENGTH;

//------------LED Status---------------
uint32_t	wLED_Status = 0;





