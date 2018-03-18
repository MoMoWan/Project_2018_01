
#ifndef __TIMER_H__
#define __TIMER_H__

/*****************************************************************************
* Description	: 16-BIT TIMER0
*****************************************************************************/
#define mskCT16Bn_CEN 	(0x01<<0)					//timer counter enable
#define mskCT16Bn_CRST	(0x01<<1)					//timer counter reset	

extern void Timer_Init(void);
extern void Tmr_initialiation(void);

#endif //__TIMER_H__


