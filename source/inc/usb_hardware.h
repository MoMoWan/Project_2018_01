/*++

Copyright (c) 2012-2014 Razer Ltd.
All rights reserved. This program contains proprietary and confidential information.

Module Name:
  main.c

Compiler:
  Keil Software µVision v5.11, with NXP Semiconductor LPC11U2x proccessor support.

Abstract:


--*/
#ifndef __USBHW_H__
#define __USBHW_H__

#include "usb_descriptor.h"
//#include "usb_core.h"

typedef struct _EP_LIST {
  uint16_t buf_ptr;	
  uint16_t buf_length : 10;
  uint16_t Type : 1;
  uint16_t RateFeedback_Toogle : 1;
  uint16_t ToogleReset : 1;
  uint16_t Stall : 1;
  uint16_t Disabled : 1;
  uint16_t Active : 1;	
} EP_LIST;

#define USB_EPLIST_ADDR		0x20004000
#define USB_EPDATA_ADDR		(USB_EPLIST_ADDR + 0x100)
#define USB_EPDATA_PAGE		(USB_EPDATA_ADDR&0xFFC00000)	/* Bit 22~31 is the page */
#define USB_RAM_LIMIT		0x800		/* maximum USB RAM size is 2K */

#if 0                     //delet
/* Be careful if ISO communication is used that it could allocate more than 64 bytes. */
#define USB_EP0DATA_OUT		(USB_EPDATA_ADDR + 0x00)
#define USB_SETUP_BUF		(USB_EPDATA_ADDR + 0x40)
#define USB_EP0DATA_IN		(USB_EPDATA_ADDR + 0x80)

#define USB_EP1DATA_OUT0	(USB_EPDATA_ADDR + 0x100)		/* INT/BULK EPs, max EP 64 bytes */
#define USB_EP1DATA_OUT1	(USB_EPDATA_ADDR + 0x140)
#define USB_EP1DATA_IN0		(USB_EPDATA_ADDR + 0x180)
#define USB_EP1DATA_IN1		(USB_EPDATA_ADDR + 0x1C0)

#define USB_EP2DATA_OUT0	(USB_EPDATA_ADDR + 0x200)
#define USB_EP2DATA_OUT1	(USB_EPDATA_ADDR + 0x240)
#define USB_EP2DATA_IN0		(USB_EPDATA_ADDR + 0x280)
#define USB_EP2DATA_IN1		(USB_EPDATA_ADDR + 0x2C0)

#define USB_EP3DATA_OUT0	(USB_EPDATA_ADDR + 0x300)
#define USB_EP3DATA_OUT1	(USB_EPDATA_ADDR + 0x340)
#define USB_EP3DATA_IN0		(USB_EPDATA_ADDR + 0x380)
#define USB_EP3DATA_IN1		(USB_EPDATA_ADDR + 0x3C0)

#define USB_EP4DATA_OUT0	(USB_EPDATA_ADDR + 0x400)
#define USB_EP4DATA_OUT1	(USB_EPDATA_ADDR + 0x440)
#define USB_EP4DATA_IN0		(USB_EPDATA_ADDR + 0x480)
#define USB_EP4DATA_IN1		(USB_EPDATA_ADDR + 0x4C0)
#endif

#define BUF_ACTIVE			(0x1U<<31)
#define EP_DISABLED			(0x1<<30)
#define EP_STALL			(0x1<<29)
#define EP_RESET			(0x1<<28)
#define EP_ISO_TYPE			(0x1<<26)

/* USB Device Command Status */
#define USB_EN              (0x1<<7)  /* Device Enable */
#define USB_SETUP_RCVD      (0x1<<8)  /* SETUP token received */
#define USB_PLL_ON          (0x1<<9)  /* PLL is always ON */
#define USB_LPM             (0x1<<11) /* LPM is supported */
#define USB_IntOnNAK_AO     (0x1<<12) /* Device Interrupt on NAK BULK OUT */
#define USB_IntOnNAK_AI     (0x1<<13) /* Device Interrupt on NAK BULK IN */
#define USB_IntOnNAK_CO     (0x1<<14) /* Device Interrupt on NAK CTRL OUT */
#define USB_IntOnNAK_CI     (0x1<<15) /* Device Interrupt on NAK CTRL IN */
#define USB_DCON            (0x1<<16) /* Device connect */
#define USB_DSUS            (0x1<<17) /* Device Suspend */
#define USB_LPM_SUS         (0x1<<19) /* LPM suspend */
#define USB_REMOTE_WAKE     (0x1<<20) /* LPM Remote Wakeup */
#define USB_DCON_C          (0x1<<24) /* Device connection change */
#define USB_DSUS_C          (0x1<<25) /* Device SUSPEND change */
#define USB_DRESET_C        (0x1<<26) /* Device RESET */
#define USB_VBUS_DBOUNCE    (0x1<<28) /* Device VBUS detect */

/* Device Interrupt Bit Definitions */
#define EP0_INT             (0x1<<0)
#define EP1_INT             (0x1<<1)
#define EP2_INT             (0x1<<2)
#define EP3_INT             (0x1<<3)
#define EP4_INT             (0x1<<4)
#define EP5_INT             (0x1<<5)
#define EP6_INT             (0x1<<6)
#define EP7_INT             (0x1<<7)
#define EP8_INT             (0x1<<8)
#define EP9_INT             (0x1<<9)
#define FRAME_INT           (0x1<<30)
#define DEV_STAT_INT        (0x80000000)

/* Rx & Tx Packet Length Definitions */
#define PKT_LNGTH_MASK      0x000003FF

/* Error Status Register Definitions */
#define ERR_NOERROR			0x00
#define ERR_PID_ENCODE      0x01
#define ERR_UNKNOWN_PID     0x02
#define ERR_UNEXPECT_PKT    0x03
#define ERR_TCRC            0x04
#define ERR_DCRC            0x05
#define ERR_TIMEOUT         0x06
#define ERR_BABBIE          0x07
#define ERR_EOF_PKT         0x08
#define ERR_TX_RX_NAK		0x09
#define ERR_SENT_STALL      0x0A
#define ERR_BUF_OVERRUN     0x0B
#define ERR_SENT_EPT_PKT    0x0C
#define ERR_BIT_STUFF       0x0D
#define ERR_SYNC            0x0E
#define ERR_TOGGLE_BIT      0x0F

#if __OPTIMIZE_CODE
/*    
 *    Based on the logic EP number and direction bit 7,
 *    when bit 7 is set, it's an IN EP, otherwise,
 *    it's an OUT EP. The return value is the
 *    location of the EP in the interrupt(status/enable/
 *    routing, etc) register.
 *    The bit info. of the interrupt register is
 *    bit 0 is EP0 OUT, bit 1 is EP0 IN,...
 *    bit 28 is EP14 OUT, bit 29 is EP14 IN....
 *    e.g. EPNum(0x80) is EP0 IN, the return
 *    value is 1; EPNum(0x8E) is EP14 IN, the return
 *    value is 29. 
 *    Parameters:      Logical EP with direction bit
 *    Return Value:    EP address in interrupt status.
 */
#define EPAddr(EPNum)        (((EPNum & 0x0F) << 1) + ((EPNum & 0x80) ? 1:0))

/*
 *  USB Get EP Command/Status Pointer Function
 *    EPNum.0..3: Address
 *    EPNum.7:    Dir
 *    Parameters:      EPList Location & Logical EP with direction bit
 *    Return Value:    Physical Memory location for EP list command/status
 */
#define USB_GetEPCmdStatusPtr(EPNum )      (USB_EPLIST_ADDR + ((EPAddr(EPNum)*2)<<2))

/*
 *  USB Get EP Data Buffer Pointer Function
 *    EPNum.0..3: Address
 *    EPNum.7:    Dir
 *    Parameters:      EPList Location & Logical EP with direction bit
 *    Return Value:    Physical Memory location for EP list command/status
 */
#define USB_GetEPBufPtr(EPNum)        ((uint16_t)((USB_EPDATA_ADDR + ((EPNum == 0xFF) ? 1: (EPAddr(EPNum)*2))*USB_MAX_PACKET0)>>6))
#define USB_GetEPPhyAddr(BufPtr)      ((BufPtr<<6)| USB_EPDATA_PAGE)
#define USB_GetEPDataPtr(EPNum )      (USB_GetEPPhyAddr(USB_GetEPBufPtr(EPNum)))

#define USB_HW_Configure()               (LPC_USB->INTEN  = DEV_STAT_INT | 0x3FF)
#define USB_HW_DeConfigure()             (LPC_USB->INTEN  = DEV_STAT_INT | 0x03)
#define USB_HW_Connect()                 (LPC_USB->DEVCMDSTAT |= USB_DCON)
#define USB_HW_Disconnect()              (LPC_USB->DEVCMDSTAT &= ~USB_DCON)
#define USB_HW_SetAddress(adr)           (LPC_USB->DEVCMDSTAT = (LPC_USB->DEVCMDSTAT & (~0x7F)) | (USB_EN | adr))
#define USB_HW_Wakeup()                  (LPC_USB->DEVCMDSTAT &= ~USB_DSUS)
#if REMOTE_WAKEUP_ENABLE
#define USB_HW_WakeupEnable()            {LPC_USB->DEVCMDSTAT &= ~USB_PLL_ON;LPC_SYSCON->USBCLKCTRL = 1;}
#define USB_HW_WakeupDisable()           {LPC_USB->DEVCMDSTAT &= ~USB_PLL_ON;LPC_SYSCON->USBCLKCTRL = 1;}
#else
#define USB_HW_WakeupEnable()      
#define USB_HW_WakeupDisable()
#endif
#define USB_HW_EPSkip(EPNum)             {LPC_USB->EPSKIP |= 0x1 << EPAddr(EPNum);  while ( LPC_USB->EPSKIP & (0x1 << EPAddr(EPNum)));}
#define USB_HW_GetIntSts()               (LPC_USB->INTSTAT)
#define USB_HW_ClearIntSts(x)            (LPC_USB->INTSTAT = x) 
#define USB_SetAddress(x)       USB_HW_SetAddress(x)
#endif /*__OPTIMIZE_CODE*/

extern U8  WakeUpPostponeFlag;
extern U16 WakeUpPostponeTimer;

/* USB Hardware Functions */
extern void  USBIOClkConfig (void);
extern void  USB_Init       (void);
extern void  USB_Reset      (void);
extern void  USB_Suspend    (void);
extern void  USB_Resume     (void);
extern void  USB_WakeUp     (void);
extern void  USB_EnableEP   (uint32_t EPNum);
extern void  USB_DisableEP  (uint32_t EPNum);
extern void  USB_ResetEP    (uint32_t EPNum);
extern void  USB_SetStallEP (uint32_t EPNum);
extern void  USB_ClrStallEP (uint32_t EPNum);
extern void  USB_ClearEPBuf (uint32_t EPNum);
extern uint32_t USB_ReadSetupEP (uint32_t EPNum, uint8_t *pData);
extern uint32_t USB_ReadEP  (uint32_t EPNum, uint8_t *pData);
extern uint32_t USB_WriteEP (uint32_t EPNum, uint8_t *pData, uint32_t cnt);
extern uint32_t USB_GetFrame(void);
extern void  USB_IRQHandler (void);
extern void  USBWakeup_IRQHandler(void);
__inline void RemoteWakeupEvent ( void ) {USB_WakeUp();}


#endif  /* __USBHW_H__ */
