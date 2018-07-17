
#ifndef __LED_H__
#define __LED_H__
#include <SN32F240B.h>

extern void MN_LEDState(void);
extern void LED_Init(void);

void LED_Mode_ReInit(uint8_t id);

void LED_Mode_Breath_Init(uint8_t id);
void LED_Mode_Breath(uint8_t id);
void LED_Effect_Breath(uint8_t id);

void LED_Mode_Spectrum_Init(uint8_t id);
void LED_Mode_Spectrum(uint8_t id);
void LED_EffectSpectrumProcess(uint8_t* nNumber_Down, uint8_t* nNumber_UP,uint8_t* nRGB_Step,uint8_t nHalforMax);

void LED_Mode_Reaction_Init(uint8_t id);
void LED_Mode_Reaction(uint8_t id);

void LED_Mode_Static_Init(uint8_t);

void LED_Mode_Blink_Init(uint8_t id);
  
void LED_Mode_Static(uint8_t id);

void LED_Mode_Wave_Init(void);
void LED_Mode_Wave(void);
void LED_Mode_None(void);

void LED_Mode_Blink(uint8_t id);

void LED_PWMBuf_Update(uint8_t rpwm_buf,uint8_t gpwm_buf,uint8_t bpwm_buf);

void LED_SettingDefaultColor(void);

void LED_ReflashPWMDuty(void);

//void decode_DPI_Stage(void);
#endif //__LED_H__
