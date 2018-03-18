/*++

Copyright (c) 2012-2014 Razer Ltd.
All rights reserved. This program contains proprietary and confidential information.

Module Name:
  keyboard.h

Compiler:
  Keil Software µVision v5.11, with NXP Semiconductor LPC11U2x proccessor support.

Abstract:
  This file contains the definitions for keyboard.c, which provides all of the keyboard
  scanning, debouncing, and processing activities.

--*/

#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "main.h"
#include "mouse.h"

//-------------------------------- Hardware Mappings -----------------------------------

#define PIN_LFT        ((uint32_t)(SN_GPIO2->DATA >> 1) & 1)	 // PIO0_23, Left button
#define PIN_MID        ((uint32_t)(SN_GPIO2->DATA >> 0) & 1)	 // PIO0_16, Middle button
#define PIN_RGT        ((uint32_t)(SN_GPIO2->DATA >> 15) & 1)	 // PIO0_7, Right button
#define PIN_LBCK       ((uint32_t)(SN_GPIO0->DATA >> 0) & 1)	 // PIO1_30, backward button
#define PIN_LFWD       ((uint32_t)(SN_GPIO0->DATA >> 1) & 1)	 // PIO0_13, faward button
#define PIN_RBCK       1//((uint32_t)(SN_GPIO2->DATA >> 11) & 1)	 // PIO0_11, backward button
#define PIN_RFWD       1//((uint32_t)(SN_GPIO2->DATA >> 12) & 1)	 // PIO0_12, faward button
#define PIN_QUICK      ((uint32_t)(SN_GPIO2->DATA >> 5) & 1)	 // PIO0_11, backward button
#define PIN_DPI_UP		((uint32_t)(SN_GPIO2->DATA >> 11) & 1)	 // PIO1_29, dpi up button
#define PIN_DPI_DOWN	1//((uint32_t)(SN_GPIO2->DATA >> 22) & 1)	 // PIO0_22, dpi down button

#define PIN_PROFILE_SWITCH 1//((uint32_t)(SN_GPIO2->DATA  >> 14) & 1)	 // PIO0_14, dpi down button
//#define BTN_INPUT ((PIN_LFT<<0) | (PIN_RGT<<1) | (PIN_MID<<2) | (PIN_LBCK<<3) | (PIN_LFWD<<4) | (PIN_RBCK<<5)|\
//                  (PIN_RFWD<<6)|(PIN_DPI_UP <<7 ) | (PIN_DPI_DOWN << 8)|((PIN_PROFILE_SWITCH << 9)) )

#define BTN_INPUT ((PIN_LFT<<0) | (PIN_RGT<<1) | (PIN_MID<<2) | (PIN_LBCK<<3) | (PIN_LFWD<<4) | (PIN_QUICK<<5)|\
                  (PIN_DPI_UP <<6 ) )

#define BTN_RETAIL  ((1<<1) |(1<<5)|(1<<6)|(1 <<7 )|(1 << 8))

#define PIN_PROFLIE_BUTTON   ((uint32_t)(SN_GPIO2->DATA  >> 31) & 1)	 // PIO1_31, profile switch button
#define MAX_DEBOUNCE          (40)                        // Max value of the user adjust button debounce time
#define DFT_MAKE_DEBOUNCE     (8)                         // Default button make debounce time in ms
#define DFT_BREAK_DEBOUNCE    (8)                         // Default button Break debounce time in ms
#define DFT_MAKE_MSK          (28)                         // Default button make masking time in ms
#define DFT_BREAK_MSK         (28)                         // Default button break masking time in ms

#define LEFT_HAND             (0x01)                      // Left hand 
#define RIGHT_HAND            (0x00)                      // Right hand
#define BTN_MAKE              (1)                         // Means button make event
#define BTN_BREAK             (0)                         // Means button break event
#define PRESSE                (0)
#define RELEASE               (1)
#define MASKING               (1 << 1)


#define DPI_UP_KEY            (0x20)                      // In driver mode, dpi up button pressed with send this key
#define DPI_DN_KEY            (0x21)                      // In driver mode, dpi down button pressed with send this key
#define BTN_RBCK_KEY          (0x22)                      // In driver mode ,BTN_RBCK pressed with send this key
#define BTN_RFWD_KEY          (0x23)                      // In driver mode ,BTN_RFWD pressed with send this key
#define BTN_Clutch_KEY        (0x24)                      // Clutch  button 
#define BTN_PROFILE_KEY       (0x50)                      // In driver mode ,Need to use DMK to let driver know button state change

#define BTN_PROFILE_IDX       (0x09)                      // Index for profile switch 
#define BTN_DPIDN_IDX         (0x08)                      // index for button dpi down
#define BTN_DPIUP_IDX         (0x07)                      // index for button dpi up

#define BTN_RBCK_RH           (0x05)                      // index for button Right backward for right hand
#define BTN_RFWD_RH           (0x06)                      // index for button right forward  for right hand  

#define BTN_RBCK_LH           (0x03)                      // index for button Right backward for left hand
#define BTN_RFWD_LH           (0x04)                      // index for button right forward  for left hand  


// ---------------- Button ID Value ---------------------------------------------------------
#define BTN_LFT               0x01              // BUTTON_ID: Left Button
#define BTN_RGT               0x02              // BUTTON_ID: Right Button
#define BTN_MID               0x03              // BUTTON_ID: Middle Button
#define BTN_BCK               0x04              // BUTTON_ID: Back Button
#define BTN_FWD               0x05              // BUTTON_ID: Forward Button
#define BTN_RBCK              0x06              // BUTTON_ID: Back Button
#define BTN_RFWD              0x07              // BUTTON_ID: Forward Button
#define BTN_QUICK             0x08              // BUTTON_ID: Forward Button
#define BTN_DUP               0x0B              // DPI UP
#define BTN_DDN               0x0C              // DPI DOWN
#define BTN_PFS               0x0E              // BUTTON_ID: Reserved :Profile switch
#define SCROLL_UP             0x09              // BUTTON_ID: Scroll up 
#define SCROLL_DN             0x0A              // BUTTON_ID: Scroll down 

// ------------------Button Mode Value -------------------------------------------------------
#define MODE_NORM             0x00              // Button Mode: Normal mode 
#define MODE_HYPERSHIFT       0x01              // Button Mode: HyperShift
//#define MODE_TURBO            0x02              // Button Mode: Turbo mode

#define NROM                   0                // Button mode  data is normal
#define SPECIFIC               1                // Button mdoe data is turbo or hypershift
// ---------------- Button Function Type--------------------------------------------------------
#define BTN_OFF               0x00              // FUNCTION_ID: Off
#define BTN_MOUSE             0x01              // FUNCTION_ID: Mouse Button
#define BTN_XCLICK            0x02              // FUNCTION_ID: Keyboard key
#define BTN_KYBRD             0x03              // FUNCTION_ID: Macro type I (Run the ID macro "0xNN" times)       
#define BTN_MEDIA             0x04              // FUNCTION_ID: Macro type I (Repeat the ID macro until button is released.)   
#define BTN_POWER             0x05              // FUNCTION_ID: Macro type I (Repeat the ID macro until button is pressed again.)  
#define BTN_DPI               0x06              // FUNCTION_ID: DPI
#define BTN_MACRO_I           0x10              // FUNCTION_ID: DPI
#define BTN_MACRO_II          0x11              // FUNCTION_ID: Profile switch
#define BTN_MACRO_III         0x12              // FUNCITON_ID: Lighting Scheme 
 
//------------------Profile Data ---------------------------------------------------------------

#define PROFILE_UP            0x01             // Profile up 
#define PROFILE_DOWN          0x02             // Profile down 
#define PROFILE_FIXID         0x03             // Jump to the profile id 

//-------------------DPI Data--------------------------------------------------------------------
#define BTN_STAGE_UP               0x01              // BUTTON_ID: DPI Up
#define BTN_STAGE_DN               0x02              // BUTTON_ID: DPI Down
#define BTN_STAGE_NN               0x03              // BUTTON_ID: DPI stage nn
#define BTN_ON_FLY                 0x04              // BUTTON_ID: DPI on the fly
#define BTN_VALUE                  0x05              // BUTTON_ID: DPI VALUE
#define BTN_STAGE_LPUP             0x06              // BUTTON_ID: DPI VALUE
#define BTN_STAGE_LPDN             0x07              // BUTTON_ID: DPI VALUE
//------------------Lighting scheme --------------------------------------------------------------
#define BTN_SCHEME_UP              0x01        // Lighting scheme up
#define BTN_SCHEME_DOWN            0x02        // Lighting scheme down

//-----------------Button Index------------------------------------------------------------------
#define INDEX_LEFT                 0x00               // Button index left
#define INDEX_RIGHT                0x01               // Button index right
#define INDEX_MIDDLE               0x02               // Button  index middle
#define INDEX_LBACK                0x03               // Button index left back
#define INDEX_LFWD                 0x04               // Button index left forward
#define INDEX_QUICK                0x05               // Button index right back
//#define INDEX_RBACK                0x05               // Button index right back
//#define INDEX_RFWD                 0x06               // Button index right forward
#define INDEX_DUP                  0x06               // Button index dpi up
//#define INDEX_DDN                  0x08               // BUtton index dpi down

//-----------------Scroll parameter------------------------------------------------------------------


#define SCROLL_UP_IDX              0x07             // Button index for scroll up 
#define SCROLL_DN_IDX              0x08             // Button index for scroll up 
typedef __packed struct _STANTARD_BUTTON {
	U8 status;                                              // 1 pressed, 0 released
	U8 active;						                                  // 1 had actived, 0 not actived
	U8 debounce;                                            // To record single key debounce time count
  U8 nativeDebounce;
	U8 pend;                                                // Pending byte for this is 32bit MCU, for better performance
  U8 debounceflag;                                        
} STANTARD_BUTTON;


typedef __packed struct _TURBO_PARM_{
//	bool run;
//	bool turboBreak;
//	bool writeEndpoint;
//	bool even;
	U8 stage;
	U16 scratch[NUMBER_OF_BUTTONS];
	U16 timer[NUMBER_OF_BUTTONS];
	U8  turboButtons;
	//U8  Turbo_In[REPORT_SIZE_KB];
	U8 buttonValue;
	U8 zCountcode;
}TURBO_PAR;

typedef __packed struct _TURBO_{
	TURBO_PAR kb;
	TURBO_PAR btn;
}TURBO;

extern STANTARD_BUTTON buttonStatus[NUMBER_OF_BUTTONS];
extern U8  currentButtons;
extern U8 profileButton;                                       // For profile switch button flag
extern U8  Hid_Ep2_Report_In[MAX_EP_SIZE];

extern TURBO turbo; 
extern BTN_PAR btnAssign[NUMBER_OF_BUTTONS];                                       // Button assignment (Profile 0 = Default Assignment)
extern void initialButtons(void);
extern U8 doubleClickEven;                                    // For double click even
extern U8 doubleClickButton;                                  // Double click buttons
extern U8 doubleClickCnt;                                     // Count the double click times
extern U8 onTheFlay;                                           // On the fly for DPI
void scanningButtonStatus(void);
void processButtonEvent(U8 eventType, U8 btnIdx);
void GetBtnAssgin(U8 profile ,U8 mode,U8 offset, U8* pData,U8 size);

#endif

