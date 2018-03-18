#ifndef __SN32F240B_UTILITY_H
#define __SN32F240B_UTILITY_H

/*_____ I N C L U D E S ____________________________________________________*/
#include <SN32F240B.h>

/*_____ D E F I N I T I O N S ______________________________________________*/

//PMW3325 Spec Delay
#ifdef PMW3325
	#define __UT_MAIN_tSRAD	UT_MAIN_DelayNx10us(4);		//40us > 35us
	#define __UT_MAIN_tSWR	UT_MAIN_DelayNx10us(5);		//50us > 45us
	#define __UT_MAIN_tSWW	UT_MAIN_DelayNx10us(5);		//50us > 45us
	#define	__UT_MAIN_tSRR	UT_MAIN_DelayNx10us(2);		//20us >= 20us 
	#define __UT_MAIN_tRESET	UT_MAIN_DelayNx10us(5);		//50us > Tsww
	#define __UT_MAIN_tSRAD_MOTBR	UT_MAIN_DelayNx10us(4);		//__UT_MAIN_tSRAD
	#define __UT_MAIN_tWAKEUP	UT_MAIN_DelayNms(50);			//50ms >= 50ms
	
//PMW3360 Spec
#else			//PWM3360
	#define __UT_MAIN_tSRAD	UT_MAIN_DelayNx10us(16+4);			//160us >= 160us
	#define __UT_MAIN_tSWR	UT_MAIN_DelayNx10us(18+4);			//180us >= 180us
	#define __UT_MAIN_tSWW	UT_MAIN_DelayNx10us(18+4);			//180us >= 180us
	#define __UT_MAIN_tSRR	UT_MAIN_DelayNx10us(2+2);				//20us >= 20us	tSRW
	#define	__UT_MAIN_tSRW	UT_MAIN_DelayNx10us(2+2);				//20us >= 20us	tSRW
	#define __UT_MAIN_tRESET	UT_MAIN_DelayNx10us(18+4);		//180us >= Tsww
	#define	__UT_MAIN_tSRAD_MOTBR	UT_MAIN_DelayNx5us(8+4);		//40us > 35us

	#define __UT_MAIN_tWAKEUP	UT_MAIN_DelayNms(70);			//50ms >= 50ms
	#define __UT_MAIN_tMOTRST	UT_MAIN_DelayNms(90);			//60ms >= 50ms
#endif

/*_____ M A C R O S ________________________________________________________*/


/*_____ D E C L A R A T I O N S ____________________________________________*/
void UT_INT_DelayNx10us (uint32_t N);
void UT_INT_DelayNms (uint32_t N);
void UT_MAIN_DelayNx10us (uint32_t N);
void UT_MAIN_DelayNms (uint32_t N);

void UT_MAIN_DelayNx5us (uint32_t N);
void UT_MAIN_DelayNx1us (uint32_t N);

#endif	/*__SN32F240B_UTILITY_H*/
