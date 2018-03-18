/*++

Copyright (c) 2013-2014 Razer Ltd.
All rights reserved. This program contains proprietary and confidential information.

Module Name:
  gpio.c

Compiler:
  Keil Software µVision v5.11, with NXP Semiconductor LPC11U2x proccessor support.

Abstract:


--*/


#include "main.h"
#include "gpio.h"
#include "avago_api.h"

void gpio_initialization(void)
/*++

Function Description:
  This function initializes all of the processor port pins used in project.
  Unused pin use its default status [input]

Arguments:
  NONE

Returns:
  NONE

--*/
{
//  // [ Wheed encoder IOs
//	LPC_IOCON->PIO0_18 = 0xB0;                              // Pin46, Wheel encoder QA (PIO0_18, GPIO, Input, internal pull-up and Hysteresis enable)
//	gpio_Direction(0, 18, 0);                               // input	
//  LPC_IOCON->PIO0_19 = 0xB0;                              // Pin47, Wheel encoder QB (PIO0_19, GPIO, Input, internal pull-up and Hysteresis enable)
//	gpio_Direction(0, 19, 0);                               // input
//  // ]

//  // [ sensor
//	LPC_IOCON->PIO1_10 = 0x90;                              // Pin1, PIO1-10 sensor IRQ (GPIO, Input, internal pull-up)
//	gpio_Direction(1,  10, 0);                              // input	
//  LPC_IOCON->PIO0_20 = 0x90;                              // Pin9, PIO0-20 LED_CC2 Motion (GPIO, output)
//  gpio_Direction(0,  20, 0);                              // input
//	LPC_IOCON->PIO0_2 = 0x80;                               // Pin10, PIO0-2 NCS (GPIO, output)
//  gpio_Direction(0,  2, 1);                               // output
//	// ]
//	
//	// [ sensor
////	LPC_IOCON->PIO0_16 = 0x90;                              // Pin40, PIO0-16 sensor IRQ (GPIO, Input, internal pull-up)
////	gpio_Direction(0,  16, 0);                              // input
////  LPC_IOCON->PIO1_19 = 0x80;                              // Pin2, PIO1-19 LED_CC1 Motion (GPIO, output)
////  gpio_Direction(1,  19, 1);                              // output
////	LPC_IOCON->PIO1_23 = 0x80;                              // Pin18, PIO1-23 NCS (GPIO, output)
////  gpio_Direction(1,  23, 1);                              // output
////	// ]



////[#wp for flash ic
////	LPC_IOCON->PIO0_16 = 0x80;                              // Pin40, PIO0-16 sensor IRQ (GPIO, Input, internal pull-up)
////	gpio_Direction(0,  16, 1);                              // input
////	gpio_SetValue(0, 16, 0);                                // output low
////]	
//  // [ Mouse button IOs
//  LPC_IOCON->PIO0_23 = 0x90;                              // Pin21: Mouse left button (PIO0_23, Input, internal pull-up enable)
//  gpio_Direction(0, 23, 0);                               // Set as an input
//  LPC_IOCON->PIO1_13 = 0x90;                               // Pin23: Mouse right button (PIO1_13, Input, internal pull-up enable)
//  gpio_Direction(1,  13, 0);                               // Set as an input 
//  LPC_IOCON->PIO0_16 = 0x90;                              // Pin47: Mouse middle button (PIO0_16, Input, internal pull-up enable)
//  gpio_Direction(0, 16, 0);                               // Set as an input
//	
//	LPC_IOCON->TDO_PIO0_13 = 0x91;					                // PIO0_13 (Left FWD pull-up enabled)
//  gpio_Direction(0, 13, 0);						                    // Set FWD1 to Input
//  LPC_IOCON->PIO1_30 = 0x90;					                    // PIO1_30 (Left BWD pull-up enabled)
//  gpio_Direction(1, 30, 0);	                              // Set as input 
//	
//	LPC_IOCON->TMS_PIO0_12 = 0x91;					                // PIO0_12 (Right FWD pull-up enabled)
//  gpio_Direction(0, 12, 0);						                    // Set FWD1 to Input
//  LPC_IOCON->TDI_PIO0_11 = 0x91;					                // PIO0_11 (Right BWD pull-up enabled)
//  gpio_Direction(0, 11, 0);	                              // Set as input 
//	
//  LPC_IOCON->PIO1_29 = 0x90;                              // Pin33: Mouse dpi up button (PIO1_29, Input, internal pull-up enable)
//  gpio_Direction(1, 29, 0);                               // Set as an input
//  LPC_IOCON->PIO0_22 = 0x90;                              // Pin34: Mouse dpi down button (PIO0_22, Input, internal pull-up enable)
//  gpio_Direction(0, 22, 0);                               // Set as an input
//  // ]
//	
//	// [ LED SDB PIN
//	LPC_IOCON->PIO1_26 = 0x80;                              // led driver SDB control 
//  gpio_Direction(1,  26, 1);                              // Set as an output
//  // ]
//	
//	//[ Profile switch button
//	LPC_IOCON->TRST_PIO0_14 = 0x91;					                // PIO0_14 (BWD pull-up enabled)
//  gpio_Direction(0, 14, 0);	                              // Set as input 
//	//]
//	
//	
//	////////////////////////////For test//////////////////////
////	LPC_IOCON->PIO1_7 = 0x90;                              // Pin46, Wheel encoder QA (PIO0_18, GPIO, Input, internal pull-up and Hysteresis enable)
////	gpio_Direction(1, 7, 1);                               // input	
////	gpio_SetValue(1, 7, 0); 

//[
	SN_GPIO0->MODE = 0x0;				//Set P0
	SN_GPIO0->CFG = 0x0;					//Set P0 as pullup
	
	SN_GPIO1->MODE = 0;					//Set P1 mode as input
	SN_GPIO1->CFG = 0;					//Set P1 as pullup	
		
	SN_GPIO2->MODE = 0;					//Set P2 mode as input
	SN_GPIO2->CFG = 0;					//Set P2 as pullup

//	SN_GPIO3->MODE = 0;					//Set P3 mode as input
//	SN_GPIO3->CFG = 0;					//Set P3 as pullup
	
//	__SPI0_OUTPUTMODE_SEL0;
	SN_GPIO0->MODE |= 0x20;	//set P05 is ouput mode
  
	SN_GPIO3->MODE_b.MODE11 = 1;			
	SN_GPIO3->DATA_b.DATA11 = 0;
	//EN1
	SN_GPIO3->MODE_b.MODE10 = 1;			
	SN_GPIO3->DATA_b.DATA10 = 0;
	//EN2
	SN_GPIO3->MODE_b.MODE9 = 1;			
	SN_GPIO3->DATA_b.DATA9 = 0;
	//EN3
	SN_GPIO3->MODE_b.MODE8 = 1;			
	SN_GPIO3->DATA_b.DATA8 = 0;  
//]
//[  scroll interrupt setting 
  SN_GPIO2->IBS = 0x18;
	SN_GPIO2->IEV = 0x18;
	SN_GPIO2->IC =0xFFFFFFFF;
  SN_GPIO2->IE = 0x18;
	NVIC_ClearPendingIRQ(P2_IRQn);
	NVIC_EnableIRQ(P2_IRQn);  
//]
}


void gpio_Direction( U32 portNum, U32 bitPosi, U32 dir )
/*++

Function Description:
  Set the direction in GPIO port

Arguments:
  portNum - GPIO port 0 or 1
  bitPosi    - GPIO0_x
  dir          - 1 output, 0 inout

Returns:
  NONE

--*/
{
  if (dir) {
    ((SN_GPIO0_Type           *) SN_GPIO0_BASE + (portNum*2000))->MODE |= (1<<bitPosi);
    //LPC_GPIO->DIR[portNum] |= (1<<bitPosi);
  } else {
    //LPC_GPIO->DIR[portNum] &= ~(1<<bitPosi);
    ((SN_GPIO0_Type           *) SN_GPIO0_BASE + (portNum*2000))->MODE &= ~(1<<bitPosi);
  }
}


void gpio_SetValue(U32 portNum, U32 bitPosi, U32 bitVal)
/*++

Function Description:
  Set the direction in GPIO value

Arguments:
  portNum - GPIO port 0 or 1,
  bitPosi    - GPIO0_x
  bitVal      - 1 set to high, 0 clear to low

Returns:
  NONE

--*/
{
#if 1 // Daniel modify to reduce code space, Pass test 2013.03.21
  if (bitVal == 0) {
    //LPC_GPIO->CLR[portNum] |= (0x1<<bitPosi);
    ((SN_GPIO0_Type           *) SN_GPIO0_BASE + (portNum*2000))->BCLR |= (1<<bitPosi);
  } else {
    //LPC_GPIO->SET[portNum] |= (0x1<<bitPosi);
    ((SN_GPIO0_Type           *) SN_GPIO0_BASE + (portNum*2000))->BSET |= (1<<bitPosi);
  }
#else
  switch (portNum) {
  case PORT0:
    if (bitVal == 1) {
      LPC_GPIO->SET[0] |= (0x1<<bitPosi);
    } else if (bitVal == 0) {
      LPC_GPIO->CLR[0] |= (0x1<<bitPosi);
    }
    break;
  case PORT1:
    if (bitVal == 1) {
      LPC_GPIO->SET[1] |= (0x1<<bitPosi);
    } else if (bitVal == 0) {
      LPC_GPIO->CLR[1] |= (0x1<<bitPosi);
    }
    break;
  default:
    break;
  }
#endif  
  return;
}


void GPIOSetFlexInterrupt(U32 channelNum, U32 portNum, U32 bitPosi, U32 sense, U32 event)
/*++

Function Description:
  Set interrupt sense, event, etc.
  sense: edge or level, 0 is edge, 1 is level
  event/polarity: 0 is active low/falling, 1 is high/rising.

Arguments:
  channelNum - numb of channel 0 - 7
  portNum      - GPIO port 0 or 1
  bitPosi         - GPIO numb: GPIO0_x or GPIO1_x
  sense          - edge or level, 0 edge, 1 level
  event          - event/polarity, 0 active low/falling, 1 active high/rising

Returns:
  NONE

--*/
{
#if 0  
#if 0 // Daniel modify to reduce code space, Pass test 2013.03.21
  if (portNum) {
    LPC_SYSCON->PINTSEL[channelNum] = bitPosi + 24;
  } else {
    LPC_SYSCON->PINTSEL[channelNum] = bitPosi;
  }
  NVIC_EnableIRQ((IRQn_Type)channelNum);
#else
  switch (channelNum) {
	case CHANNEL0:
    if (portNum) {
      LPC_SYSCON->PINTSEL[0] = bitPosi + 24;
    } else {
      LPC_SYSCON->PINTSEL[0] = bitPosi;
    }
    NVIC_EnableIRQ(FLEX_INT0_IRQn);
    break;
	case CHANNEL1:
    if (portNum) {
      LPC_SYSCON->PINTSEL[1] = bitPosi + 24;
    } else {
      LPC_SYSCON->PINTSEL[1] = bitPosi;
    }
    NVIC_EnableIRQ(FLEX_INT1_IRQn);
    break;
	case CHANNEL2:
    if (portNum) {
      LPC_SYSCON->PINTSEL[2] = bitPosi + 24;
    } else {
      LPC_SYSCON->PINTSEL[2] = bitPosi;
    }
    NVIC_EnableIRQ(FLEX_INT2_IRQn);
    break;
	case CHANNEL3:
    if (portNum) {
      LPC_SYSCON->PINTSEL[3] = bitPosi + 24;
    } else {
      LPC_SYSCON->PINTSEL[3] = bitPosi;
    }
    NVIC_EnableIRQ(FLEX_INT3_IRQn);
    break;
	case CHANNEL4:
	  if (portNum) {
      LPC_SYSCON->PINTSEL[4] = bitPosi + 24;
    } else {
      LPC_SYSCON->PINTSEL[4] = bitPosi;
    }
    NVIC_EnableIRQ(FLEX_INT4_IRQn);
    break;
	case CHANNEL5:
    if (portNum) {
      LPC_SYSCON->PINTSEL[5] = bitPosi + 24;
    } else {
      LPC_SYSCON->PINTSEL[5] = bitPosi;
    }
    NVIC_EnableIRQ(FLEX_INT5_IRQn);
    break;
	case CHANNEL6:
    if (portNum) {
      LPC_SYSCON->PINTSEL[6] = bitPosi + 24;
    } else {
      LPC_SYSCON->PINTSEL[6] = bitPosi;
    }
    NVIC_EnableIRQ(FLEX_INT6_IRQn);
    break;
	case CHANNEL7:
    if (portNum) {
      LPC_SYSCON->PINTSEL[7] = bitPosi + 24;
    } else {
      LPC_SYSCON->PINTSEL[7] = bitPosi;
    }
    NVIC_EnableIRQ(FLEX_INT7_IRQn);
    break;
  default:
    break;
  }
  #endif
  if (sense == 0) {
    LPC_GPIO_PIN_INT->ISEL &= ~(0x1<<channelNum);	        // Edge trigger 
    if (event == 0) {
      LPC_GPIO_PIN_INT->IENF |= (0x1<<channelNum);	      // faling edge 
    } else {
      LPC_GPIO_PIN_INT->IENR |= (0x1<<channelNum);	      // Rising edge 
    }
  } else {
    LPC_GPIO_PIN_INT->ISEL |= (0x1<<channelNum);	        // Level trigger
    LPC_GPIO_PIN_INT->IENR |= (0x1<<channelNum);	        // Level enable
    if (event == 0) {
      LPC_GPIO_PIN_INT->IENF &= ~(0x1<<channelNum);	      // active-low 
    } else {
      LPC_GPIO_PIN_INT->IENF |= (0x1<<channelNum);	      // active-high 
    }
  }
  return;
  #endif
}


#if 0 // Never be call, shut to save space
/*****************************************************************************
** Function name:		GPIOSetInterrupt
**
** Descriptions:		Set interrupt sense, event, etc.
**						sense: edge or level, 0 is edge, 1 is level 
**						event/polarity: 0 is active low/falling, 1 is high/rising.
**
** parameters:			channel #, sense, polarity(event)
** Returned value:		None
** 
*****************************************************************************/
void GPIOSetInterrupt(U32 ch_num, U32 sense, U32 event)
{
  if (sense == 0) {
    LPC_GPIO_PIN_INT->ISEL &= ~(0x1<<ch_num);	            // Edge trigger 
    if (event == 0) {
      LPC_GPIO_PIN_INT->IENF |= (0x1<<ch_num);	          // faling edge
    } else {
      LPC_GPIO_PIN_INT->IENR |= (0x1<<ch_num);	          // Rising edge
    } 
  } else {
    LPC_GPIO_PIN_INT->ISEL |= (0x1<<ch_num);	            // Level trigger
    if (event == 0) {
      LPC_GPIO_PIN_INT->IENF &= ~(0x1<<ch_num);	          // active-low interrupt enabled
    } else {
      LPC_GPIO_PIN_INT->IENF |= (0x1<<ch_num);	          // active-high interrupt enabled
    }
  }
  return;
}


/*****************************************************************************
** Function name:		GPIOIntEnable
**
** Descriptions:		Enable Interrupt Mask for a port pin.
**
** parameters:			channel num, event(0 is falling edge, 1 is rising edge)
** Returned value:		None
** 
*****************************************************************************/
void GPIOIntEnable( U32 ch_num, U32 event )
{
  if (event == 0) {
    LPC_GPIO_PIN_INT->SIENF = (0x1<<ch_num);	          // faling edge enabled	
  } else {
    LPC_GPIO_PIN_INT->SIENR = (0x1<<ch_num);	          // Rising edge enabled 
  } 
  return;
}
#endif



