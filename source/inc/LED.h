
#ifndef __LED_H__
#define __LED_H__
#include <SN32F240B.h>

extern void MN_LEDState(void);
extern void LED_Init(void);

void LED_Mode_ReInit(void);

void LED_Mode_Breath_Init(void);
void LED_Mode_Breath(void);
void LED_Effect_Breath(void);

void LED_Mode_Spectrum_Init(void);
void LED_Mode_Spectrum(void);
void LED_EffectSpectrumProcess(uint8_t* nNumber_Down, uint8_t* nNumber_UP,uint8_t* nRGB_Step,uint8_t nHalforMax);

void LED_Mode_Reaction_Init(void);
void LED_Mode_Reaction(void);

void LED_Mode_Static_Init(void);

void LED_Mode_Blink_Init(void);
  
void LED_Mode_Static(uint8_t id);

void LED_Mode_Wave_Init(void);
void LED_Mode_Wave(void);
void LED_Mode_None(void);

void LED_Mode_Blink(void);

void LED_PWMBuf_Update(uint8_t rpwm_buf,uint8_t gpwm_buf,uint8_t bpwm_buf);

void LED_SettingDefaultColor(void);

void LED_ReflashPWMDuty(void);

//void decode_DPI_Stage(void);
#endif //__LED_H__
