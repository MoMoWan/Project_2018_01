
/*_____ I N C L U D E S ____________________________________________________*/
#include "SN32F240B.h"
#include "led_driver.h"
#include "CT16B1.h"
//#include "LED_RamSetting.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/


/*****************************************************************************
* Function			: CT16B1_Init
* Description		: PWM Init
* Input				: None
* Output			: None
* Return			: None
* Note				: None
******************************************************************************/
void CT16B1_Init(void)
{
	//Enable P_CLOCK for CT16B1.
	SN_SYS1->AHBCLKEN_b.CT16B1CLKEN = 1;				//使能CT16B1时钟
	//ClOCK 48M  
	SN_CT16B1->PRE = 47;								//前置分频 (47+1)
	
	SN_CT16B1->MR24 = 255;								//255us 
	
	//Use 18 PWM Output
	//Set MR value for 0% PWM duty for Init 
	SN_CT16B1->MR0 =  0;	
	SN_CT16B1->MR1 =  0;	
	SN_CT16B1->MR2 =  0;	
	SN_CT16B1->MR3 =  0;
	SN_CT16B1->MR4 =  0;
	SN_CT16B1->MR5 =  0;
	SN_CT16B1->MR14 = 0;
	SN_CT16B1->MR15 = 0;
	SN_CT16B1->MR16 = 0;
	SN_CT16B1->MR17 = 0;
	SN_CT16B1->MR18 = 0;
	SN_CT16B1->MR10 = 0;
	SN_CT16B1->MR11 = 0;
	SN_CT16B1->MR19 = 0;
	SN_CT16B1->MR20 = 0;
	SN_CT16B1->MR21 = 0;
	SN_CT16B1->MR22 = 0;
	SN_CT16B1->MR23 = 0;	

	// Enable PWM function, IOs and select the PWM modes
	SN_CT16B1->PWMIOENB = 0xFFCC3F;							//CT16B1_PWMn pin act as match output, and output signal depends on PWMnEN bit.	
	SN_CT16B1->PWMENB = 0xFFCC3F;							//PWM mode is enabled for CT16B1_PWMn
	
	//For NPN Trans
//	SN_CT16B1->PWMCTRL_b.PWM3MODE = 1;					//PWM Mode 2
//	SN_CT16B1->PWMCTRL_b.PWM4MODE = 1;					//PWM Mode 2
//	SN_CT16B1->PWMCTRL_b.PWM5MODE = 1;					//PWM Mode 2
	
//	SN_CT16B1->MCTRL3_b.MR24IE = 1;						//使能MR24中断
	SN_CT16B1->MCTRL3_b.MR24RST = 1;					//Reset TC
//	SN_CT16B1->MCTRL3_b.MR24STOP = 1;					//EN STOP	

	SN_CT16B1->TMRCTRL_b.CRST = 1;						//Reset CT16B1
	while(SN_CT16B1->TMRCTRL_b.CRST == 1);
	SN_CT16B1->TMRCTRL_b.CEN = 1;						//ENABLE CT16B1
	
	NVIC_ClearPendingIRQ(CT16B1_IRQn);
	//NVIC_SetPriority(CT16B1_IRQn,1);					//设置优先级，低于USB中断
	NVIC_EnableIRQ(CT16B1_IRQn);						//使能CT16B1中断
}

/*****************************************************************************
* Function			: CT16B1_IRQHandler
* Description		: CT16B1_IRQHandler
* Input				: None
* Output			: None
* Return			: None
* Note				: None
******************************************************************************/
__irq void CT16B1_IRQHandler(void)
{
	SN_CT16B1->IC =0x0ffffffff;		

//	SN_CT16B1->TC = 0;									//TC清零

	//RollerLED
//	SN_CT16B1->MR3 = bReload_MR[3];
//	SN_CT16B1->MR4 = bReload_MR[4];
//	SN_CT16B1->MR5 = bReload_MR[5];
//	//DPI LED
//	SN_CT16B1->MR0 = bReload_MR[0];
//	SN_CT16B1->MR1 = bReload_MR[1];
//	SN_CT16B1->MR2 = bReload_MR[2];
//	//Logo LED
//	SN_CT16B1->MR21 = bReload_MR[6];
//	SN_CT16B1->MR22 = bReload_MR[7];
//	SN_CT16B1->MR23 = bReload_MR[8];	
//	//LED1
//	SN_CT16B1->MR11 = bReload_MR[9];
//	SN_CT16B1->MR19 = bReload_MR[10];
//	SN_CT16B1->MR20 = bReload_MR[11];
//	//LED2
//	SN_CT16B1->MR14 = bReload_MR[12];
//	SN_CT16B1->MR15 = bReload_MR[13];
//	SN_CT16B1->MR16 = bReload_MR[14];
//	//LED3
//	SN_CT16B1->MR17 = bReload_MR[15];
//	SN_CT16B1->MR18 = bReload_MR[16];
//	SN_CT16B1->MR10 = bReload_MR[17];
	//RollerLED
//	SN_CT16B1->MR3 = ledPwmData[0].r;
//	SN_CT16B1->MR4 = ledPwmData[0].g;
//	SN_CT16B1->MR5 = ledPwmData[0].b;
//	//DPI LED
//	SN_CT16B1->MR0 = ledPwmData[1].r;
//	SN_CT16B1->MR1 = ledPwmData[1].g;
//	SN_CT16B1->MR2 = ledPwmData[1].b;
//	//Logo LED
//	SN_CT16B1->MR21 = ledPwmData[2].r;
//	SN_CT16B1->MR22 = ledPwmData[2].g;
//	SN_CT16B1->MR23 = ledPwmData[2].b;	
//	//LED1
//	SN_CT16B1->MR11 = ledPwmData[3].r;
//	SN_CT16B1->MR19 = ledPwmData[3].g;
//	SN_CT16B1->MR20 = ledPwmData[3].b;
//	//LED2
//	SN_CT16B1->MR14 = ledPwmData[4].r;
//	SN_CT16B1->MR15 = ledPwmData[4].g;
//	SN_CT16B1->MR16 = ledPwmData[4].b;
//	//LED3
//	SN_CT16B1->MR17 = ledPwmData[5].r;
//	SN_CT16B1->MR18 = ledPwmData[5].g;
//	SN_CT16B1->MR10 = ledPwmData[5].b;	
//	SN_CT16B1->TMRCTRL_b.CEN = 1;						//开始TC计数	
}

/*****************************************************************************
* Function			: CT16B1_Disable
* Description		: CT16B1_Disable
* Input				: None
* Output			: None
* Return			: None
* Note				: None
******************************************************************************/
void CT16B1_Disable(void)
{
	SN_CT16B1->TMRCTRL_b.CEN=0;							//stop tc count
	SN_CT16B1->IC =0x0ffffffff;							//清初中断标志
}


