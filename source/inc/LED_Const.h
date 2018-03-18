
#ifndef __LED_CONST_H__
#define __LED_CONST_H__


#define PWM_MAX 	255

#define	LIGHT_LENGTH		5 
#define LIGHT_TABLE_LENGTH	6

#define LIGHT_PARAM 		0x10

//=============================//
//    LED Mode case          //
//=============================//
#define		S_LED_MODE_NONE  			0x00
#define		S_LED_MODE_MIN				0x01

#define		S_LED_MODE_STATIC  			0x01
#define		S_LED_MODE_BREATH 			0x02
#define		S_LED_MODE_SPECTRUM 		0x03
#define		S_LED_MODE_REACTION  		0x04

#define		S_LED_MODE_MAX  			0x04

#define		S_LED_MODE_WAVE 			0x05
#define		S_LED_MODE_EXPLOSION 		0x06
#define		S_LED_MODE_HEARTBEAT 		0x07
#define		S_LED_MODE_BLINK  		  0x08
#define		S_LED_MODE_USER_DEFINE 		0x80

//Refresh Time
#define	BREATH_REFRESH_TIME		30
#define REACTION_REFRESH_TIME	20
#define SPECTRUM_REFRESH_TIME	80
#define STATIC_REFRESH_TIME		10
#define WAVE_REFRESH_TIME		60
#define BLINK_REFRESH_TIME		100

#define LED_SPECTRUM_PWM_STEP_VALUE		0x05  //0x0F-0x01 

//wLED_Status
#define mskLED_ModeChange				(0x1<<0)

#define	mskLED_BreathPWMChangeFlag		(0x1<<3)
#define mskLED_BreathUpcntFlag			(0x1<<4)	
#define mskLED_BreathDowncntFlag		(0x1<<5)
#define mskLED_BreathColorSwitchFlag	(0x1<<6)
#define mskLED_ReactionFlag				(0x1<<7)
#define mskLED_HeartBeatFlag			(0x1<<8)	
#define mskLED_SyncEffectStartFlag		(0x1<<9)	
#define mskLED_SyncEffectInitialFlag	(0x1<<10)	

#endif //__LED_CONST_H__



