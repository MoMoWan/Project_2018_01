/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 * Name:    usbuser.h
 * Purpose: USB Custom User Definitions
 * Version: V1.20
 *----------------------------------------------------------------------------
 *---------------------------------------------------------------------------*/

#ifndef __HIDRAM_H__
#define __HIDRAM_H__


typedef struct
{
	volatile uint8_t	wHID_SetRptByte[16];
	volatile uint8_t	wHID_IdleTimeIf0ID;
	volatile uint8_t	wHID_Protocol;
	volatile uint8_t	wHID_SetReportFeature;
	union
	{
	volatile uint32_t	wHID_Setreportfeature[32];  //16[s
		uint8_t b_aryAPP_SetReportFeature[128];      // 64 [s    
	}ReportFeature;
	volatile uint32_t	wHID_Status;
}S_HID_DATA;

extern S_HID_DATA	sHID_Data;
extern uint8_t	bAPP_HIDStatus;

/*_____ D E C L A R A T I O N S ____________________________________________*/
 	/* USB bAPP_HIDStatus Register */
 	#define mskUSB_SetReportFeatureArrival	(0x1<<0)
	#define mskUSB_GetReportFeature			(0x1<<1)
	#define mskUSB_EPBusy					(0x1<<2)


#endif  /* __HIDRAM_H__ */
