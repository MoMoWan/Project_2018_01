/*++

Copyright (c) 2009-2010 Razer Ltd.
All rights reserved. This program contains proprietary and confidential information.

Module Name:
  avago3668_api.c

Compiler:
  Freescale CodeWarrior IDE v6.0, with support for MC9S08JM60 proccessor.

Abstract:
  This file provides the interface and support routines for the Avago SDNS-3668 Sensor.

--*/

#include "Type.h"
#include "wdt.h"
#include "variables.h"
#include "SPI.h"
#include "avago_api.h"
#include "mouse.h"
#include "protocol.h"
#include "gpio.h"
#include "avago3389_srom04.h"

S16 sensor_X = 0;
S16 sensor_Y = 0;
U16 sensorIntegrity;
U8 motion = 0;									
U8 Observation = 0;
U8 surfaceQual = 0;
U8 pixelAvg = 0;
U8 pixelSum = 0;
U32 pixelShutter = 0;
U8 pixelMax = 0;
U8 pixelMin = 0;
U8 shutterValueUpper = 0;
U8 shutterValueLower = 0;
U16 ShutterValue = 0;
U8 FramePeriod_Upper = 0;
U8 FramePeriod_Lower = 0;
U8 SROMchecktimer = 0;
U8 WriterMotionBurstflag = 0;

void spiDisplacement(void)
/*++

Function Description:
  This function reads the X and Y displacement values. The displacement values are
  read in burst mode to speed up the process. Reading the Motion register freezes the
  Delta data registers and should be read sequentially (registers 0x02, 0x03, and 0x04).
  Note that this function is written using the principles outlined in the read function.
 
Arguments:
  NONE

Returns: 
  NONE

--*/
{
  U8 buf[15] = {0x0};
//	motion = MOT_BIT;
	if (WriterMotionBurstflag == 0) {
	  SPI_W_2BYTE(REG_MOTION_BURST,0xff);   // Need write any value to Motion_Burst register,othwerwise can't read x,y data,request for pwm3389DM Mark 2016.3.23
    WriterMotionBurstflag  = 1;
  }
//  SPI_Burst_Read(REG_MOTION_BURST, buf, 12,1);                // Force Delta_X and Delta_Y to be read
  SPI_Burst_Read(REG_MOTION_BURST, buf);                // Force Delta_X and Delta_Y to be read
	motion = buf[0];
  Observation = buf[1]; 
  surfaceQual = buf[6];                                   // Reading surface Quality
	pixelSum = buf[7];                                      // Reading Sum pixel value
  pixelMax = buf[8];                                      // Reading Max pixel value
  pixelMin = buf[9];                                      // Reading Min pixel value
	shutterValueUpper = buf[10];                            // Reading Shutter Value Upper
  shutterValueLower = buf[11];                            // Reading Shutter Value Lower

	if((buf[0] & 0x80)) {
      sensor_X = (S16)(buf[2] |(buf[3] << 8)) ;           // Reading Delta X
      sensor_Y = (S16)(buf[4] |(buf[5] << 8)) ;           // Reading Delta Y
  }
}

void spiDownload_SROM(void)
/*++

Function Description:
  This function downloads the Avago Sensor firmware into the SROM. This is done by
  using burst mode write 1986 bytes to the SROM. After the sensor is put into the
  "download srom" mode, the download address is written out the bus followed by the
  data bytes coming at 10us intervals. Note that this function is written using the
  principles outlined in the write function. There are two different sensor firmware
  versions that can be downloaded, currently only the Standard displacement support
  is downloaded.

Arguments:
  NONE

Returns: 
  NONE

--*/
{  
	//[PMW3389 SROM download
	SPI_W_2BYTE(REG_CONFIGURATION_II,0x0);                 // Normal CPI setting affects both delta X and Y
	SPI_W_2BYTE(REG_SROM_ENABLE, 0x1D);                    // SROM_Enable registe for initializing
  delayMilliseconds(10);                                  // Delay 10ms 
  SPI_W_2BYTE(REG_SROM_ENABLE, 0x18);                    // start SROM download
  delayMicroseconds(200);							                    // Measured at 0.2ms, 

  SPI_W_NBYTE(0x80 | REG_SROM_LOAD_BURST, (U8 *)srom3389param, sizeof(srom3389param),1); // write SROM file into SROM_Load_Burst register
  delayMicroseconds(200);                                 // Delay 200us before the next command

}

BOOL spiChecksum_SROM(void)
/*++

Function Description:
  This function causes the Avago Sensor firmware to perform a CRC test on the SROM.
  After placing the sensor in the 'CRC test' mode, this function gives 7.5ms to allow
  the sensor to complete this task.
 
Arguments:
  NONE

Returns: 
  result - TRUE  = SROM is valid.
           FALSE = SROM is invalid.

--*/
{
  BOOL result = FALSE;
 
  if (SPI_R_BYTE(REG_SROM_ID) != SROM_ID) {                   //0x0d
    SPI_W_2BYTE(REG_SROM_ENABLE, SROM_CHECK);           // Put Avago Sensor into SROM CRC test mode
    delayMilliseconds(13);                                // Measured at 10 ms, allow CRC to be generated
    
    if (SPI_R_BYTE(REG_DATA_OUT_UPPER) == VALID_UPPER_CRC) {  
      if (SPI_R_BYTE(REG_DATA_OUT_LOWER) == VALID_LOWER_CRC) {
        result = TRUE;                                    // If CRC is valid, return TRUE
      }
    }
  }
  	// [PMW3389 power up step 7~10
	SPI_W_2BYTE(0x3d,0x80);
  SROMchecktimer = 0;
  do {
    wdt_resetTimer();                                     // If Watchdog Timer expires (1.024s), MCU resets 
    delayMilliseconds(1);
    if (SROMchecktimer > 55) {
      result = FALSE;
    }
  } while ((SPI_R_BYTE(0x3d) != 0xc0) && (result !=  FALSE));
	
	SPI_W_2BYTE(0x3d,0x0);
	SPI_W_2BYTE(REG_CONFIGURATION_II,0x04);                // diable rest mode
	// ]
  
  return (result);
}

void sensorProgramAPI(void)
/*++

Function Description:
  This function is the main interface to the Optical Sensor and updates the
  displacement value if there is data to be read. This function also checks
  every 3.0 seconds the sensors integrity and re-downloads the sensor firmware
  to the SROM and then reconfigures the sensor settings. The displacement
  values are read and adjusted based on resolution setting.
 
Arguments:
  NONE

Returns: 
  NONE

--*/
{
  spiDisplacement();                                      // Read sensor X,Y values until buffer empty
}


void checkSROM(void)
{
	U8 cnt=0;
	if (sensorIntegrity == 0) {							                // If time to check sensor integrity, then...
    WriterMotionBurstflag = 0;
    while ((SPI_R_BYTE(REG_SROM_ID) != SROM_ID) && (cnt < 3)) {
		  updateSensorFirmware();                             //  Download sensor firmware, reconfigure
		  cnt++;
		}  
		sensorIntegrity = 4000;                               //  Next Check after 4s        	
	}
}


void updateSensorFirmware(void)
/*++

Function Description:
  This function resets the sensor, then sets up burst write mode, to download the
  sensor firmware to the SROM. Once downloaded the firmware is verified through
  a sensor command, which creates a CRC value. After a successful firmware update
  all of the configuration settings have to be reapplied (shutter boundaries, 
  frame rate, and resolution). The sensor's integrity is checked every 3.0 seconds
  the 'sensorIntegrity' timer keeps track of the time.
 
Arguments:
  NONE

Returns: 
  NONE

--*/
{
  U8 result;
	U8 cnt = 0;
  delayMilliseconds(100);
   do {
    wdt_resetTimer();                                     // If Watchdog Timer expires (1.024s), MCU resets 
    sensorPowerUp();                                      // sensor power up
    spiDownload_SROM();                                   // SROM download
    result = spiChecksum_SROM();                          // SROM CRC test
		cnt++;
   } while ((result == FALSE) && (cnt < 6));              // If successful, reconfigure all settings
 
  setResolution(user.profile[user.ProNO].activeDpi.x,user.profile[user.ProNO].activeDpi.y);
}





void sensorPowerUp(void)
	/*
	Function Description:
	This function for every power up replace the function sensorReset
	*/
{     
	wdt_resetTimer();                                   // If Watchdog Timer expires (1.024s), MCU resets 
  // [PMW3389 Power up step2~5
	SPI_ASSERT;                                         // NCS high,then low to reset the SPI port
  SPI_W_2BYTE(REG_POWER_UP_RESET,0x5A);              // reset the chip
  delayMilliseconds(55);                              // wait for at least 50ms
	SPI_R_BYTE(REG_MOTION);                            // read from registers 0x02/0x03/0x04/0x05/0x06 one time regardless of the motion pin state
	SPI_R_BYTE(REG_DELTA_X_L);
	SPI_R_BYTE(REG_DELTA_X_H);
	SPI_R_BYTE(REG_DELTA_Y_L);
	SPI_R_BYTE(REG_DELTA_Y_H);
	// ]
}


void sensorShutDown(void)
{
	/*Function Description:
	This function for sensor shutdown function
	*/
	SPI_W_2BYTE(REG_SHUTDOWN,0xB6);
}

