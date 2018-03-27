/***********************************************************************/
/*  This file is part of the ARM Toolchain package                     */
/*  Copyright (c) 2010 Keil - An ARM Company. All rights reserved.     */
/***********************************************************************/
/*                                                                     */
/*  Powermanagement.C:  power management                   */
/*                                           		   */
/*                                                                     */
/***********************************************************************/
#include <SN32F240B.h>
#include "CT16.h"
#include "CT16B0.h"
#include "CT16B1.h"
#include	"main.h"
#include	"usbram.h"
#include	"usbhw.h"
#include	"mouse.h"
#include	"SPI.h"
#include	"Utility.h"
#include	"avago_api.h"
#include	"powermanagement.h"
#include	"gpio.h"

void	MS_SuspendSetting(void);
/*****************************************************************************
* Function			: MN_BootState
* Description		: MN_BootState
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
//void MN_BootState(void)
//{
//	//Given a init Random value , by the set configuration time
//	LED_EffectRandomValueGen();
//	
//	//** Check BusSuspend
//	if (sUSB_EumeData.wUSB_Status & mskBUSSUSPEND)	
//	{
//		MS_SuspendSetting();					//进入挂起前的设置
//		USB_Suspend();
//		
//		//**(bSetconfiguration=1) --> Leave BOOT STATE
//		if(sUSB_EumeData.wUSB_SetConfiguration & BIT_0)
//		{
//			//** Timer Initial	
//			CT16B1_Init();
//			MS_Init();	
//			bMN_State = S_MAIN_IDLE_STATE;
//		}
//	}
//	//**(bSetconfiguration=1) --> Leave BOOT STATE
//	else if(sUSB_EumeData.wUSB_SetConfiguration & BIT_0)
//	{
//		CT16B1_Init();	
//		MS_Init();	
//		bMN_State = S_MAIN_IDLE_STATE;
//	}
//}

void USB_checkStatus(void)
/*++

Function Description:
  This function processes the suspend and resume of USB support.

Arguments:
  NONE

Returns:
  NONE

--*/
{  
	//** Check BusSuspend
	if (sUSB_EumeData.wUSB_Status & mskBUSSUSPEND)	
	{
		MS_SuspendSetting();					//进入挂起前的设置
		USB_Suspend();
		
		//**(bSetconfiguration=1) --> Leave BOOT STATE
		//if(sUSB_EumeData.wUSB_SetConfiguration & BIT_0)
//    if (sUSB_EumeData.wUSB_SetConfiguration == 1)
		{
			//** Timer Initial
      gpio_initialization();       
      CT16B0_Init();
			CT16B1_Init();
			initializeMouseVarialbes();
//[  scroll interrupt setting 
//  SN_GPIO2->IS = 0x18;
//	SN_GPIO2->IEV = 0x18;
//	SN_GPIO2->IC =0xFFFFFFFF;
//  SN_GPIO2->IE = 0x18;
//	NVIC_ClearPendingIRQ(P2_IRQn);
//	NVIC_EnableIRQ(P2_IRQn);  
//]      
//      SN_WDT->CFG |= 0x5AFA0001; 			//Enable      
		}
	}
	//**(bSetconfiguration=1) --> Leave BOOT STATE
//	else if(sUSB_EumeData.wUSB_SetConfiguration & BIT_0)
//	{
//		CT16B1_Init();	
//		initializeMouseVarialbes();	
//    SN_WDT->CFG |= 0x5AFA0001; 			//Enable
//	}  
 // sUSB_EumeData.wUSB_Status &= ~mskBUSSUSPEND;
}

/*****************************************************************************
* Function		: MS_SuspendSetting
* Description	: Setting Timer Disable, WDT Disable.
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void MS_SuspendSetting(void)
{
//	MS_SetRowInputMode();
//	MS_SetRowPullUp();
//	UT_MAIN_DelayNx10us(2);	
//	//** Setting Wheel status as HH or LL, to avoid current leakage.
//	MS_SetWheelIOSuspend();
 //[ 
// 	if (SN_GPIO2->DATA_b.DATA2)
//	{
//		//** Setting Wheel as input pullup to avoid current leakage.
//    SN_GPIO2->MODE_b.MODE2 = 0;
//    SN_GPIO2->CFG_b.CFG2 = 0;
//	}
//	else
//	{
//		//** Setting Wheel as output L to avoid current leakage.
//    SN_GPIO2->MODE_b.MODE2 = 1;
//	  SN_GPIO2->DATA_b.DATA2 = 0;
//	}
//	
//	
//	if (SN_GPIO2->DATA_b.DATA3)
//	{
//		//** Setting Wheel as input pullup to avoid current leakage.
//    SN_GPIO2->MODE_b.MODE3 = 0;
//    SN_GPIO2->CFG_b.CFG3 = 0;
//	}
//	else
//	{
//		//** Setting Wheel as input floating to avoid current leakage. (wheel short IO to GND)	
//    SN_GPIO2->MODE_b.MODE3 = 1;
//	  SN_GPIO2->DATA_b.DATA3 = 0;		
//	} 
  //]
  
	SN_GPIO3->MODE_b.MODE11 = 1;			
	SN_GPIO3->DATA_b.DATA11 = 1;
	//EN1
	SN_GPIO3->MODE_b.MODE10 = 1;			
	SN_GPIO3->DATA_b.DATA10 = 1;
	//EN2
	SN_GPIO3->MODE_b.MODE9 = 1;			
	SN_GPIO3->DATA_b.DATA9 = 1;
	//EN3
	SN_GPIO3->MODE_b.MODE8 = 1;			
	SN_GPIO3->DATA_b.DATA8 = 1; 
	
	SPI_W_2BYTE(REG_SHUTDOWN, SENSOR_POWER_DOWN);
	UT_MAIN_DelayNms(50);
	
	__SPI0_SET_SEL0	
	//Disable timer
	SN_CT16B0->TMRCTRL &= ~0x01;
	SN_CT16B1->TMRCTRL &= ~0x01;	
	
	//Disable RGB LED
	SN_CT16B1->PWMCTRL = 0xAAAAAAAA;//0xFFFFFFFF;
	SN_CT16B1->PWMCTRL2 = 0xAAAAAAAA;//0xFFFFFFFF;
  UT_MAIN_DelayNms(50);
  
	//Disable WDT
	SN_WDT->CFG = 0x5AFA0000;
}

/*****************************************************************************
* Function			: MS_Init
* Description		: MS_Init
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
//void MS_Init(void)
//{
//	MS_WheelInit();				// Scroll Wheel Initialization 
//	MS_SensorInit();			// Sensor Initialization
//	MS_TimerInit();				// Timer Initialization
//	
//	SN_WDT->CFG |= 0x5AFA0001; 			//Enable
//}

/*****************************************************************************
* Function			: MS_WheelInit
* Description		: MS_WheelInit
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
//void MS_WheelInit(void)
//{
//	uint8_t bWheelStatus = 0;
//	
//	MS_SetWheelIOInputMode();
//	MS_SetWheelIOPullUp();	
//	//Delay 40us to make sure pull-up ok
//	UT_MAIN_DelayNx10us(4);	
//	
//	if (MS_WHEEL_PM_0->DATA & mskMS_WHEEL_IO_PM_A)
//	{
//		bWheelStatus |= mskWHEEL_A_BIT0; 
//	}
//	if (MS_WHEEL_PM_0->DATA & mskMS_WHEEL_IO_PM_B)
//	{
//		bWheelStatus |= mskWHEEL_B_BIT1; 
//	}
//	bWHEEL_PreviousStatus	= bWheelStatus;
//	//** Wheel Initial status
//	bWHEEL_DebounceStatus |= WHEEL_NEW_DEBOUNCE_VALUE;	
//}

/*****************************************************************************
* Function			: MS_SensorInit
* Description		: MS_SensorInit
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
//void MS_SensorInit(void)
//{  
////  U8 dpiData=0,dpiDataTemp = 0;
//	//Reset Sensor SPI.
//	__SPI0_CLR_SEL0
//	__UT_MAIN_tSRAD
//	__SPI0_SET_SEL0
//  
////	flash_buttfer[0] = bMSDPI_Index = *((uint8_t *)MANUFACTURING_SATRT);
////  bMSDPI_Index = user.profile[0].Sensor.dpiStage;
//#ifdef	PMW3325
//	
//	SPI_W_2BYTE(SENSOR_RESET, SETTING_POWER_UP_RESET);
////	__UT_MAIN_tSWW;	
////  SPI_W_2BYTE(SENSOR_RESOLUTION_3325, (user.profile[0].activeDpi.x/40));   
////  SPI_W_2BYTE(SENSOR_RESOLUTION_3325, tabSensor_DPI[bMSDPI_Index]);
////	SPI_W_2BYTE(SENSOR_RESOLUTION_3325, SENSOR_RESOLUTION_1600_3325);
//	__UT_MAIN_tSWW;	
//	SPI_W_2BYTE(0x18, 0x39);				//Must Write 0x39 To 0x18
//	UT_MAIN_DelayNms(50);	
//	b_arySPI_Rx_Fifo[0] = SPI_R_BYTE(0x02);
//  __UT_MAIN_tSRR;	
//  b_arySPI_Rx_Fifo[0] = SPI_R_BYTE(0x03);
//  __UT_MAIN_tSRR;	
//  b_arySPI_Rx_Fifo[0] = SPI_R_BYTE(0x04);
//  __UT_MAIN_tSRR;	
//  b_arySPI_Rx_Fifo[0] = SPI_R_BYTE(0x05);
//  __UT_MAIN_tSRR;	
//  b_arySPI_Rx_Fifo[0] = SPI_R_BYTE(0x06);
//  __UT_MAIN_tSRR;	
//  SPI_W_2BYTE(0x78, 0x80);
//  __UT_MAIN_tSWW;
//  SPI_W_2BYTE(0x79, 0x80);
//  __UT_MAIN_tSWW;
//  SPI_W_2BYTE(0x14, 0x80);
//  __UT_MAIN_tSWW;
//  SPI_W_2BYTE(0x20, 0x40);
//  __UT_MAIN_tSWW;
//  SPI_W_2BYTE(0x1A, 0x40);
//  __UT_MAIN_tSWW; 
//  SPI_W_2BYTE(0x47, 0x00);
//  __UT_MAIN_tSWW;
//  SPI_W_2BYTE(0x48, 0x01);
//  __UT_MAIN_tSWW;
//  SPI_W_2BYTE(0x60, 0x01);
//  __UT_MAIN_tSWW;
//  SPI_W_2BYTE(0x69, 0x03);
//  __UT_MAIN_tSWW;
//  SPI_W_2BYTE(0x1D, 0x90);
//  __UT_MAIN_tSWW; 
//  SPI_W_2BYTE(0x1B, 0x2E);
//  __UT_MAIN_tSWW;
//  SPI_W_2BYTE(0x24, 0x05);
//  __UT_MAIN_tSWW;
//  SPI_W_2BYTE(0x56, 0x00);
//  __UT_MAIN_tSWW;
//  SPI_W_2BYTE(0x2C, 0x8A);
//  __UT_MAIN_tSWW;
//  SPI_W_2BYTE(0x2D, 0x58);
//  __UT_MAIN_tSWW; 
//  SPI_W_2BYTE(0x40, 0x80);
//  __UT_MAIN_tSWW;
//  SPI_W_2BYTE(0x7F, 0x01);
//  __UT_MAIN_tSWW;
//  SPI_W_2BYTE(0x7A, 0x32);
//  __UT_MAIN_tSWW;
//  SPI_W_2BYTE(0x6A, 0x93);
//  __UT_MAIN_tSWW;
//  SPI_W_2BYTE(0x6B, 0x68);
//  __UT_MAIN_tSWW; 
//  SPI_W_2BYTE(0x6C, 0x71);
//  __UT_MAIN_tSWW;
//  SPI_W_2BYTE(0x6D, 0x50);
//  __UT_MAIN_tSWW;
//  SPI_W_2BYTE(0x7F, 0x00);
//  __UT_MAIN_tSWW;
//  SPI_W_2BYTE(0x7F, 0x02);
//  __UT_MAIN_tSWW;
//  SPI_W_2BYTE(0x29, 0x1C);
//  __UT_MAIN_tSWW; 
//  SPI_W_2BYTE(0x2A, 0x1A);
//  __UT_MAIN_tSWW;
//  SPI_W_2BYTE(0x2B, 0x90);
//  __UT_MAIN_tSWW;
//  SPI_W_2BYTE(0x40, 0x01);
//  __UT_MAIN_tSWW;
//  SPI_W_2BYTE(0x7F, 0x00);
//  __UT_MAIN_tSWW;
//// dpiDataTemp = (user.profile[0].activeDpi.x -200)/100; 
//// SPI_W_2BYTE(SENSOR_RESOLUTION_3325, dpiTable[dpiDataTemp]);
//  SPI_W_2BYTE(SENSOR_RESOLUTION_3325, dpiTable[(user.profile[0].activeDpi.x -200)/100]); 
//  __UT_MAIN_tSWW;  
//  SPI_W_2BYTE(SENSOR_ANGLESNAP_3325, user.profile[0].Sensor.angleSnap > 0 ? 0x84:0x04); // setting the angle snap   
//#else			//3360

//	SPI_W_2BYTE(SENSOR_RESET, SETTING_POWER_UP_RESET);	
//	UT_MAIN_DelayNms(50);	
//	
//	__UT_MAIN_tSWW;	
//	SPI_W_2BYTE(SENSOR_CONTROL_3360, DEGREE_0_3360);
//	
//	__UT_MAIN_tSWW;	
//	SPI_W_2BYTE(SENSOR_CONFIG1_3360, SENSOR_CPI_5000_3360);
//	
////	__UT_MAIN_tSWW;	
////	SPI_W_2BYTE(SENSOR_CONFIG2_3360, REST_DISENABLE);
//	
//	__UT_MAIN_tSWW;	
//	SPI_W_2BYTE(SENSOR_CONFIG5_3360, SENSOR_CPI_5000_3360);	
//	
//#endif

//	__UT_MAIN_tSWW;		

//	//MS_SensorDPISetting_by_SPI();
//}

/*****************************************************************************
* Function			: MS_SensorDPISetting_by_SPI
* Description		: MS_SensorDPISetting_by_SPI
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
//void MS_SensorDPISetting_by_SPI(void)
//{
////	if(++bMSDPI_Index>4)
////		bMSDPI_Index = 0;
//	if(++user.profile[0].Sensor.dpiStage > (user.profile[0].Sensor.maxStages -1)) {
//    user.profile[0].Sensor.dpiStage = 0;
//  }
//  user.profile[0].activeDpi.x = user.profile[0].activeDpi.y = user.profile[0].Sensor.stageDPI[user.profile[0].Sensor.dpiStage].x;
//#ifdef PMW3325
////	SPI_W_2BYTE(SENSOR_RESOLUTION_3325, tabSensor_DPI[bMSDPI_Index]);
////  SPI_W_2BYTE(SENSOR_RESOLUTION_3325, (user.profile[0].Sensor.stageDPI[user.profile[0].Sensor.dpiStage].x/40));
//  SPI_W_2BYTE(SENSOR_RESOLUTION_3325, dpiTable[(user.profile[0].activeDpi.x -200)/100]); 
//#else
////	SPI_W_2BYTE(SENSOR_CONFIG1_3360, tabSensor_DPI[bMSDPI_Index]);
//  SPI_W_2BYTE(SENSOR_CONFIG1_3360, (user.profile[0].Sensor.stageDPI[user.profile[0].Sensor.dpiStage].x/40));
//#endif	

//}

/*****************************************************************************
* Function		: N-Fire Page
* Description	: 
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
//void MS_NFirePage(uint8_t bKeyCode, uint8_t bKeyEvent)
//{
//	if(bKeyEvent == KB_MAKE_EVENT)
//	{
//		if(!(wMS_Status & mskMS_NFireKeyAction))		//** Check any NFire key action
//		{
//			wMS_Status |= mskMS_NFireKeyAction;		//** Enable N-Fire key action
//			//** Action Counter of N Fire Key: BYTE[1]
//			bMS_NFireOperationCounter =  3;	//3连击
//			//** MS Key Code of N-Fire Key: BYTE[2]
//			bMS_NFireKeyCode=  bKeyCode;		//** Set combo key index
//			//** Setting Toggle Bit0 = 1 for switching N Fire Key Make/Break event. 
//			bMS_NFireKeyToggleBit = BIT_0;
//		}
//	}		
//}

/*****************************************************************************
* Function			: MS_TimerInit
* Description		: MS_TimerInit
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
//void MS_TimerInit(void)
//{
//	Timer_Init();
//}

/*****************************************************************************
* Function		: KB_SetRowPullUp
* Description	: Set Row pull up
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
//void MS_SetRowPullUp(void)
//{
//	__MS_IO_0_SETPUR
//	__MS_IO_1_SETPUR
//	__MS_IO_2_SETPUR
//	__MS_IO_3_SETPUR
//	__MS_IO_4_SETPUR
//	__MS_IO_5_SETPUR
//	__MS_IO_6_SETPUR
////	__MS_IO_7_SETPUR
//}

/*****************************************************************************
* Function		: MS_SetWheelIOPullUp
* Description	: Set Wheel IO as Pull up
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
//void MS_SetWheelIOPullUp(void)
//{
//	__MS_WHEEL_IO_A_SETPUR
//	__MS_WHEEL_IO_B_SETPUR
//}

/*****************************************************************************
* Function		: MS_SetRowInputMode
* Description	: Set MS button GPIO as Input Mode
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
//void MS_SetRowInputMode(void)
//{
//	__MS_IO_0_INPUTM
//	__MS_IO_1_INPUTM
//	__MS_IO_2_INPUTM
//	__MS_IO_3_INPUTM
//	__MS_IO_4_INPUTM
//	__MS_IO_5_INPUTM
//	__MS_IO_6_INPUTM
////	__MS_IO_7_INPUTM	
//	
//}

/*****************************************************************************
* Function		: MS_SetWheelIOInputMode
* Description	: Set MS Wheel GPIO as Input Mode
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
//void MS_SetWheelIOInputMode(void)
//{
//	__MS_WHEEL_IO_A_INPUTM
//	__MS_WHEEL_IO_B_INPUTM
//}

/*****************************************************************************
* Function		: MS_SetWheelIOSuspend
* Description	: Set Wheel IO as Pull up
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
//void MS_SetWheelIOSuspend(void)
//{
//	//** Read MS IO status
//	bWHEEL_WheelStatus &= ~mskWheel_CurrentStatus;		
//	if (MS_WHEEL_PM_0->DATA & mskMS_WHEEL_IO_PM_A)
//	{
//		//** Setting Wheel as input pullup to avoid current leakage.
//		__MS_WHEEL_IO_A_INPUTM
//		__MS_WHEEL_IO_A_SETPUR
//	}
//	else
//	{
//		//** Setting Wheel as output L to avoid current leakage.
//		__MS_WHEEL_IO_A_OUTPUTM
//		__MS_WHEEL_IO_A_OUTPUTL		
//	}
//	
//	
//	if (MS_WHEEL_PM_0->DATA & mskMS_WHEEL_IO_PM_B)
//	{
//		//** Setting Wheel as input pullup to avoid current leakage.
//		__MS_WHEEL_IO_B_INPUTM
//		__MS_WHEEL_IO_B_SETPUR	
//	}
//	else
//	{
//		//** Setting Wheel as input floating to avoid current leakage. (wheel short IO to GND)	
//		__MS_WHEEL_IO_B_OUTPUTM
//		__MS_WHEEL_IO_B_OUTPUTL			
//	}
//}
