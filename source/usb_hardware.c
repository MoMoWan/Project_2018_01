/*++

Copyright (c) 2012-2014 Razer Ltd.
All rights reserved. This program contains proprietary and confidential information.

Module Name:
  usb_hardware.c

Compiler:
  Keil Software µVision v5.11, with NXP Semiconductor LPC11U2x proccessor support.

Abstract:


--*/

#include "usb_configure.h"
#include "usb_hardware.h"
#include "usb_core.h"
#include "usb_costom.h"
#include "Type.h"

#if POWERDOWN_MODE_USB_WAKEUP
#include "timer.h"
#endif
#include "variables.h"
#include "mouse.h"

#if !__REMOVE_UNUSED_REQ
/* The ActiveFlag is to deal with STALL situation, if STALL happens during
WriteEP(), don't set the ACTIVE bit in EPList CommandStatus immediately but 
this flag only, ACTIVE bit will be set after ClearStallEP() that data is ready
to be sent. This is a bit mask flag for non-zero EPs. */  
volatile uint32_t EPActiveFlag = 0;
#endif /*__REMOVE_UNUSED_REQ*/
/* EP buffer address and length default setting */
U8  WakeUpPostponeFlag = 0;
U16 WakeUpPostponeTimer = 0;

#define LOG_COUNT		0x20


void USBIOClkConfig( void )
/*    
 * Pull-down is needed, or internally, VBUS will be floating. This is to
 * address the wrong status in VBUSDebouncing bit in CmdStatus register. It
 * happens on the NXP Validation Board only that a wrong ESD protection chip is used
 *
 *    Configure clock and I/Os for USB.
 *    Parameters:      None
 *    Return Value:    None
 */
{ 
  LPC_SYSCON->SYSAHBCLKCTRL |= (0x1<<6) | (0x1<<14) | (0x1<<27); // Enable AHB clock to the GPIO, USB block and USB RAM

  LPC_SYSCON->USBPLLCLKUEN =0x00;                         // Select Sys Oscilator for USB PLL
  LPC_SYSCON->USBPLLCLKSEL =0x01;						              // Select PLL input
  LPC_SYSCON->USBPLLCLKUEN =0x01;                         // update clock source

  LPC_SYSCON->USBCLKUEN = 0x00;                           // Select Main Clock as USB Clock soruce (USB PLL not used)
  LPC_SYSCON->USBCLKSEL = 0x00;
  LPC_SYSCON->USBCLKUEN = 0x01;			

  LPC_IOCON->PIO0_3   = ((LPC_IOCON->PIO0_3 & (~0x1F))|(0x1<<3)|(0x01<<0));              // Secondary function VBUS			

  return;
}



void USB_EPInit( void ) 
/*
 *  USB EP List Initialize Function
 *    Called by the USB_Init to initialize USB EPList memory
 *    The example below is the simpliest case, every location is pre-configured
 *    with fixed length for each EP. See USB chapter, Endpoint Command/Status
 *    List for more details.
 *    CTRL, BULK or Interrupt IN/OUT EPs, max EP size is 64 
 *    For EP0, double buffer doesn't apply to CTRL EPs, but, EP0OUTBuf0 is
 *    for EP0OUT, EP0OUTBuf1 is for SETUP, EP0INBuf0 is for EP0IN, EP0INTBuf1 is
 *    reserved. Also note: ACTIVE bit doesn't apply to SETUP and Reserved EP buffer
 *     
 *    Parameters:      None
 *    Return Value:    None
 */
{
  uint32_t *addr, data_ptr;
  uint32_t i = 0;  
     
  addr = (uint32_t *)USB_EPLIST_ADDR;
  LPC_USB->EPLISTSTART = USB_EPLIST_ADDR;
  data_ptr = USB_EPDATA_ADDR;
  LPC_USB->DATABUFSTART = USB_EPDATA_PAGE; /* Bit 22~31 is the page */
  
  *addr++ = (USB_MAX_PACKET0<<16)|((uint16_t)(data_ptr>>6));
  data_ptr += USB_MAX_PACKET0;
  *addr++ = ((uint16_t)(data_ptr>>6));                      // No length field for SETUP 
  data_ptr += USB_MAX_PACKET0;

  *addr++ = (USB_MAX_PACKET0<<16)|((uint16_t)(data_ptr>>6));
  data_ptr += USB_MAX_PACKET0;
#if 0													  // EP0 IN second buffer(buffer1) is reserved
  *addr++ = (USB_MAX_PACKET0<<16)|((uint16_t)(data_ptr>>6));
#else
  addr++;
#endif

  data_ptr += USB_MAX_PACKET0;

  for ( i = 2; i < USB_EP_NUM; i++ )
  {
	
#if USB_ISO_COMMUNICATION								  // Buffer 0 
    *addr++ = EP_DISABLED|EP_ISO_TYPE|(USB_MAX_ISO_SIZE<<16)|((uint16_t)(data_ptr>>6));
	data_ptr += USB_MAX_ISO_SIZE;
#else
    *addr++ = EP_DISABLED|(USB_MAX_NON_ISO_SIZE<<16)|((uint16_t)(data_ptr>>6));
	data_ptr += USB_MAX_NON_ISO_SIZE;
#endif
	
#if USB_ISO_COMMUNICATION								  // Buffer 1 
    *addr++ = EP_DISABLED|EP_ISO_TYPE|(USB_MAX_ISO_SIZE<<16)|((uint16_t)(data_ptr>>6));
	data_ptr += USB_MAX_ISO_SIZE;
#else
    *addr++ = EP_DISABLED|(USB_MAX_NON_ISO_SIZE<<16)|((uint16_t)(data_ptr>>6));
	data_ptr += USB_MAX_NON_ISO_SIZE;
#endif
  }
  return;
}


void USB_Init (void) 
/*
 *  USB Initialize Function
 *   Called by the User to initialize USB
 *    Parameters:      None
 *    Return Value:    None
 */
{
  USBIOClkConfig();										   // Initialize clock and I/O pins for USB
  NVIC_EnableIRQ(USB_IRQn);								  // Enable the USB Interrupt
  USB_Reset();
  USB_SetAddress(0);
  deviceState = USB_INIT; 
#if __OPTIMIZE_CODE
  USB_HW_Connect();
#endif  
  return;
}

void USB_Reset (void) 
/*
 *  USB Reset Function
 *   Called automatically on USB Reset
 *    Parameters:      None
 *    Return Value:    None
 */
{
  USB_EPInit();
  LPC_USB->DEVCMDSTAT |= USB_EN;
  LPC_USB->INTSTAT = 0xC00003FF;						              // Clear all EP interrupts, device status, and SOF interrupts
                                                          // Enable all ten(10) EPs interrupts including EP0, note: EP won't be 
                                                          // ready until it's configured/enabled when device sending SetEPStatus command 
                                                          // to the command engine 
  LPC_USB->INTEN  = DEV_STAT_INT | 0x3FF | (USB_SOF_EVENT ? FRAME_INT : 0);

  return;
}

__inline void USB_Suspend (void)
/*++

Function Description:
  This function is automatically called on a USB Suspend event.

Arguments:
  NONE

Returns:
  NONE

--*/
{
  USB_DeviceStatus = (USB_DeviceStatus & (~USB_RESUME))| USB_SUSPEND;
	if (USB_Configuration != 0)                             //  except power up
    SUSPEND_led_fadout();
}

__inline void USB_Resume (void) 
/*++

Function Description:
  This function is automatically called on a USB Resume event.

Arguments:
  NONE

Returns:
  NONE

--*/
{
  USB_DeviceStatus = (USB_DeviceStatus & (~USB_SUSPEND))| USB_RESUME;
  WakeUpPostponeFlag = 1;
  WakeUpPostponeTimer = 0;
  if (SUSPEND_LED_fadoutflag == 1) {
    SUSPEND_LED_fadoutflag = 0;
    (void)memset((void*)matrixStaging,(int)0,(int)sizeof(matrixStaging));     //clear staging
  	(void)memset((void*)lighting,(int)0,(int)sizeof(lighting));   // clear lighting
	  forceLightingReload();                                  // Force a reload & refresh of all LEDs
  }
}

void USB_WakeUp (void) 
/*
 *  USB Remote Wakeup Function
 *   Called automatically on USB Remote Wakeup
 *    Parameters:      None
 *    Return Value:    None
 */
{

  if (USB_DeviceStatus & USB_GETSTATUS_REMOTE_WAKEUP) {
    USB_HW_Wakeup();
  }
}


void USB_DeactivateEP (uint32_t EPNum) 
/*
 *  Deactivate USB Endpoint
 *    Set the EP bits in the SKIP register and until the bits are
 *    cleared by the H/W. Clear the EP interrupts as well.
 *    It's used for non-zero EPs. For EP0 IN and OUT, clear
 *    the ACTIVE bit in EP Command/Status list will serve the purpose. 
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */
{
  uint32_t epbit;

  epbit = 0x1 << EPAddr(EPNum);
  LPC_USB->EPSKIP |= epbit;
  while ( LPC_USB->EPSKIP & epbit );
  LPC_USB->INTSTAT = epbit;			                      // Clear EP interrupt(s)
  return;
}



void USB_EnableEP (uint32_t EPNum) 
/*
 *  Enable USB Endpoint
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */
{
  uint32_t *addr = (uint32_t *)USB_GetEPCmdStatusPtr(EPNum );
        *addr &= ~EP_DISABLED;
  if((EPNum&0x80)==0x00)
        *addr |= BUF_ACTIVE;
      }

void USB_DisableEP (uint32_t EPNum)
/*
 *  Disable USB Endpoint
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */
{
  uint32_t *addr = (uint32_t *)USB_GetEPCmdStatusPtr(EPNum );
      *addr |= EP_DISABLED;
    }




void USB_ResetEP (uint32_t EPNum)
/*
 *  Toggle Reset USB Endpoint
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */
{
  uint32_t *addr = (uint32_t *)USB_GetEPCmdStatusPtr(EPNum );
  *addr = (*addr & (~EP_STALL))|EP_RESET;
}


void USB_SetStallEP (uint32_t EPNum) 
/*
 *  Set Stall for USB Endpoint
 *    STALL bit(s) can not be set until ACTIVE bit(s) are cleared.
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */
{
  uint32_t *addr = (uint32_t *)USB_GetEPCmdStatusPtr(EPNum );

  if(EPNum&0x0F) USB_DeactivateEP(EPNum);	
  *addr = (*addr & (~BUF_ACTIVE))|EP_STALL;
    }


#if !__REMOVE_UNUSED_REQ
void USB_ClrStallEP (uint32_t EPNum) 
/*
 *  Clear Stall for USB Endpoint
 *    For non-zero EP OUT, Toggle Reset should happen once the STALL bit(s)
 *    are cleared. At mean time, ACTIVE bit(s) and data length need to be
 *    reinitialized for EP read. 
 *    For non-zero EP IN, when Write EP occurs while STALL bit is set, 
 *    EPActiveFlag will be set, inside ClearSTALL, when this flag is set, it's
 *    time to set the ACTIVE bit so that the data buffer is ready to write data 
 *    to the EPs.
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

{
  uint32_t *addr = (uint32_t *)USB_GetEPCmdStatusPtr(EPNum );

	*addr &= ~EP_STALL;
  if (EPNum & 0x0F)
  {
	*addr |= EP_RESET;
	if ( !(EPNum & 0x80) )
	{ 
	  *addr &= ~(PKT_LNGTH_MASK<<16);			          // For non-zero EP OUT, ACTIVE bit and length field need to  be set again after clearing STALL
#if USB_ISO_COMMUNICATION
	  *addr |= ((USB_MAX_ISO_SIZE<<16)| BUF_ACTIVE);
#else
	  *addr |= ((USB_MAX_NON_ISO_SIZE<<16)| BUF_ACTIVE);
#endif
	}
	else
	{
	                                                     // For non-zero EP IN, the EPActiveFlag will be set when WriteEP() happens 
	                                                     // while STALL is set. If so, when ClearSTALL happens, set the ACTIVE bit that	  data buffer is ready to write data to the EPs	  
	  if ( EPActiveFlag & (0x1U << EPAddr(EPNum)) )
	  {
		*addr |= BUF_ACTIVE;
		EPActiveFlag &= ~(0x1U << EPAddr(EPNum));
	  }
	}
  }
  return;
}
#endif /*!__REMOVE_UNUSED_CODE*/

uint32_t USB_ReadSetupEP (uint32_t EPNum, uint8_t *pData) 
/*
 *  Read USB Endpoint SETUP Data
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *                     pData: Pointer to Data Buffer
 *    Return Value:    Number of bytes read
 */
{
  uint32_t *dataptr;
  uint32_t *addr;

  addr = (uint32_t *)USB_GetEPCmdStatusPtr( EPNum );

 if ( (*addr & EP_STALL) || (*(addr+2) & EP_STALL) )		// For the USB Enumeration compatibility 
  {
	*addr &= ~EP_STALL;
	*(addr+2) &= ~EP_STALL;
  }

  dataptr = (uint32_t *)(USB_GetEPDataPtr(0xFF));
  *(( uint32_t *)pData) = *dataptr;
  pData += 4;    dataptr++;
  *(( uint32_t *)pData) = *dataptr;

  addr++;
  *addr &= ~0x3FFFFFF;		
  *addr |= USB_GetEPBufPtr(0xFF); 
  return (USB_SETUP_PACKET_SIZE);
}

uint32_t USB_ReadEP (uint32_t EPNum, uint8_t *pData) 
/*
 *  Read USB Endpoint Data
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *                     pData: Pointer to Data Buffer
 *    Return Value:    Number of bytes read
 */
{
  uint32_t cnt, cnt1, n;
  uint32_t *dataptr;
  EP_LIST *EP = (EP_LIST*)USB_GetEPCmdStatusPtr(EPNum);

  dataptr = (uint32_t *)(USB_GetEPDataPtr(EPNum));
  if( EPNum & 0x0F ) cnt1 = USB_MAX_NON_ISO_SIZE;
  else cnt1 = USB_MAX_PACKET0;
      cnt = cnt1 - (EP->buf_length);

  for (n = 0; n < (cnt + 3) / 4; n++) {
    *((__packed uint32_t *)pData) = *dataptr;
	pData += 4;
	dataptr++;
  }
  EP->buf_ptr = USB_GetEPBufPtr(EPNum); 
  EP->buf_length = cnt1;
  EP->Active = 1;
  return cnt;
}


uint32_t USB_WriteEP (uint32_t EPNum, uint8_t *pData, uint32_t cnt) 
/*
 *  Write USB Endpoint Data
 *    For EP0 OUT, ACTIVE bits will not be set until SETUP and
 *	  EP0 IN have been received. 
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *                     pData: Pointer to Data Buffer
 *                     cnt:   Number of bytes to write
 *    Return Value:    Number of bytes written
 */
{
  uint32_t n;
  uint32_t *dataptr, *addr;
  EP_LIST *EP = (EP_LIST*)USB_GetEPCmdStatusPtr(EPNum );

  do {
    __nop();
    addr = (uint32_t *)USB_GetEPCmdStatusPtr(EPNum );                //of course this endpoint is IN endpoint
  // [ Problem happen in Keil T3, fast press keys and plug in, get unknow device. it cause by FW try to report key during enumeration
  //} while(((EPNum & 0x0F) && ((*addr) & (BUF_ACTIVE))) != 0);          // Not EP0 and ep isn't acitve
  // ]
  } while(((*addr) & (EP_DISABLED)) != 0);          // Not EP0 and ep isn't acitve

  dataptr = (uint32_t *)(USB_GetEPDataPtr(EPNum));

  for (n = 0; n < (cnt + 3) / 4; n++) {
    *dataptr = *((__packed uint32_t *)pData);
    pData += 4;
    dataptr++;
  }

  if ( !(EPNum & 0x0F) )
  {
    *(addr-2) |= BUF_ACTIVE;
  }
  EP->buf_ptr = USB_GetEPBufPtr(EPNum);
  EP->buf_length = cnt;
  EP->Active = 1;
  return cnt;
}


#if USB_FIQ_EVENT

void USB_FIQHandler( void )
/* USB FIQ Interrupt Service Routine for SOF */
{
  uint32_t disr;

  disr = LPC_USB->IntStat;                                // Device Interrupt Status 
  LPC_USB->IntStat = disr;

  if (disr & FRAME_INT) {
	SOFFIQCount++;
  }
  return;
}
#endif



void USB_IRQHandler (void)
/*
 *  USB Interrupt Service Routine
 *    All the device status, EP, and SOF interrupts are handled here.
 *    Parameters:      None
 *    Return Value:    None
 */
{
  uint32_t disr, val, n, m;

  disr = LPC_USB->INTSTAT;                                // Get Interrupt Status and clear immediately
  LPC_USB->INTSTAT = disr;                                         
  wakeupEvents |= WAKE_NON_IDLE;                          // Wokeup on non-idle USB event   
  if (disr & DEV_STAT_INT) {				                      // Device Status Interrupt (Reset, Connect change, Suspend/Resume)
    val = LPC_USB->DEVCMDSTAT;                            // Device Status 
    if (val & USB_DRESET_C) {                             // Reset 
      USB_Reset();
#if   USB_RESET_EVENT
      USB_Reset_Event();
#endif
      goto isr_end;
    }
    if (val & USB_DCON_C) {                               // Connect change 
#if   USB_POWER_EVENT
      USB_Power_Event(val & DEV_CON);
#endif
      goto isr_end;
    }
    if (val & USB_DSUS_C) {                               // Suspend/Resume
      if (val & USB_DSUS) {                               // Suspend 
        USB_Suspend();
      } else {                                            // Resume 
        USB_Resume();
      }
      goto isr_end;
    }
  }
  
#if USB_SOF_EVENT
  if (disr & FRAME_INT) {								                  // Start of Frame Interrupt
		
		USB_SOF_Event();
		
		if(SofCnt[1]!=0){
    
			SofCnt[1]--;
		}
		else{
			if(isEpBusy(1)){
				freeEp(1);
				
			}
		}
	 
		
		
		if(SofCnt[2]!=0){
			
			SofCnt[2]--;
		}
		else{
			if(isEpBusy(2)){
				freeEp(2);
			}
		}
  }
#endif

#if USB_ERROR_EVENT
  /* NO error interrupt anymore, below code can be used
  as example to get error status from USBInfo register */
#if  USB_DEBUG
    DeviceErrorCode = (LPC_USB->Info >> 11) & 0x0F;
#endif
#endif
  
  if (disr & 0x3FF) {									                    // Endpoint's Interrupt
    for (n = 0; n < USB_EP_NUM; n++) {                    // Check All Endpoints 
	    if (disr & (1 << n)) {
        m = n >> 1;
        if ((n & 1) == 0) {                               // OUT Endpoint 
          if (n == 0) {                                   // Control OUT Endpoint 
            if (LPC_USB->DEVCMDSTAT & USB_SETUP_RCVD) {									
                USB_P_EP[0](USB_EVT_SETUP);
                LPC_USB->DEVCMDSTAT |= USB_SETUP_RCVD;
                continue;
              }
            }
            USB_P_EP[m](USB_EVT_OUT);
        } else {                                          // IN Endpoint 
         
            USB_P_EP[m](USB_EVT_IN);
          }
        }
      }
    }
isr_end:
  return;
}

#if POWERDOWN_MODE_USB_WAKEUP
/*
 *  USB Wakeup Interrupt Service Routine
 *    A dedicated USB wakeup interrupt is handled in LPC11Uxx.
 *    In earlier version, USB interrupt can only wake up MCU
 *    normal sleep mode. When RESUME occurs from the host, this dedicated 
 *    USB wakeup interrupt can be used to wake up MCU from either sleep or 
 *    deep sleep mode.
 *    Parameters:      None
 *    Return Value:    None
 */

void USBWakeup_IRQHandler(void) 
/*++

Function Description:
  This function is the wake-up Interrupt Handler. It configures all of the port
  pins as wake-up sources.

Arguments:
  NONE

Returns:
  NONE

--*/

{

}

#endif

