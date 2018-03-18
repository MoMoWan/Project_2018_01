/*++

Copyright (c) 2013-2014 Razer Ltd.
All rights reserved. This program contains proprietary and confidential information.

Module Name:
  variables.c

Compiler:
  Keil Software µVision v5.11, with NXP Semiconductor LPC11U2x proccessor support.

Abstract:
  This file provides the definitions for all of the global variables.

--*/

#include "variables.h"
#include "button.h"
#include "Type.h"

//PROFILE_PARM OBMemory[profileNO];
uint8_t  USB_Configuration;
U8 Endpoint_BusyFlag[4];
U8 SofCnt[4] ;
U8  deviceMode;                                           // 0=Normal, 1=Bootloader, 2=Test, 3=Driver Mode
U8  deviceState;                                          // 0=Boot, 1=HW_init, 2=Var_init, 3=USB_init
U8  wakeupEvents;
U16 milliseconds;
U8  calStatus;                                            // To indicate the calibration status
U8  calEnable;                                            // To recorder calibration function Enable or Disable
//U8  angleTune;
//U8  liftDis;

U8  ntfType;                                              // To store the notification type
U8  ntfValue;                                             // To store the notification event value 

U8  updateStorage = 0;                                    // Variable Storage needs updating flags
U16 updateOneshot = 0;                                    // Variable Storage updating oneshot timer                              

U8 pollingChange = 0;
U8 blinkCount = 0;
U8 pollingTimer = 0;
volatile uint16_t	dbMS_LED_ReflashTimeFrame;

U8  usbReceiveFlag;
U8  usbReceivelen = 0;
U8  protocolTransmit[PROTOCOL_DATA_SIZE] = {0};           // 8 bytes command + 80 bytes payload + 1 byte checksum + 1 Reserved


//const U32 fw_signature __attribute__((at(SIGNATURE_ADDR))) = STD_SIGNATURE;

//volatile U8 sw_bootloader __attribute__((at(0x10000FF8))); // Soft enter bootloader flag

#if BASIC_FW
const U8 firmwareVersion[4]  = {0,50,0,0};                // Basic Firmware v0.00.00.00
#else
const U8 firmwareVersion[4]  = {0,6,0,0};                 // Firmware version 
#endif

STORAGE1 sys __attribute__((aligned(4)));                                             // Variables filled from dft_special table
//STORAGE2 user __attribute__((aligned(4)));                                             // Variables filled from dft_generic & dft_profile tables
STORAGE3 user __attribute__((aligned(4)));
                                       
const COLOR DPICOLOR[5] = { {0xFF,0,0},{0xFF,0xFF,0},{0,0xFF,0},{0,0xFF,0xFF},{0,0,0xFF},};
const STORAGE1 dft_special = {
//                              {
                               sizeof(STORAGE1),          // Size of variable storage
                               {0, 0},                    // Mouse edtion informations
                               {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
//                               {
//                                 {0xFF, 0xFF, 0xFF, 0xFF}, // Logo led calibration
//															   {0xFF, 0xFF, 0xFF, 0xFF}, // Scroll led calibration
//																 {0xFF, 0xFF, 0xFF, 0xFF}, // Left side calibration
//																 {0xFF, 0xFF, 0xFF, 0xFF}, // Right side calibration
//														   },
//														  },
                              0,                          // crc
                             };

//const STORAGE2 dft_generic1 = {
//                               sizeof(STORAGE2),          // Size of variable storage
//                               0,                         // Emily Inhibit level: full function
//                               DFT_MAKE_DEBOUNCE,         // Mouse buttons make debounce time, default 8ms
//                               DFT_MAKE_MSK,              // Mouse buttons make mask time, default 20ms
//                               DFT_BREAK_DEBOUNCE,        // Mouse buttons break debounce time, default 8ms
//                               DFT_BREAK_MSK,             // Mouse buttons break mask time, default 20ms
//														   0,                         //Normal status
//															 RIGHT_HAND,
//                               1,                          // 1000 hz polling rate
//															 {
//															 	 150,                     // pulseOn: Length of "On Period" for Pulsating (1ms)                  
//																 1700,                    // pulseOff: Length of "Off Period" for Pulsating (1ms)                
//																 5,                       // pulseRiseTime: Length of "Step Rising step Time" for Pulsating (1ms)
//																 140,                     // pulseRise1Step: Size of "Rise 1 Step" in magnitude for Pulsating    
//																 140,                     // pulseRise2Step: Size of "Rise 2 Step" in magnitude for Pulsating    
//																 5,                       // pulseFallTime: Length of "Fall Step Time" for Pulsating (1ms)       
//																 117,                     // pulseFall1Step: Size of "Fall 1 Step" in magnitude for Pulsating   
//																 117,                     // pulseFall2Step: Size of "Fall 2 Step" in magnitude for Pulsating   
//																 30000,                   // pulseNeckLine: Size of "Neck line" of Rise or Fall 1 and 2        
//															 },
//                               0,                           // check sum
//													   };                      
//#define PROFILE_CONTENT 				    55, 
            
#define PROFILE_CONTENT 		 {                                                                                                                                    \
																	    { /* Logo led   */                                                                                                          \
																	      LED_LOGO_ID,                         /*   // Chroma region ID                                                 */          \
																	      CHROMA_EFX_SPECTRUM,                 /*   // effect ID                                                        */          \
																	      0xFF,                                /*   // COLOR R                                                          */          \
																	      0xFF,                                /*   // COLOR G                                                          */          \
																	      0xFF,                                /*   // COLOR B                                                          */          \
																	      55,                                /*   // Speed                                                            */          \
                                        0,                                   /*   // mode                                                             */          \
                                        0xFF,                                /*   // brightness                                                       */          \
															        },                                     /*                                                                       */          \
																      {    /*  Scroll led                                                                                             */          \
																	      LED_SCROLL_ID,                       /*   // Chroma region ID                                                 */          \
																	      CHROMA_EFX_SPECTRUM,                 /*   // effect ID                                                        */          \
																	      0xFF,                                /*   // COLOR R                                                          */          \
																	      0xFF,                                /*   // COLOR G                                                          */          \
																	      0xFF,                                /*   // COLOR B                                                          */          \
																	      55,                                /*   // Speed                                                            */          \
																	      0,                                   /*   // mode                                                             */          \
                                        0xFF,                                /*   // brightness                                                       */          \
															        },                                                                                                                          \
																			{  /* // Tail led                                                                                               */          \
																	      LED_TAIL_ID,                         /*   // Chroma region ID                                                 */          \
																	      CHROMA_EFX_SPECTRUM,                 /*   // effect ID                                                        */          \
																	      0xFF,                                /*   // COLOR R                                                          */          \
																	      0xFF,                                /*   // COLOR G                                                          */          \
																	      0xFF,                                /*   // COLOR B                                                          */          \
																	      55,                                /*   // Speed                                                            */          \
																	      0,                                   /*   // mode                                                             */          \
                                        0xFF,                                /*   // brightness                                                       */          \
															        },                                                                                                                           \
																	  },                                          /* // LED_PARM                                                           */    \
															                   /* //pollingRate,dpiStage,                                                                              */    \
															      {DPI_1600, DPI_1600},                                                                                                    \
                                    {5,2,0,0,2,{{DPI_400,DPI_400,DPI_400},{DPI_800,DPI_800,DPI_800},{DPI_1600,DPI_1600,DPI_1600},{DPI_3200,DPI_3200,DPI_3200},{DPI_4800,DPI_4800,DPI_4800}}},                 /*   // LIFTOFF_SENSOR_PARM                                                                  */    \
                                    {/*Button ID,  Mode ,  Type[0],Type[1]  Size, D1[0],D1[1]    D2[0],D2[1]  D3[0],D3[1]   D4[0],D4[1]    D5[0],D5[1]         */    \
                                      {BTN_LFT,   MODE_NORM, {BTN_MOUSE,0},   {1,0}, {BTN_LFT,0},     {0,0},      {0,0},        {0,0},         {0,0},  },               \
                                      {BTN_RGT,   MODE_NORM, {BTN_MOUSE,0},   {1,0}, {BTN_RGT,0},     {0,0},      {0,0},        {0,0},         {0,0},  },               \
                                      {BTN_MID,   MODE_NORM, {BTN_MOUSE,0},   {1,0}, {BTN_MID,0},     {0,0},      {0,0},        {0,0},         {0,0},  },               \
                                      {BTN_BCK,   MODE_NORM, {BTN_MOUSE,0},   {1,0}, {BTN_BCK,0},     {0,0},      {0,0},        {0,0},         {0,0},  },               \
                                      {BTN_FWD,   MODE_NORM, {BTN_MOUSE,0},   {1,0}, {BTN_FWD,0},     {0,0},      {0,0},        {0,0},         {0,0},  },               \
																			{BTN_QUICK, MODE_NORM, {BTN_XCLICK,0},{1,0}, {BTN_LFT,0},   {3,0},      {0,0},        {0,0},         {0,0},  },               \
                             	        {BTN_DUP,   MODE_NORM, {BTN_DPI,0},     {1,0}, {BTN_STAGE_LPUP,0},{0,0},      {0,0},        {0,0},         {0,0},  },               \
                                      {SCROLL_UP, MODE_NORM, {BTN_MOUSE,0},   {1,0}, {SCROLL_UP,0},   {0,0},      {0,0},        {0,0},         {0,0},  },               \
																			{SCROLL_DN, MODE_NORM, {BTN_MOUSE,0},   {1,0}, {SCROLL_DN,0},   {0,0},      {0,0},        {0,0},         {0,0},  },              \
																		}                
														 
														 
														 
														 
														 
														 
const STORAGE3 dft_generic2 = {
	                              sizeof(STORAGE3),                            // Size of variable storage
                                DFT_MAKE_DEBOUNCE,                           // Mouse buttons make debounce time, default 8ms
                                DFT_MAKE_MSK,                                // Mouse buttons make mask time, default 20ms
                                DFT_BREAK_DEBOUNCE,                          // Mouse buttons break debounce time, default 8ms
                                DFT_BREAK_MSK,                               // Mouse buttons break mask time, default 20ms  
                                1,                                           // POlling rate 
																1,                                           // profile Class 
																0,                                           // profile[profileNO] PClass-1
																5,                                           // Show the active profile             
	                              {
																	{// [ profile 1
																    1,                                         // Pofile ID 1
//																		0,                                         // Profile name address
//																		0,                                         // Profile name size
//                                    0,                                         // Profile name crc
//                                    0,                                         // Profile stage, 0 is the name not available
                                   PROFILE_CONTENT,                                    
														      }, // ] profile 1
															    {// [ profile 2
																    2,                                         // Pofile ID 2
//																		0,                                         // Profile name address
//																		0,                                         // Profile name size
//                                    0,                                         // Profile name crc
//                                    0,                                         // Profile stage, 0 is the name not available
                                    PROFILE_CONTENT,
														      }, // ] profile 2
																	{// [ profile 3
																    3,                                         // Pofile ID 3
//																		0,                                         // Profile name address
//																		0,                                         // Profile name size
//                                    0,                                         // Profile name crc
//                                    0,                                         // Profile stage, 0 is the name not available
                                    PROFILE_CONTENT,
														      }, // ] profile 3
																	{// [ profile 4
																    4,                                         // Pofile ID 4
//																		0,                                         // Profile name address
//																		0,                                         // Profile name size
//                                    0,                                         // Profile name crc
//                                    0,                                         // Profile stage, 0 is the name not available
                                    PROFILE_CONTENT,
														      }, // ] profile 4
																	{// [ profile 5
																    5,                                         // Pofile ID 5
//																		0,                                         // Profile name address
//																		0,                                         // Profile name size
//                                    0,                                         // Profile name crc
//                                    0,                                          // Profile stage, 0 is the name not available
                                    PROFILE_CONTENT,
														      }, // ] profile 5
//																	{// [ profile 5
//																    6,                                         // Pofile ID 6
//                                    PROFILE_CONTENT,
//														      }, // ] profile 5
																},
                                0,                           // check sum	
                              };

//                                      {/*Button ID,  Mode ,  Type[0],Type[1]  Size, D1[0],D1[1]    D2[0],D2[1]  D3[0],D3[1]   D4[0],D4[1]    D5[0],D5[1]         */    \
//                                      {BTN_LFT,  MODE_NORM, {BTN_MOUSE,0},   {1,0}, {BTN_LFT,0},     {0,0},      {0,0},        {0,0},         {0,0},  },               \
//                                      {BTN_RGT,  MODE_NORM, {BTN_MOUSE,0},   {1,0}, {BTN_RGT,0},     {0,0},      {0,0},        {0,0},         {0,0},  },               \
//                                      {BTN_MID,  MODE_NORM, {BTN_MOUSE,0},   {1,0}, {BTN_MID,0},     {0,0},      {0,0},        {0,0},         {0,0},  },               \
//                                      {BTN_BCK,  MODE_NORM, {BTN_MOUSE,0},   {1,0}, {BTN_BCK,0},     {0,0},      {0,0},        {0,0},         {0,0},  },               \
//                                      {BTN_FWD,  MODE_NORM, {BTN_MOUSE,0},   {1,0}, {BTN_FWD,0},     {0,0},      {0,0},        {0,0},         {0,0},  },               \
//																			{BTN_RBCK,  MODE_NORM,{BTN_MOUSE,0},   {1,0}, {BTN_BCK,0},     {0,0},      {0,0},        {0,0},         {0,0},  },               \
//                                      {BTN_RFWD,  MODE_NORM,{BTN_MOUSE,0},   {1,0}, {BTN_FWD,0},     {0,0},      {0,0},        {0,0},         {0,0},  },               \
//                             	        {BTN_DUP,  MODE_NORM, {BTN_DPI,0},     {1,0}, {BTN_STAGE_UP,0},{0,0},      {0,0},        {0,0},         {0,0},  },               \
//                                      {BTN_DDN,  MODE_NORM, {BTN_DPI,0},     {1,0}, {BTN_STAGE_DN,0},{0,0},      {0,0},        {0,0},         {0,0},  },               \
//																			{BTN_PFS,  MODE_NORM, {BTN_PROFILE,0}, {1,0}, {PROFILE_UP,0},  {0,0},      {0,0},        {0,0},         {0,0},  },              \
//                                      {SCROLL_UP, MODE_NORM,{BTN_MOUSE,0},   {1,0}, {SCROLL_UP,0},{0,0},     {0,0},        {0,0},         {0,0},  },               \
//																			{SCROLL_DN, MODE_NORM,{BTN_MOUSE,0},   {1,0}, {SCROLL_DN,0},{0,0},     {0,0},        {0,0},         {0,0},  },              \
//																		}                             
//                              
                                                            
 const U8 dpiTable[] = { 0x04,0x06,0x08,0x0B,0x0D,0x0F,0x12,0x14,0x16,0x19,0x1B,0x1D,0x20,0x22,0x24,0x27,0x29,0x2B,0x2E,0x30,0x32,0x34,0x37,0x39,0x3B,0x3E,0x40,0x42,0x45,0x47,\
                        0x49,0x4C,0x4E,0x50,0x53,0x55,0x57,0x5A,0x5C,0x5E,0x61,0x63,0x65,0x68,0x6A,0x6C,0x6F,0x71,0x73,};

//]
                        
          