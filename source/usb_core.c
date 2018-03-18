/*++

Copyright (c) 2012-2014 Razer Ltd.
All rights reserved. This program contains proprietary and confidential information.

Module Name:
  usb_core.c

Compiler:
  Keil Software µVision v5.11, with NXP Semiconductor LPC11U2x proccessor support.

Abstract:


--*/

#include "type.h"
#include "usb_core.h"
#include "main.h"
#include "usb_hardware.h"
#include "usb_descriptor.h"
#include "usb_costom.h"
#include "usb_hid.h"
//#include "usb_configure.h"


#if (USB_CLASS)

#if (USB_AUDIO)
#include "audio.h"
#include "adcuser.h"
#endif

//#if (USB_HID)
//#include "usb_hid.h"
//#include "hiduser.h"
//#endif

#if (USB_MSC)
#include "msc.h"
#include "mscuser.h"
extern MSC_CSW CSW;
#endif

#if (USB_CDC)
#include "cdc.h"
#include "cdcuser.h"
#endif

#endif

#if (USB_VENDOR)
#include "vendor.h"
#endif

#if defined ( __CC_ARM   )
#pragma diag_suppress 111,177,1441
#endif

uint16_t  USB_DeviceStatus;
uint8_t  USB_DeviceAddress;
#if __OPTIMIZE_CODE
__IO
#endif
uint8_t  USB_Configuration;
uint32_t USB_EndPointMask;
uint32_t USB_EndPointHalt;
uint8_t  USB_AltSetting[USB_IF_NUM];

uint8_t  EP0Buf[92];

#pragma diag_suppress 1441

//U8  EP0Buffer[90];

USB_EP_DATA EP0Data;

USB_SETUP_PACKET SetupPacket;

#if __OPTIMIZE_CODE
uint32_t USB_GetEPBitMsk(uint8_t EPNum);
#endif


void USB_ResetCore (void) 
/*
 *  Reset USB Core
 *    Parameters:      None
 *    Return Value:    None
 */
{

  USB_DeviceStatus  = USB_POWER;
  USB_DeviceAddress = 0;
  USB_Configuration = 0;
  USB_EndPointMask  = 0x00010001;
  USB_EndPointHalt  = 0x00000000;
}



#if __OPTIMIZE_CODE
__inline
#endif
void USB_SetupStage (void) 
/*
 *  USB Request - Setup Stage
 *    Parameters:      None (global SetupPacket)
 *    Return Value:    None
 */
{
  USB_ReadSetupEP(0x00, (uint8_t *)&SetupPacket);
}




void USB_DataInStage (void) 
/*
 *  USB Request - Data In Stage
 *    Parameters:      None (global EP0Data)
 *    Return Value:    None
 */
{
  uint32_t cnt;

  if (EP0Data.Count > USB_MAX_PACKET0) {
    cnt = USB_MAX_PACKET0;
  } else {
    cnt = EP0Data.Count;
  }
  cnt = USB_WriteEP(0x80, EP0Data.pData, cnt);
  EP0Data.pData += cnt;
  EP0Data.Count -= cnt;
}


void USB_DataOutStage (void) 
/*
 *  USB Request - Data Out Stage
 *    Parameters:      None (global EP0Data)
 *    Return Value:    None
 */
{

  uint32_t cnt;

  cnt = USB_ReadEP(0x00, EP0Data.pData);
  EP0Data.pData += cnt;
  EP0Data.Count -= cnt;
}



#if __OPTIMIZE_CODE
__inline
#endif
void USB_StatusInStage (void) 
/*
 *  USB Request - Status In Stage
 *    Parameters:      None
 *    Return Value:    None
 */
{
  USB_WriteEP(0x80, NULL, 0);
}



#if __OPTIMIZE_CODE
__inline
#endif
void USB_StatusOutStage (void)
/*
 *  USB Request - Status Out Stage
 *    Parameters:      None
 *    Return Value:    None
 */
{
  USB_ReadEP(0x00, EP0Buf);
}



#if !__REMOVE_UNUSED_REQ
uint32_t USB_ReqGetStatus (void) 
/*
 *  Get Status USB Request
 *    Parameters:      None (global SetupPacket)
 *    Return Value:    TRUE - Success, FALSE - Error
 */
{
  uint32_t n, m;

#if __OPTIMIZE_CODE
  uint8_t WB_L = SetupPacket.wIndex.WB.L;
#endif    
  switch (SetupPacket.bmRequestType.BM.Recipient) {
    case REQUEST_TO_DEVICE:
      EP0Data.pData = (uint8_t *)&USB_DeviceStatus;
      break;
    case REQUEST_TO_INTERFACE:
#if __OPTIMIZE_CODE
      if ((USB_Configuration != 0) && (WB_L < USB_IF_NUM)) {
#endif	  
#if defined(__GNUC__)
    	  (( unaligned_uint16 *)EP0Buf)->value = 0;
#else
    	  *((__packed uint16_t *)EP0Buf) = 0;
#endif
    	  EP0Data.pData = EP0Buf;
      } else {
        return (FALSE);
      }
      break;
    case REQUEST_TO_ENDPOINT:
#if __OPTIMIZE_CODE
      n = WB_L & 0x8F;
      m = USB_GetEPBitMsk(n);

#endif	  
      if (((USB_Configuration != 0) || ((n & 0x0F) == 0)) && (USB_EndPointMask & m)) {
#if defined(__GNUC__)
    	  (( unaligned_uint16 *)EP0Buf)->value = (USB_EndPointHalt & m) ? 1 : 0;;
#else
    	  *((__packed uint16_t *)EP0Buf) = (USB_EndPointHalt & m) ? 1 : 0;
#endif
    	  EP0Data.pData = EP0Buf;
      } else {
        return (FALSE);
      }
      break;
    default:
      return (FALSE);
  }
  return (TRUE);
}

 uint32_t USB_ReqSetClrFeature (uint32_t sc) 
/*
 *  Set/Clear Feature USB Request
 *    Parameters:      sc:    0 - Clear, 1 - Set
 *                            (global SetupPacket)
 *    Return Value:    TRUE - Success, FALSE - Error
 */
{
  uint32_t n, m;

  switch (SetupPacket.bmRequestType.BM.Recipient) {
    case REQUEST_TO_DEVICE:
      if (SetupPacket.wValue.W == USB_FEATURE_REMOTE_WAKEUP) {
        if (sc) {
#if __OPTIMIZE_CODE
          USB_HW_WakeupEnable();
#endif
          USB_DeviceStatus |=  USB_GETSTATUS_REMOTE_WAKEUP;
        } else {
#if __OPTIMIZE_CODE
          USB_HW_WakeupDisable();

#endif
          USB_DeviceStatus &= ~USB_GETSTATUS_REMOTE_WAKEUP;
        }
      } else {
        return (FALSE);
      }
      break;
    case REQUEST_TO_INTERFACE:
      return (FALSE);
    case REQUEST_TO_ENDPOINT:
      n = SetupPacket.wIndex.WB.L & 0x8F;
#if __OPTIMIZE_CODE
      m = USB_GetEPBitMsk(n);

#endif    
      if ((USB_Configuration != 0) && ((n & 0x0F) != 0) && (USB_EndPointMask & m)) {
        if (SetupPacket.wValue.W == USB_FEATURE_ENDPOINT_STALL) {
          if (sc) {
            USB_SetStallEP(n);
            USB_EndPointHalt |=  m;
          } else {
            USB_ClrStallEP(n);
            USB_EndPointHalt &= ~m;
          }
        } else {
          return (FALSE);
        }
      } else {
        return (FALSE);
      }
      break;
    default:
      return (FALSE);
  }
  return (TRUE);
}
#endif /*!__REMOVE_UNUSED_REQ*/



__inline uint32_t USB_ReqSetAddress (void) 
/*
 *  Set Address USB Request
 *    Parameters:      None (global SetupPacket)
 *    Return Value:    TRUE - Success, FALSE - Error
 */
{
#if  __OPTIMIZE_CODE
      USB_DeviceAddress = 0x80 | SetupPacket.wValue.WB.L;
#endif  /*__OPTIMIZE_CODE*/  
  return (TRUE);
}



uint32_t USB_ReqGetDescriptor (void) 
/*
 *  Get Descriptor USB Request
 *    Parameters:      None (global SetupPacket)
 *    Return Value:    TRUE - Success, FALSE - Error
 */
{
  uint8_t  *pD = 0;
  uint32_t len, n;
#if __OPTIMIZE_CODE
  uint8_t WB_H = SetupPacket.wValue.WB.H;
  uint8_t WB_L = SetupPacket.wValue.WB.L;
#endif  
  switch (SetupPacket.bmRequestType.BM.Recipient) {
  case REQUEST_TO_DEVICE:
#if __OPTIMIZE_CODE
      switch (WB_H) {

#endif        
    case USB_DEVICE_DESCRIPTOR_TYPE:
#if __OPTIMIZE_CODE
      pD = (uint8_t *)USB_DeviceDescriptor;	

#endif
      len = USB_DEVICE_DESC_SIZE;
      break;
          
    case USB_CONFIGURATION_DESCRIPTOR_TYPE:
      if(SetupPacket.wValue.WB.L != 0)
        return FALSE;
      pD = (uint8_t *)USB_ConfigDescriptor;
      len = ((USB_CONFIGURATION_DESCRIPTOR *)pD)->wTotalLength;
      break;
    case USB_STRING_DESCRIPTOR_TYPE:
      pD = (uint8_t *)USB_StringDescriptor; 
      for (n = 0; n != WB_L; n++) { 
        pD += ((USB_STRING_DESCRIPTOR *)pD)->bLength;
      }
      len = ((USB_STRING_DESCRIPTOR *)pD)->bLength;
      if(len == 0)
        return FALSE;
      break;
    default:
      return (FALSE);
    }
    break;
  case REQUEST_TO_INTERFACE:
#if __OPTIMIZE_CODE
      if(SetupPacket.wIndex.WB.L >= USB_IF_NUM)
        return FALSE;
      switch (WB_H) {
#else    
    switch (SetupPacket.wValue.WB.H) {
#endif        
#if USB_HID
#if !__REMOVE_UNUSED_REQ        
    case HID_HID_DESCRIPTOR_TYPE:
      pD = (U8 *)(GET_HID_DESCRIPTOR(SetupPacket.wIndex.WB.L));
      len = 9;
      break;
#endif /*!__REMOVE_UNUSED_CODE*/     
    case HID_REPORT_DESCRIPTOR_TYPE:
      pD = HID_ReportDescList[SetupPacket.wIndex.WB.L].ReportDesc;
      len = HID_ReportDescList[SetupPacket.wIndex.WB.L].ReportDescSize;
      break;
    case HID_PHYSICAL_DESCRIPTOR_TYPE:
      return (FALSE);                                     // HID Physical Descriptor is not supported 
#endif
    default:
      return (FALSE);
    }
    break;
  default:
    return (FALSE);
  }
#if __OPTIMIZE_CODE
  EP0Data.pData = pD;
#endif
  if (EP0Data.Count > len) {
    EP0Data.Count = len;
  }
  return (TRUE);
}



#if !__REMOVE_UNUSED_REQ
 __inline uint32_t USB_ReqGetConfiguration (void) 
/*
 *  Get Configuration USB Request
 *    Parameters:      None (global SetupPacket)
 *    Return Value:    TRUE - Success, FALSE - Error
 */
{
  switch (SetupPacket.bmRequestType.BM.Recipient) {
  case REQUEST_TO_DEVICE:
      EP0Data.pData = (uint8_t*)&USB_Configuration;
    break;
  default:
    return (FALSE);
  }
  return (TRUE);
}
#endif /*!__REMOVE_UNUSED_REQ*/

#if __OPTIMIZE_CODE
uint32_t USB_GetEPBitMsk(uint8_t EPNum)
/*
 *  Get bit mask of an endpoint
 *    Parameters:      Endpoint number
 *    Return Value:    None
 */
{
    return  ((EPNum & 0x80) ? ((1 << 16) << (EPNum & 0x0F)) : (1 << EPNum));
}

void USB_ReqDisableEP(uint8_t EPNum)
/*
 *  Disable an endpoint
 *    Parameters:      Endpoint number
 *    Return Value:    None
 */
{
   uint32_t n, m;

    m = USB_GetEPBitMsk(EPNum & 0x8F);  
    USB_EndPointMask &= ~m;
    USB_EndPointHalt &= ~m;
    USB_DisableEP(EPNum);
}
void USB_ReqDisableAllEP(void)
/*
 *  Disable all endpoints
 *    Parameters:      None
 *    Return Value:    None
 */
{
    uint32_t n;
    for (n = 1; n < USB_LOGIC_EP_NUM; n++) {
      USB_ReqDisableEP(n);
      USB_ReqDisableEP(n|0x80);
    }
}
void USB_ReqEnableEP(USB_ENDPOINT_DESCRIPTOR * pD)
{
    uint32_t n, m;

    n = ((USB_ENDPOINT_DESCRIPTOR *)pD)->bEndpointAddress & 0x8F;
    m = USB_GetEPBitMsk(n);
    USB_EndPointMask |=  m;
    USB_ConfigEP((USB_ENDPOINT_DESCRIPTOR *)pD);
    USB_EnableEP(n);
    USB_ResetEP(n);

}
#endif /*__OPTIMIZE_CODE*/

uint32_t USB_ReqSetConfiguration (void) 
/*
 *  Set Configuration USB Request
 *    Parameters:      None (global SetupPacket)
 *    Return Value:    TRUE - Success, FALSE - Error
 */
{
  USB_COMMON_DESCRIPTOR *pD;
  uint32_t alt = 0;
  uint32_t n, m;
#if __OPTIMIZE_CODE
  uint8_t WB_L = SetupPacket.wValue.WB.L;
#endif  
  switch (SetupPacket.bmRequestType.BM.Recipient) {
  case REQUEST_TO_DEVICE:
#if __OPTIMIZE_CODE
      if (WB_L) {
      pD = (USB_COMMON_DESCRIPTOR *)USB_ConfigDescriptor;
        if (((USB_CONFIGURATION_DESCRIPTOR *)pD)->bConfigurationValue != WB_L)
            return FALSE;
        USB_Configuration = WB_L;
            for (n = 0; n < USB_IF_NUM; n++) {
              USB_AltSetting[n] = 0;
            }
       USB_ReqDisableAllEP();
       USB_HW_Configure();
       
#if USB_SELF_POWER_SUPPORT
              USB_DeviceStatus |=  USB_GETSTATUS_SELF_POWERED;
#else
              USB_DeviceStatus &= ~USB_GETSTATUS_SELF_POWERED;
#endif
       for(n = 0; n < USB_IF_NUM; n++) {
         alt = ((USB_INTERFACE_DESCRIPTOR*)(GET_INTERFACE_DESCRIPTOR(n)))->bAlternateSetting;
         if(alt == 0) {
           USB_ReqEnableEP((USB_ENDPOINT_DESCRIPTOR*)(GET_ENDPOINT_DESCRIPTOR(n)));
          }
          }
        }
#endif
#if !__REMOVE_UNUSED_REQ      
      else {
      USB_Configuration = 0;
        USB_ReqDisableAllEP();
        USB_HW_DeConfigure();
    }

    if (USB_Configuration != SetupPacket.wValue.WB.L) {
      return (FALSE);
    }
#endif /*!__REMOVE_UNUSED_CODE */      
    break;
  default:
    return (FALSE);
  }
  return (TRUE);
}



#if !__REMOVE_UNUSED_REQ
__inline uint32_t USB_ReqGetInterface (void) 

/*
 *  Get Interface USB Request
 *    Parameters:      None (global SetupPacket)
 *    Return Value:    TRUE - Success, FALSE - Error
 */
 {
  switch (SetupPacket.bmRequestType.BM.Recipient) {
    case REQUEST_TO_INTERFACE:
    if ((USB_Configuration != 0) && (SetupPacket.wIndex.WB.L < USB_IF_NUM)) {
        EP0Data.pData = USB_AltSetting + SetupPacket.wIndex.WB.L;
      } else {
        return (FALSE);
      }
      break;
    default:
      return (FALSE);
  }
  return (TRUE);
}
#endif /*!__REMOVE_UNUSED_CODE*/


#if !__REMOVE_UNUSED_REQ
 uint32_t USB_ReqSetInterface (void) 
/*
 *  Set Interface USB Request
 *    Parameters:      None (global SetupPacket)
 *    Return Value:    TRUE - Success, FALSE - Error
 */
{
 if((SetupPacket.bmRequestType.BM.Recipient == REQUEST_TO_INTERFACE) &&
    (USB_Configuration != 0))
  {
      uint32_t alt = 0, old = 0;
      uint32_t n, m;
      uint8_t WB_L = SetupPacket.wIndex.WB.L;
      USB_INTERFACE_DESCRIPTOR* pIntDesc = (USB_INTERFACE_DESCRIPTOR*)(GET_INTERFACE_DESCRIPTOR(WB_L));
      USB_ENDPOINT_DESCRIPTOR* pEPDesc = (USB_ENDPOINT_DESCRIPTOR*)(GET_ENDPOINT_DESCRIPTOR(WB_L));

      if (WB_L  >= USB_IF_NUM)  return FALSE;
    
      // Get the old alternate setting
      old = USB_AltSetting[WB_L];
      alt = pIntDesc->bAlternateSetting;
      USB_AltSetting[WB_L] = alt;

          if (alt == SetupPacket.wValue.WB.L) {
        USB_ReqEnableEP(pEPDesc);
        }
      else if (alt == old) {
        USB_ReqDisableEP(pEPDesc->bEndpointAddress);
      }
      return TRUE;
  }
  return FALSE;
}
#endif /*#if !__REMOVE_UNUSED_REQ*/

void USB_EndPoint0 (uint32_t event) 
/*
 *  USB Endpoint 0 Event Callback
 *    Parameters:      event
 *    Return Value:    none
 */
{
  switch (event) {
  case USB_EVT_SETUP:
    USB_SetupStage();
    USB_DirCtrlEP(SetupPacket.bmRequestType.BM.Dir);
      EP0Data.Count = SetupPacket.wLength;     /* Number of bytes to transfer */
    switch (SetupPacket.bmRequestType.BM.Type) {

    case REQUEST_STANDARD:
        {        
      switch (SetupPacket.bRequest) {
#if !__REMOVE_UNUSED_REQ            
      case USB_REQUEST_GET_STATUS:
        if (!USB_ReqGetStatus()) {
          goto stall_i;
        }
        USB_DataInStage();
        break;

      case USB_REQUEST_CLEAR_FEATURE:
        if (!USB_ReqSetClrFeature(0)) {
          goto stall_i;
        }
        USB_StatusInStage();
        break;

      case USB_REQUEST_SET_FEATURE:
        if (!USB_ReqSetClrFeature(1)) {
          goto stall_i;
        }
        USB_StatusInStage();
        break;
#endif /*!__REMOVE_UNUSED_REQ*/
      case USB_REQUEST_SET_ADDRESS:
              USB_ReqSetAddress();
        USB_StatusInStage();
        break;

      case USB_REQUEST_GET_DESCRIPTOR:
        if (!USB_ReqGetDescriptor()) {
          goto stall_i;
        }
        USB_DataInStage();
        break;
#if !__REMOVE_UNUSED_REQ
      case USB_REQUEST_SET_DESCRIPTOR:
/*stall_o:*/  USB_SetStallEP(0x00);                       // not supported 
        EP0Data.Count = 0;
        break;
      case USB_REQUEST_GET_CONFIGURATION:
        if (!USB_ReqGetConfiguration()) {
          goto stall_i;
        }
        USB_DataInStage();
        break;
#endif /*!__REMOVE_UNUSED_REQ*/
      case USB_REQUEST_SET_CONFIGURATION:
        if (!USB_ReqSetConfiguration()) {
          goto stall_i;
        }
        USB_StatusInStage();
#if USB_CONFIGURE_EVENT
        USB_Configure_Event();
#endif
        break;
#if !__REMOVE_UNUSED_REQ
      case USB_REQUEST_GET_INTERFACE:
        if (!USB_ReqGetInterface()) {
          goto stall_i;
        }
        USB_DataInStage();
        break;

      case USB_REQUEST_SET_INTERFACE:
        if (!USB_ReqSetInterface()) {
          goto stall_i;
        }
        USB_StatusInStage();
#if USB_INTERFACE_EVENT
        USB_Interface_Event();
#endif
        break;
#endif /*!__REMOVE_UNUSED_REQ*/
      default:
        goto stall_i;
      }
      break;                                          // end case REQUEST_STANDARD
        }
#if USB_CLASS
    case REQUEST_CLASS:
      switch (SetupPacket.bmRequestType.BM.Recipient) {

      case REQUEST_TO_DEVICE:
        goto stall_i;                               // not supported

      case REQUEST_TO_INTERFACE:
				   
#if USB_HID												  // IF number correct?
        if(SetupPacket.wIndex.WB.L < USB_IF_NUM ) {
             // if (SetupPacket.wIndex.WB.L == USB_HID_IF_NUM) {           
          switch (SetupPacket.bRequest) {
          case HID_REQUEST_GET_REPORT:
            if (HID_GetReport()) {
                      goto data_in_stage;
            }
            break;
          case HID_REQUEST_SET_REPORT:
            EP0Data.pData = EP0Buf;               // data to be received  
            goto setup_class_ok;
          case HID_REQUEST_GET_IDLE:
            if (HID_GetIdle()) {
                      goto data_in_stage;
            }
            break;
          case HID_REQUEST_SET_IDLE:
            if (HID_SetIdle()) {
                      goto status_in_stage;
            }
            break;
          case HID_REQUEST_GET_PROTOCOL:
            if (HID_GetProtocol()) {
                      goto data_in_stage;
            }
            break;
          case HID_REQUEST_SET_PROTOCOL:
            if (HID_SetProtocol()) {
                      goto status_in_stage;
            }
            break;
          }
        }
#endif  /* USB_HID */
        goto stall_i;                               // end case REQUEST_TO_INTERFACE not supported 

      case REQUEST_TO_ENDPOINT:
        goto stall_i;								 // end case REQUEST_TO_ENDPOINT 

      default:
        goto stall_i;
      }
status_in_stage:
          USB_StatusInStage();               // send Acknowledge           
          goto setup_class_ok;
data_in_stage:
          EP0Data.pData = EP0Buf;             // point to data to be sent 
          USB_DataInStage();                  // send requested data 
#endif
setup_class_ok:                                           // request finished successfully 
      break;                                          // end case REQUEST_CLASS */


    default:
stall_i:  USB_SetStallEP(0x80);
      EP0Data.Count = 0;
      break;
    }
    break;                                              // end case USB_EVT_SETUP 
  case USB_EVT_OUT:
    if (SetupPacket.bmRequestType.BM.Dir == REQUEST_HOST_TO_DEVICE) {
      if (EP0Data.Count) {                              // still data to receive ? 
        USB_DataOutStage();                             // receive data 
        if (EP0Data.Count == 0) {                       // data complete ? 
          switch (SetupPacket.bmRequestType.BM.Type) {

          case REQUEST_STANDARD:
            goto stall_i;                                 // not supported 

#if (USB_CLASS) 
          case REQUEST_CLASS:
            switch (SetupPacket.bmRequestType.BM.Recipient) {
            case REQUEST_TO_DEVICE:
              goto stall_i;                               // not supported 

            case REQUEST_TO_INTERFACE:
#if USB_HID												  // IF number correct? 
					if (SetupPacket.wIndex.WB.L < USB_IF_NUM) {
                  //  if (SetupPacket.wIndex.WB.L == USB_HID_IF_NUM) {     
                switch (SetupPacket.bRequest) {
                case HID_REQUEST_SET_REPORT:
                  if (HID_SetReport()) {
                    USB_StatusInStage();                  // send Acknowledge 
                    goto out_class_ok;
                  }
                  break;
                }
              }
#endif                                                  // USB_HID                                                      // USB_AUDIO 
              goto stall_i;						                    //  end case REQUEST_TO_ENDPOINT 
                    

            default:
              goto stall_i;
            }
out_class_ok:                                             // request finished successfully
            break;                                        // end case REQUEST_CLASS 
#endif                                                    // USB_CLASS


          default:
            goto stall_i;
          }
        }
      }
    } else {
      USB_StatusOutStage();                               // receive Acknowledge 
    }
    break;                                                // end case USB_EVT_OUT

  case USB_EVT_IN :
    if (SetupPacket.bmRequestType.BM.Dir == REQUEST_DEVICE_TO_HOST) {
      USB_DataInStage();                                  // send data 
    } else {
      if (USB_DeviceAddress & 0x80) {
        USB_DeviceAddress &= 0x7F;
        USB_SetAddress(USB_DeviceAddress);
      }
    }
    break;                                                // end case USB_EVT_IN 
  }
}


