/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 * Name:    usbepfunc.c
 * Purpose: USB Custom User Module
 * Version: V1.00
 * Date:		2017/07
 *------------------------------------------------------------------------------*/
#include	<SN32F240B.h>
#include	"type.h"
#include	"Utility.h"
#include	"usbhw.h"
#include	"usbram.h"
#include	"usbdesc.h"
#include	"usbuser.h"
#include	"usbepfunc.h"
#include	"hid.h"
#include	"hiduser.h"
//#include	"Mouse_RamSetting.h"
//#include	"Mouse_Const.h"
#include "avago_api.h"
#include "variables.h"
#include "mouse.h"

/*****************************************************************************
* Function			: USB_EP1Function
* Description		: USB_EP1Function
* Input				: None
* Output			: None
* Return			: None
* Note				: None
******************************************************************************/
uint32_t USB_EP1Function(void)
{
	volatile	uint32_t	*pUsbReg;
//	uint32_t	i;
	uint32_t	wEPn_Address;
	
	pUsbReg = (&SN_USB->EP0CTL) + USB_EP1;
	
	wEPn_Address = SN_USB->EP1BUFOS; 	//get EPn offset 
	
	if (!((*pUsbReg) & mskEPn_ENDP_EN))
			return	EPn_RETURN_DISABLE;
				
	
	if ((*pUsbReg & (mskEPn_ENDP_EN|mskEPn_ENDP_STATE)) == (mskEPn_ENDP_EN|mskEPn_ENDP_STATE_NAK))
	{
		if (wUSB_EndpHalt[USB_EP1] == USB_EPn_NON_HALT)
		{
			//;=================================//
			//;			   Mouse
			//;=================================//		
			//if(wKB_SendStatus & mskKB_SendMouseFlag)
      if(isEpBusy(1))
			{
//				wKB_SendStatus &= ~mskKB_SendMouseFlag;
				freeEp(1);
				//b_aryKB_Mouse[0]: Mouse Key
				//b_aryKB_Mouse[1]: Mouse X_L
				//b_aryKB_Mouse[2]: Mouse X_H
				//b_aryKB_Mouse[3]: Mouse Y_L
				//b_aryKB_Mouse[4]: Mouse X_H
				//b_aryKB_Mouse[5]: Mouse Wheel
				//b_aryKB_Mouse[6]: Mouse Tilt Wheel   Not Use
	      //b_aryKB_Mouse[6] = sensorIntegrity;
				fnUSBMAIN_WriteFIFO( wEPn_Address , (( Hid_Ep1_Report_In[3]<<24)|(Hid_Ep1_Report_In[2]<<16) |Hid_Ep1_Report_In[1]<<8) | Hid_Ep1_Report_In[0]);				
				fnUSBMAIN_WriteFIFO( wEPn_Address + 4 , ((Hid_Ep1_Report_In[7]<<24)|Hid_Ep1_Report_In[6]<<16|(Hid_Ep1_Report_In[5]<<8) | Hid_Ep1_Report_In[4]  ));

//				if(sHID_Data.wHID_Protocol == BOOT_MODE)
//				{}
				//** clear X/Y data
//				for(i=1; i<7; i++)
//				{
//					b_aryKB_Mouse[i] = 0;	
//				}				
				
				USB_EPnAck(USB_EP1,8);				//** ACK hwByteCnt byte	
				return EPn_RETURN_ACK_OK;	
			}
			else
			{
				return EPn_RETURN_ACK_BUSY;
			}			
		}
		else//Halt = 1, return STALL
		{
			USB_EPnStall(USB_EP1);					// EPn STALL
			return EPn_RETURN_STALL;
		}
	}
	else
	{
		return EPn_RETURN_ACK_BUSY;
	}
}

/*****************************************************************************
* Function			: USB_EP2Function
* Description		: USB_EP2Function
* Input				: None
* Output			: None
* Return			: None
* Note				: None
******************************************************************************/
uint32_t USB_EP2Function(void)
{
	volatile	uint32_t	*pUsbReg;
//	uint32_t	wEPn_Address;
	
	pUsbReg = (&SN_USB->EP0CTL) + USB_EP2;
	
//	wEPn_Address = SN_USB->EP2BUFOS; 	//get EPn offset 
	
	if (!((*pUsbReg) & mskEPn_ENDP_EN))
			return	EPn_RETURN_DISABLE;
				
	
	if ((*pUsbReg & (mskEPn_ENDP_EN|mskEPn_ENDP_STATE)) == (mskEPn_ENDP_EN|mskEPn_ENDP_STATE_NAK))
	{
		if (wUSB_EndpHalt[USB_EP2] == USB_EPn_NON_HALT)
		{
			//;===============================================================================
			//;								Consumer Key
			//;===============================================================================				
			//if(wKB_SendStatus & mskKB_SendConsumerFlag)
       if(isEpBusy(2))
			{
//				wKB_SendStatus &= ~mskKB_SendConsumerFlag;
//				
//				fnUSBMAIN_WriteFIFO( wEPn_Address , ((b_aryKB_Consumer[1]<<16) | (b_aryKB_Consumer[0]<<8) | 0x01));	
        freeEp(2);
				USB_EPnAck(USB_EP2,3);	// ACK hwByteCnt byte
			}
			
			//;===============================================================================
			//;								System Key
			//;===============================================================================
//			if(wKB_SendStatus & mskKB_SendSystemFlag)
//			{
//				wKB_SendStatus &= ~mskKB_SendSystemFlag;
//				fnUSBMAIN_WriteFIFO( wEPn_Address , ((bKB_System<<8) | 0x02));	
//				USB_EPnAck(USB_EP2,2);	// ACK hwByteCnt byte			
//			}

//			//;===============================================================================
//			//;								Standard Key
//			//;===============================================================================
//			if(wKB_SendStatus & mskKB_SendKBFlag)
//			{
//				wKB_SendStatus &= ~mskKB_SendKBFlag;
//				
//				fnUSBMAIN_WriteFIFO( wEPn_Address , ((b_aryKB_UsageKey_Standard[1]<<24) | (0x00<<16) | (b_aryKB_UsageKey_Standard[0]<<8) | 0x03));	
//				fnUSBMAIN_WriteFIFO( wEPn_Address + 4, ((b_aryKB_UsageKey_Standard[5]<<24) | (b_aryKB_UsageKey_Standard[4]<<16) | (b_aryKB_UsageKey_Standard[3]<<8) | (b_aryKB_UsageKey_Standard[2])));
//				USB_EPnAck(USB_EP2,8);	// ACK hwByteCnt byte			
//			}
//			
//			//;===============================================================================
//			//;								Vendor Key
//			//;===============================================================================
//			if(wKB_SendStatus & mskKB_SendVendorKeyFlag)
//			{
//				wKB_SendStatus &= ~mskKB_SendVendorKeyFlag;
//				
//				fnUSBMAIN_WriteFIFO( wEPn_Address , ( (0x00<<24)|(b_aryKB_VendorKey[1]<<16) | (b_aryKB_VendorKey[0]<<8) | 0x05));	
//				USB_EPnAck(USB_EP2,4);	// ACK hwByteCnt byte				
//			}	
			
			//;===============================================================================
			//;								ExtraFunction Key
			//;===============================================================================
//			if(wKB_SendStatus & mskKB_ExtraFuntionKeyFlag)
//			{
//				wKB_SendStatus &= ~mskKB_ExtraFuntionKeyFlag;
//				
//				fnUSBMAIN_WriteFIFO( wEPn_Address , ((b_aryKB_ExtraFunctionKey[1]<<16) | (b_aryKB_ExtraFunctionKey[0]<<8) | 0x05));	
//				USB_EPnAck(USB_EP2,4);	// ACK hwByteCnt byte				
//			}	

			return EPn_RETURN_ACK_OK;
		}
		else//Halt = 1, return STALL
		{
			USB_EPnStall(USB_EP2);					// EPn STALL
			return EPn_RETURN_STALL;
		}
	}
	else
	{
		return EPn_RETURN_ACK_BUSY;
	}
}
//  uint32_t	wLen;
/*****************************************************************************
* Function		: USB_EPnINFunction
* Description	: SET EP1~EP4 IN token RAM
* Input				: 1. wEPNum: EP1~EP4
*								2. pData: transaction Data buffer
*								3. wBytecnt: Byte conuter Number of transaction
* Output			: None
* Return			: by EPn_RETURN_VALUE
* Note				: None
*****************************************************************************/
uint32_t	USB_EPnINFunction(uint32_t	wEPNum, uint32_t *pData, uint32_t	wBytecnt)
{
	volatile	uint32_t	*pUsbReg;
	uint32_t	i;
  uint32_t	wLen;
//	#if (USB_LIBRARY_TYPE == USB_MOUSE_TYPE)
	uint32_t	wLoop;
//	#endif
	pUsbReg = (&SN_USB->EP0CTL) + wEPNum;
	
	if (!((*pUsbReg) & mskEPn_ENDP_EN))
			return	EPn_RETURN_DISABLE;
	
	// Only support EP1~EP4 and Byte counter < 64
	if ((wEPNum == USB_EP0) || (wEPNum > USB_EP4) || wBytecnt > 64)
		return EPn_RETURN_NOT_SUPPORT;			
	
	if ((*pUsbReg & (mskEPn_ENDP_EN|mskEPn_ENDP_STATE)) == (mskEPn_ENDP_EN|mskEPn_ENDP_STATE_NAK))
	{
		if (wUSB_EndpHalt[wEPNum] == USB_EPn_NON_HALT)
		{
			pUsbReg = &wUSB_EPnPacketsize[wEPNum];
			if (wBytecnt > *pUsbReg)
				return EPn_RETURN_OVER_MAX_SIZE;		//wBytecnt > EPn packet size

			//set EPn FIFO offset value to i
			i = wUSB_EPnOffset[wEPNum-1];
//			#if (USB_LIBRARY_TYPE == USB_MOUSE_TYPE)
      wLen = ((wBytecnt % 4) == 0) ? (wBytecnt>>2) : ((wBytecnt>>2) +1);
      for (wLoop=0; wLoop<wLen; wLoop++)
//			for (wLoop=0; wLoop<(wBytecnt>>2); wLoop++)
			{
				fnUSBMAIN_WriteFIFO( i+(wLoop*4), *(pData+(wLoop*1)));
			}
//			#else
//			fnUSBMAIN_WriteFIFO( i , ((*pData<<8)&0xFFFFFF00)|0x01 );	//** Byte 0 = Report ID
//			fnUSBMAIN_WriteFIFO( i+4 , (*pData>>24)&0x000000FF );	//** Byte 1 ~ 4 = Mouse data
//			#endif
//			USB_Test();				//return Mouse data
			USB_EPnAck(wEPNum,wBytecnt);	// ACK hwByteCnt byte
      freeEp(wEPNum);
			return EPn_RETURN_ACK_OK;
		}
		else//Halt = 1, return STALL
		{
			USB_EPnStall(wEPNum);					// EPn STALL
			return EPn_RETURN_STALL;
		}
	}
	else
	{
		return EPn_RETURN_ACK_BUSY;
	}
}

