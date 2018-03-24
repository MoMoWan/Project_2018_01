/******************** (C) COPYRIGHT 2017 SONiX *******************************
* COMPANY:			SONiX
* DATE:					2017/07
* AUTHOR:				SA1
* IC:						SN32F240B
* DESCRIPTION:	CT16B0 related functions.
*____________________________________________________________________________
* REVISION	Date				User		Description
* 1.0				2017/07/07	SA1			First release
*
*____________________________________________________________________________
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS TIME TO MARKET.
* SONiX SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR CONSEQUENTIAL 
* DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH SOFTWARE
* AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN 
* IN CONNECTION WITH THEIR PRODUCTS.
*****************************************************************************/

/*_____ I N C L U D E S ____________________________________________________*/
#include <SN32F240B.h>
#include "CT16.h"
#include "CT16B0.h"
#include "main.h"
#include "timer.h"
#include "button.h"
#include "Mouse.h"
#include "variables.h"
#include "wdt.h"
#include "usb_hardware.h"
#include "led_driver.h"
#include "avago_api.h"
#include "macro.h"
#include "usbram.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/


/*_____ D E F I N I T I O N S ______________________________________________*/


/*_____ M A C R O S ________________________________________________________*/


/*_____ F U N C T I O N S __________________________________________________*/
/*****************************************************************************
* Function		: CT16B0_Init
* Description	: Initialization of CT16B0 timer
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void CT16B0_Init(void)
{
	//Enable P_CLOCK for CT16B0.
	__CT16B0_ENABLE;
	
	SN_CT16B0->PRE = 47;			//SystemClock/48		

	//Set MR0 value for 250us period
	SN_CT16B0->MR0 = 1000;//250;	

	//Set MR0 match as TC RESET, and enable MR0 interrupt
	SN_CT16B0->MCTRL = (mskCT16_MR0IE_EN |mskCT16_MR0RST_EN);
	
	//Wait until timer reset done.
	SN_CT16B0->TMRCTRL = mskCT16_CRST;
	while (SN_CT16B0->TMRCTRL & mskCT16_CRST);	

	//Let TC start counting.
	SN_CT16B0->TMRCTRL |= mskCT16_CEN_EN;			
	
	NVIC_ClearPendingIRQ(CT16B0_IRQn);
	//Enable CT16B0's NVIC interrupt.	
	NVIC_EnableIRQ(CT16B0_IRQn);
	
}

/*****************************************************************************
* Function		: CT16B0_IRQHandler
* Description	: ISR of CT16B0 interrupt
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
__irq void CT16B0_IRQHandler(void)
{
  U8 i;
	//MR0
	if (SN_CT16B0->MCTRL_b.MR0IE)				//Check if MR0 IE enables?
	{
		if (SN_CT16B0->RIS_b.MR0IF)							//Check if MR0 Interrupt ?
		{
			SN_CT16B0->IC = mskCT16_MR0IC;					//Clear MR0 match interrupt status	
			
//			MN_WheelState();
			
//			__Set_Timer250usFlag
//			
//			if(++bMS_TimeFrame_CNT==4)
//			{
//				bMS_TimeFrame_CNT = 0;									//** clr timer count
//				//** Set MS 1ms timer Flag	, and it will clear at sensor state
//				__Set_Timer1msFlag
//				__Set_Timer1msFlag_Temp
//				__Set_ReportRate1msFlag	
//			}	
//			
//			if(wMS_Status & mskMS_Timer1msFlag_Temp)		//1ms
//			{
//				//clear mouse 1ms timer flag for counting timer 2/4/8ms flag
//				__Clear_Timer1msFlag_Temp
//				
//				if(++bMS_TimeFrame_2ms == 2)
//				{
//					bMS_TimeFrame_2ms = 0;	
//					__Set_Timer2msFlag
//					__Set_ReportRate2msFlag	
//				}
//				if(++bMS_TimeFrame_4ms == 4)
//				{
//					bMS_TimeFrame_4ms = 0;				
//					__Set_Timer4msFlag
//					__Set_ReportRate4msFlag				
//				}			
//				if(++bMS_TimeFrame_8ms == 8)
//				{
//					bMS_TimeFrame_8ms = 0;				
//					__Set_ReportRate8msFlag				
//				}		
//				if(++dbMS_LED_ReflashTimeFrame >= (8 + bLED_DataRefreshTime_Reload))
//				{
//					dbMS_LED_ReflashTimeFrame = 0;				
//					__Set_LEDReflashTimeFlag				
//				}	
//			}
 for (i = 0; i<5 ;i++) {
  dbMS_LED_ReflashTimeFrame[i]++;
 }
	++ Profiletime;
	++ Profiledebounce;
  SROMchecktimer ++;
  if (milliseconds != 0) {
    milliseconds--;
  }

  if (updateOneshot != 0) {
    updateOneshot--;
  }
	 if ((PIN_MID == 0) && (PIN_RGT == 0) && (PIN_LFT == 0)){
     CalibrationTimer++;
   }
	
	if(AutoCalibrationFalg == 1) {
    HaltTimer1++;
  }
	if(HaltTimer2 < 0x9FFFF){
     HaltTimer2++;
  }
	
  if (((sUSB_EumeData.wUSB_SetConfiguration & BIT_0) != 0)  && (deviceState >= VAR_INIT)) {
    basicFWTimer ++;

			ledTiming();
		if(sensorIntegrity!=0)
				sensorIntegrity--;
  }
		}
	}
		
}

/*****************************************************************************
* Function			: CT16B0_Disable
* Description		: CT16B0_Disable
* Input				: None
* Output			: None
* Return			: None
* Note				: None
******************************************************************************/
void CT16B0_Disable(void)
{
	SN_CT16B0->TMRCTRL_b.CEN=0;							//stop tc count
	SN_CT16B0->IC =0x0ffffffff;							//清初中断标志
}


/*****************************************************************************
* Function		: CT16B0_NvicEnable
* Description	: Enable CT16B0 timer interrupt
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void CT16B0_NvicEnable(void)
{
	NVIC_ClearPendingIRQ(CT16B0_IRQn);
	NVIC_EnableIRQ(CT16B0_IRQn);
	//NVIC_SetPriority(CT16B0_IRQn,0);		// Set interrupt priority (default)
}

/*****************************************************************************
* Function		: CT16B0_NvicEnable
* Description	: Disable CT16B0 timer interrupt
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void CT16B0_NvicDisable(void)
{
	NVIC_DisableIRQ(CT16B0_IRQn);
}
