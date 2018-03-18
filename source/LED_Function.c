
#include "LED_Function.h"
#include "LED_RamSetting.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/

/*_____ D E F I N I T I O N S ______________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/


/*****************************************************************************
* Function		: LED_AllEnIO_SetOutputLow
* Description	: IO SetputLow -> LED Light
* Input			: None
* Output		: None		
* Return		: None
* Note			: None
*****************************************************************************/
void LED_AllEnIO_SetOutputLow(void)
{
	//EN0
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
	
	/*
	SN_GPIO3->MODE |= 0xF00;
	SN_GPIO3->BCLR |= 0xF00;
	*/
}

/*****************************************************************************
* Function		: LED_AllEnIO_SetOutputHighvoid
* Description	: IO Setputhigh -> LED Darken
* Input			: None
* Output		: None		
* Return		: None
* Note			: None
*****************************************************************************/
void LED_AllEnIO_SetOutputHigh(void)
{
	//EN0
	SN_GPIO3->MODE_b.MODE11 = 1;			
	SN_GPIO3->DATA_b.DATA11 = 1;
	//EN1
	SN_GPIO3->MODE_b.MODE10 = 1;			
	SN_GPIO3->DATA_b.DATA10 = 1;
	//EN2
	SN_GPIO3->MODE_b.MODE9 = 1;			
	SN_GPIO3->DATA_b.DATA9 = 1;
	//EN3
	SN_GPIO3->MODE_b.MODE8 = 1;			
	SN_GPIO3->DATA_b.DATA8 = 1;
	
	/*
	SN_GPIO3->MODE |= 0xF00;
	SN_GPIO3->BSET |= 0xF00;
	*/
}

	
/*****************************************************************************
* Function		: LED_EffectRandomValueGen
* Description	: Generation random value
* Input(Global)	: bLED_RandomValue, bLED_RandomValue_N_1, bLED_RandomValue_N_2
* Input(Local)  : None
* Output(Global): None
* Return(Local)	: None
*****************************************************************************/
void LED_EffectRandomValueGen(void)
{
	//** F[n] = F[n-1] + F[n-2];
	wLED_RandomValue = wLED_RandomValue_N_1 + wLED_RandomValue_N_2;
	//** Recheck new value different than Before
	while( ((wLED_RandomValue & 0x0F) == (wLED_RandomValue_N_1&0x0F)) || ((wLED_RandomValue & 0x0F) == (wLED_RandomValue_N_2&0x0F)) )
	{		
		//** Save new F[n-1], F[n-2] 
		wLED_RandomValue_N_2 = wLED_RandomValue_N_1;	
		wLED_RandomValue_N_1 = wLED_RandomValue;
		//** F[n] = F[n-1] + F[n-2];
		wLED_RandomValue = wLED_RandomValue_N_1 + wLED_RandomValue_N_2;
	}	
	
	//** Save new F[n-1], F[n-2] 
	wLED_RandomValue_N_2 = wLED_RandomValue_N_1;
	wLED_RandomValue_N_1 = wLED_RandomValue; 	
		
}


