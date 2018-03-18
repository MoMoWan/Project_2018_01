/*++

Copyright (c) 2012-2014 Razer Ltd.
All rights reserved. This program contains proprietary and confidential information.

Module Name:
  main.c

Compiler:
  Keil Software µVision v5.1, with NXP Semiconductor LPC11U3x proccessor support.

Abstract:
  This file provides the overall structure along with the initialization procedure,
  which includes the MCU and Board configurations. Debugging requires the commenting
  out the wdt_initialization() function.

--*/

#include "main.h"
#include "Type.h"
#include "iap.h"
//#include "usb_core.h"
//#include "usb_costom.h"
#include "usbHW.h"
#include "protocol.h"
#include "variables.h"
#include "pmu.h"
#include "gpio.h"
#include "wdt.h"
#include "timer.h"
#include "mouse.h"
#include "button.h"
#include "i2c_driver.h"
#include "avago_api.h"
#include "SPI.h"
#include "led_driver.h"
#include "macro.h"
//#include "sst26vf016b.h"
#include "storage.h"
#include "profile.h"
#include "keyboard.h"
#include	"usbram.h"
#include	"powermanagement.h"
#include "LED.h"
#include "LED_Function.h"
#include "LED_RamSetting.h"
#include "LED_Const.h"
#include "LED_Table.h"


void PFPA_Init(void);
void Check_Device_status(void );
//void irq_initialization(void);

#define RND_M 2147483647                                  // Random number generator terms
#define RND_A 16807
#define RND_Q (RND_M / RND_A)
#define RND_R (RND_M % RND_A)

int main(void)
/*++

Function Description:
  This function initializes the MCU and Board functions, resets all of the variables,
  configures settings based on previously saved state. Finally, enables interrupts and
  waits in this "Main Loop" for USB bus or Keyboard activity. The Lighting variables
  are set to turn on the Logo LED (acknownledging USB successful initialization) as
  soon as USB status equals CONFIGURED_STATE.

Arguments:
  NONE

Returns:
  NONE

--*/
{
//	U8 i;
  initializeHardware();                                   // Init clocks, timers, gpio definitions
  initializeVariables();                                  // Init state, reset all variables
	Check_Device_status();                                  // For retail mode
//	if (user.devicestatus == DEVICE_STATUS_RETAIL) {
//	  for (i = 0; i < NUMBER_OF_LEDS; i++) {
//			lighting[i].pendingeffect = CHROMA_EFX_WAVE;    // Retail mode ,LED effect is spectrum
//			Load_LED_Effect(&lighting[i]);
//			lighting[i].Chroma_effect.rate=60;        
//      lighting[i].Chroma_effect.flags=0;      
//		}
//		for ( ; ; ) {
//			wdt_resetTimer();
//			processVariableUpdate();                            // Process any updates to variables in storage
//		  updateLightingEffects();                            // process any led setting update   
//		}
//	} else if (user.devicestatus == DEVICE_STATUS_NORMAL) {  // For normal mode
		
    USB_Init();											                      // USB Initialization 
    do {
      wdt_resetTimer();                                   // If Watchdog Timer expires the MCU resets
      USB_checkStatus();                                  // Process USB Suspend and Resume events
    } while ((sUSB_EumeData.wUSB_SetConfiguration & BIT_0) == 0); //(USB_Configuration == 0);                     // Waiting unt  ill finish usb configuration
		for ( ; ; ) {
      wdt_resetTimer();                                   // If Watchdog Timer expires the MCU resets
      USB_checkStatus();                                  // Process USB Suspend and Resume events
      if ((sUSB_EumeData.wUSB_SetConfiguration & BIT_0) != 0) {
		    if (startFlag == 0x55) {
  		    checkSROM(); 
          opticalSensorPolling();                         // Poll Optical Sensor at specified rate 
        }
		    manualCalibration();
				AutoCalibrationReset();
        processVariableUpdate();                          // Process any updates to variables in storage
        processPendingTasks();                            // Process any pending USB HID Reports 
        processPendingCommand();                          // Process any pending Razer Protocol commands 
				macroEngine();                                    // Process any macro even
	      #if (BASIC_FW)
        BasicFwLedEffect();
				#else
        MN_LEDState();
//				updateLightingEffects();                          // process any led setting update  
				#endif          
		  }
    }
//  }
}


void initializeHardware()
/*++

Function Description:
  This function configures the hardware, which include the GPIO, the Clocks, the
  Timers, and the Interrupts. To manage the Scroll Wheel, the PIO1_6 pin is 
  configured for Input Active Edge Interrupts. To manage the scanning of the keypad
  matrix all of the columns initialized low and to be inputs, until they are turned
  around to outputs (optionally high drive outputs) one at time as each column is
  scanned. All rows are also set as inputs (never turned around). The LEDs pins are 
  all made outputs and are initialized high turning off all the LEDs. The Timer0 
  interrupt is configured as a 1ms periodic interrupt, which will manage LED effects,
  debouncing the keypad matrix, and all timing variables. Timer1 is setup to create 
  a Pulse Width Modulation for LED_BCKLIT. The ENCODER_SW and LED_ON/OFF switches are
  debounced the ENCODER_SW send as part of the mouse packet. The LED_ON/OFF processed
  internally to adjust the lighting.
--*/
{
//	#if releaseFW
//  LPC_SYSCON->SYSMEMREMAP = 0x01;                         // Device Vectors table remap to Ram
//
  SystemInit();                                           // Initialize system clock
  SystemCoreClockUpdate();
  PFPA_Init();
////  irq_initialization();
	gpio_initialization();                                  // Initialize general purpose IO
  wdt_initialization(WDT_MODE_RESET);                     // Set Watchdog Timer to Reset on expiration
  wdt_startTimer(WDT_TIMEOUT);                            // Start Watchdog Timer  
  SPI0_Init();                                            // Initialize SSP0
  SPI0_Disable();                                         // Disable SSP0 
//  i2c_initialization(MASTER);
//	PMU_Init();                                             // PMU and GPIO interrput setting
  Tmr_initialiation();                                    // Initialize 16-bit Timers
//  led_initialization();									                  // Initialize LED PWM/drivers
//	delayMilliseconds(10);
//	#if EXTER_FLASH_ENABLE
//	sst26Init();
//	#endif
//	delayMilliseconds(10);
//	LPC_SYSCON->SYSRSTSTAT |= LPC_SYSCON->SYSRSTSTAT;       // Clear pending System reset status. Otherwise, current status will keeping after Reset mcu to enter bootloader mode
//  deviceState = HW_INIT;                                  // Set Hardware is initialized and configured
}

void initializeVariables(void)
/*++

Function Description:
  This function initializes all variables to their default state including the keyboard
  variables. Then the flash stored variables are copied to RAM and the lighting effects
  updated to match stored value.

Arguments:
  NONE

Returns:
  NONE

--*/
{
//	U8 index;
	U8 i;
  milliseconds = 0;
  usbReceiveFlag = 0;
  updateStorage = 0;
  updateOneshot = 0;
  wakeupEvents = 0;																				// wake up flag, clear
  deviceMode = NORMAL_MODE;                               // Default mode = NORMAL_MODE
	calStatus = 2;//CAL_S_OFF;
	memset((U8*)matrixStaging,(int)0,(int)sizeof(matrixStaging)); //clear staging
	memset((U8*)lighting,(int)0,(int)sizeof(lighting));           // clear lighting
  memset((U8*)ledPwmData,(int)0,(int)sizeof(ledPwmData));       // clear lighting
	memset((U8*)&eepromSave,(int)0,(int)sizeof(eepromSave));       // clear lighting
	
	memset((U8*)&Macro_Ep1_In,(int)0,(int)sizeof(Macro_Ep1_In));   // clear lighting
	memset((U8*)&Macro_Ep2_In,(int)0,(int)sizeof(Macro_Ep2_In));   // clear lighting
	memset((U8*)&Macro_Ep3_In,(int)0,(int)sizeof(Macro_Ep3_In));   // clear lighting
  memset((U8*)&hid_key,0,sizeof(hid_key));                       // Clear
#if (EP_OVERWRITE_PROTECT)
  freeEp(1);
	freeEp(2);
	freeEp(3);
#endif
	
#if (BASIC_FW == 1)
	memset((void *)&sys,0,sizeof(sys));                                              // Factory setting
  iap_WriteEEPROM((void*)MANUFACTURING_SATRT, (void *)&sys, sizeof(sys));

	memset((void *)&user,0,sizeof(user));                                              // User setting
	iap_WriteEEPROM((void*)USER_SETTING_START, (void *)&user, sizeof(user));	
	
	memset((void *)&user,0,sizeof(user));                                          // Profile setting
	iap_WriteEEPROM((void*)Profile_SETTING_START, (void *)&user, sizeof(user));
	
	memset((void *)&macroHead,0,sizeof(macroHead));                               // Macro setting
	iap_WriteEEPROM((void*)MACRO_START, (void *)&macroHead, sizeof(macroHead)); 
	
	memset((void *)&blockMange,0,sizeof(blockMange));                               // Macro setting
	iap_WriteEEPROM((void*)BLOCK_MAMAGE_START, (void *)&blockMange, sizeof(blockMange)); 
	
	
	
#endif	

  
//  iap_ReadEEPROM((void *)MANUFACTURING_SATRT, (void *)&sys, sizeof(sys));
  memcpy(&sys, (void *)MANUFACTURING_SATRT, sizeof(sys)); 
  if (sys.storageSize != sizeof(sys)) {
    memcpy(&sys, &dft_special, sizeof(sys));                    // copy dft_special to sys
  }

//  iap_ReadEEPROM((void *)USER_SETTING_START, (void *)&user, sizeof(user));
//  memcpy(&user, (void *)USER_SETTING_START, sizeof(user));            // copy dft_generic to user
//  if ((user.storageSize != sizeof(user)) || (user.crc != crc16((U8 *)&user, (sizeof(user)-2)))) {
//    memcpy(&user, (void *)&dft_generic1, sizeof(user));            // copy dft_generic to user
//  }
	
//	iap_ReadEEPROM((void *)Profile_SETTING_START, (void *)&user, sizeof(user));
   memcpy(&user, (void *)Profile_SETTING_START, sizeof(user));       // copy dft_generic2 to user
  if ((user.storageSize != sizeof(user)) || (user.crc != crc16((U8 *)&user, (sizeof(user))-2))) {  // the "user-2" is just suitable for Keil,other compiler the crc may be not the last position 
    memcpy(&user, (void *)&dft_generic2, sizeof(user));       // copy dft_generic2 to user
		i=profileNO;
		while(i){                                              // All proflie need to update 
		 requestVariableUpdate(USER_CHANGED,--i);              // Generic1 variable storage needs updating
		}
  }
	
//	iap_ReadEEPROM((void *)MACRO_START, (void *)&macroTable, sizeof(macroTable));
   memcpy(&macroTable, (void *)MACRO_START, sizeof(macroTable));       // copy dft_generic2 to user
	if(macroTable.crc!=crc16((U8 *)&macroTable, (sizeof(macroTable))-2)){           // Macro not available
		formatMacroMemory();
		memset((void *)&macroTable,0,sizeof(macroTable));                               // Macro setting
		i=MAX_MACRO;
	  while(i){
		 requestVariableUpdate(MACRO_CHANGE,--i);
	 }
	}
	
//	iap_ReadEEPROM((void *)BLOCK_MAMAGE_START, (void *)&blockMange, sizeof(blockMange));
   memcpy(&blockMange, (void *)BLOCK_MAMAGE_START, sizeof(blockMange));       // copy dft_generic2 to user
	if(blockMange.crc!=crc16((U8 *)&blockMange, (sizeof(blockMange))-2)){           // Macro not available
		memset((void *)&blockMange,0,sizeof(blockMange));                               // Macro setting
		memset((void *)&macroTable,0,sizeof(macroTable));                               // Macro setting
		memcpy(&user, (void *)&dft_generic2, sizeof(user));                           // copy dft_generic2 to user
		formatMacroMemory();                                                            // Erase the flash
		i=profileNO;
		while(i){                                              // All proflie need to update 
		 requestVariableUpdate(USER_CHANGED,--i);              // Generic1 variable storage needs updating
		}
		i=MAX_MACRO;
	  while(i){
		 requestVariableUpdate(MACRO_CHANGE,--i);
	 }
	}
	checkMacroPowerUp();                                          // Check all macro data when power up
  checkAllProfileName();                                        // Check all profiles'name 
	memset((void *)&exterFlashErase,0,sizeof(exterFlashErase));    
  randomGenerate(1, randomGenerate(10, 100));                   // call it one time to avoid it frist call get fix value.
  forceLightingReload();                                        // Force a reload & refresh of all LEDs
	copyLiftoff2Active(0x00);                                     // Copy the lift value to active function
	initializeMouseVarialbes();                                   // Init variables about sensor, buttons, wheel decoder...
  initialButtons();                                             // Initial button
  deviceState = VAR_INIT;                                       // Set Variable are initialized and configured
}


void delayMilliseconds(U16 time)
/*++

Function Description:
  This function delays the specified number of milliseconds, using the 1ms
  periodic timer interrupt to update the time variable.
 
Arguments:
  time = Number of milliseconds to delay.

Returns: 
  NONE

--*/
{
  milliseconds = time;
	while (milliseconds != 0) {                             // Wait for specified time to elapse
    wdt_resetTimer();                                     // If Watchdog Timer expires the MCU resets
  }
}

void delayMicroseconds(U16 time)                          // Unit us
{
  U16 i,delay = time;
  for (i = 0; i <= delay; i++) {
 	__NOP();
	__NOP();
	__NOP();
 	__NOP();
	__NOP();
	__NOP();
 	__NOP();
	__NOP();
	__NOP(); 
 	__NOP();
	__NOP();
	__NOP();
 	__NOP();
	__NOP();
	__NOP();
 	__NOP();
	__NOP();
	__NOP(); 
 	__NOP();
	__NOP();
	__NOP();
 	__NOP();
	__NOP();
	__NOP();
 	__NOP();
	__NOP();
	__NOP(); 
 	__NOP();
	__NOP();
	__NOP();
 	__NOP();
	__NOP();    
  }
}

U16 crc16(U8* pData, U16 len)
/*++

Function Description:
  The function update_crc_ccitt calculates  a  new  CRC-CCITT
  value  based  on the previous value of the CRC and the next
  byte of the data to be checked.

Arguments:
  *pData = pointer to the data to caluate crc16
  len    = length of pData

Returns: 
  NONE

--*/
{
  U8 x;
  U16 crc = 0xFFFF;

  while (len--){
    x = crc >> 8 ^ *pData++;
    x ^= x >> 4;
    crc = (crc << 8) ^ ((U16)(x << 12)) ^ ((U16)(x <<5)) ^ ((U16)x);
  }
  return crc;
}
U16 crc16_parm(U8* pData, U16 len,U16 preCrc)
/*++

Function Description:
  The function update_crc_ccitt calculates  a  new  CRC-CCITT
  value  based  on the previous value of the CRC and the next
  byte of the data to be checked.

Arguments:
  *pData = pointer to the data to caluate crc16
  len    = length of pData
  preCrc = previous crc result
Returns: 
  NONE

--*/
{
  U8 x;
  U16 crc = preCrc;

  while (len--){
    x = crc >> 8 ^ *pData++;
    x ^= x >> 4;
    crc = (crc << 8) ^ ((U16)(x << 12)) ^ ((U16)(x <<5)) ^ ((U16)x);
  }
  return crc;
}

void Check_Device_status(void )
{
	U32 currentStatus ;
  currentStatus = BTN_INPUT;
//  buttons = button_input;
     switch (currentStatus) {
       case 0x66:             // B+F+L
         pollingChange = 1;
         user.pollingrate = 8;
         break;
       case 0x65:             // B+F+R 
         pollingChange = 2;
         user.pollingrate = 2;
         break;
       case 0x63:             // B+F+M
         pollingChange = 3;
         user.pollingrate = 4;
         break;
       case 0x27:             // B+F+D
         pollingChange = 4;
         user.pollingrate = 1;
         break;       
       default:
         break;                            
     }
  while ((BTN_INPUT & 0x7F) != 0x7F) {
    wdt_resetTimer();                                   // If Watchdog Timer expires the MCU resets
  }  
     if (pollingChange) {
       requestVariableUpdate(SW_CHANGEDPr,NULL);
//       bLED_Mode = S_LED_MODE_BLINK;
//       wLED_Status |= mskLED_ModeChange;
       bLED_DataRefreshTime_Reload = BLINK_REFRESH_TIME;
     } else {
       bLED_Mode = 3;//user.profile[user.ProNO].savedLight[0].effect;
       wLED_Status |= mskLED_ModeChange;
     }    
  //]   
}
//void irq_initialization(void)
///*++

//Function Description:
//  This function initializes the interrupt priorities of all the ISR handlers. The
//  only reason the default setting is not good enough is due the priority of the
//  Interrupt is below the Timer Interrupt. During the Timer Interrupt we need
//  to be able to control the LED Driver which is out on the I2C bus.

//Arguments:
//  NONE

//Returns:
//  NONE

//--*/
//{	
//  NVIC_SetPriority(USB_FIQn, 0);
//  NVIC_SetPriority(USB_IRQn, 1);
//  NVIC_SetPriority(TIMER_16_0_IRQn, 2);
//  NVIC_SetPriority(I2C_IRQn, 3);
//}


U32 randomGenerate(U32 lower, U32 upper)
/*++

Function Description:
  This function generates a pseudo random number within the specified range.

Arguments:
  lower - Low side of generated number.
  upper - High side of generated number.

Returns:
  value - Generated random number.

--*/
{
  static S32 rnd_seed = 1;                                  // Random number generator seed

  U32 value, test = 0x80000000, mask = 0xFFFFFFFF;
  U32 hi = upper, lo = lower;

  if (upper < lower) {                                    // If upper is smaller than lower limit, then...
    lo = upper;                                           // Swap the upper and lower limits
    hi = lower;
  }
  value = (hi - lo);                                      // Calculate range of numbers
  if (value != 0) {                                       // If valid range, then...
    while ((value & test) == 0) {                         // Build mask for upper range
      test >>= 1;                                         // Next bit position
      mask >>= 1;                                         // Next mask value
    }
    do {                                                  // Create a random # within the range
      rnd_seed = ((RND_A * (rnd_seed % RND_Q)) - (RND_R * (rnd_seed / RND_Q)));
      if (rnd_seed <= 0) {                                // If the seed is not positive, then...
        rnd_seed += RND_M;                                // Make the seed positive again
      }
      value = ((rnd_seed & mask) + lo);                   // Force range to start above lower
    } while (value > hi);                                 // If range is above upper, redo calculation
  } else {
    return (upper);                                       // Else there is no range (return upper)
  }
  return (value);
}

/*****************************************************************************
* Function		: PFPA
* Description	: Select each IO as PWM Output
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void PFPA_Init(void)
{
	SN_PFPA->CT16B1 |= 0x7C03F;			//PWMB Set 0 1 2 3 4 5 14 15 16 17 18 
}

