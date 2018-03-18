
#ifndef __LED_RAMSETTINNG_H__
#define __LED_RAMSETTINNG_H__

#include <SN32F240B.h>


typedef struct
{
	uint8_t bDirection;
	uint8_t bTableIndex;
	uint8_t bColorIndex;
	uint8_t bRed;
	uint8_t bGreen;
	uint8_t bBlue;	
}BreathEffect;

//typedef struct
//{
//	uint8_t bRed;
//	uint8_t bGreen;
//	uint8_t bBlue;
//}COLOR;
	
typedef union {
	uint32_t w_aryLED_InfoPageTemp[16];
	uint8_t  b_aryLED_InfoPage[64];		
}LED_Profile;

//extern 	COLOR structLEDxCn[];
extern 	BreathEffect	structLED_BreathEffect;
extern	LED_Profile	unionLED_Profile;

//===========================//
//** LED Variable     			 //
//===========================//
extern uint32_t wLED_RandomValue;
extern uint32_t wLED_RandomValue_N_1;
extern uint32_t wLED_RandomValue_N_2;

extern uint8_t 	bLED_DataRefreshTime_Reload;
extern uint8_t  bLED_ReativeJustMake;

extern uint8_t	bLED_StepPWM;
extern uint8_t 	bLED_MaxPWM;

extern uint8_t bLED_Mode;

extern uint8_t rPWM_Buf[];
extern uint8_t gPWM_Buf[];
extern uint8_t bPWM_Buf[];

extern uint8_t bReload_MR[];

extern uint8_t rPWM_Value;
extern uint8_t gPWM_Value;
extern uint8_t bPWM_Value;

extern uint8_t rBREATH_Tab_Index;
extern uint8_t rPWM_BREATH_Value;
extern uint8_t gPWM_BREATH_Value;
extern uint8_t bPWM_BREATH_Value;

extern uint8_t rSPECT_Tab_Index;
extern uint8_t rPWM_SPECT_Value;
extern uint8_t gPWM_SPECT_Value;
extern uint8_t bPWM_SPECT_Value;

extern uint8_t rREAT_Tab_Index;
extern uint8_t rPWM_REAT_Value;
extern uint8_t gPWM_REAT_Value;
extern uint8_t bPWM_REAT_Value;

extern uint8_t rPWM_BLINK_Value;
extern uint8_t gPWM_BLINK_Value;
extern uint8_t bPWM_BLINK_Value;

extern uint8_t bLED_Rbuf[];
extern uint8_t bLED_Gbuf[];
extern uint8_t bLED_Bbuf[];

extern uint8_t rTab_Index;
extern uint8_t bLED_Light_Param;

extern uint32_t wLED_Status;

extern uint8_t b_aryLED_LEDM2StepCx[];

#endif //__LED_RAMSETTINNG_H__
