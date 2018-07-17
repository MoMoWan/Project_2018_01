/*_____ I N C L U D E S ____________________________________________________*/
#include "LED.h"
#include "LED_Function.h"
#include "LED_RamSetting.h"
#include "LED_Const.h"
#include "LED_Table.h"

//#include "Mouse_RamSetting.h"
//#include "Mouse_Const.h"
#include "variables.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/

/*_____ D E F I N I T I O N S ______________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/

  COLOR color[3];
/*****************************************************************************
* Function		: MN_LEDState
* Description	: Select LED Effect
* Input			: None
* Output		: None		
* Return		: None
* Note			: None
*****************************************************************************/
U8 syncbreathCOLOR = 0xFF;
void MN_LEDState(void)
{
  U8 i =0 ,j=0,syncbreath = 0xFF, /*syncbreathCOLOR = 0xFF,*/ syncspect = 0xFF, syncreact = 0xFF;
//  COLOR color;
//  if(dbMS_LED_ReflashTimeFrame >= (8 + bLED_DataRefreshTime_Reload)/5){

//  dbMS_LED_ReflashTimeFrame = 0;
	if(wLED_Status & mskLED_ModeChange)							//LED Mode Change
	{
		wLED_Status &=~ mskLED_ModeChange;
    for (i =0; i < 3; i++) {
		  LED_Mode_ReInit(i);
    }
	}


  if (pollingChange) { 
    //for (i =0; i < 3; i++) {
    //  LED_Mode_Blink(i); 
    //} 
    LED_Mode_Blink(0);
//  if (++blinkCount >= 24) {
//    pollingChange = 0;
//    blinkCount =0;
//    for (i =0; i < 3; i++) {
//    bLED_Mode = user.profile[user.ProNO].led[i].effect;
//    }
//    wLED_Status |= mskLED_ModeChange;    
//  }    
  } else {
//    LED_Mode_Breath();
//    LED_Mode_Reaction();
//    LED_Mode_Spectrum();    
    for (i =0,j=1; i < 3; i++,j++ ) {
    LED_Mode_Breath(i);
    LED_Mode_Reaction(i);
    LED_Mode_Spectrum(i);        
     switch(user.profile[user.ProNO].led[i].effect) {
       case S_LED_MODE_BREATH:
         
         if (user.profile[user.ProNO].led[i].mode) { 
//           if (rBREATH_Tab_Index[i] == 0) {           
//             color[i].r = randomGenerate(0,255); 
//             color[i].g = randomGenerate(0,255);
//             color[i].b = randomGenerate(0,255);
              #if 0 // unsync  
//             if (rBREATH_Tab_Index[i] == 0) {   
              do {
                SN_WDT->FEED = 0x5AFA55AA;	
                randomcheck  = randomGenerate(0, 8-1);
              } while (randomcheck == scanindex);
              scanindex = randomcheck;
              color[i].r  = cnstMXXcolor[scanindex].r; //wave_color_buffer_R_1[(runlitlednumber+8)%24]; //temp[0][0];
              color[i].g = cnstMXXcolor[scanindex].g; //wave_color_buffer_G_1[(runlitlednumber+8)%24]; //temp[0][1];
              color[i].b  = cnstMXXcolor[scanindex].b; //wave_color_buffer_B_1[(runlitlednumber+8)%24]; //temp[0][2];
//            }
              #else  // sync 
//            if (rBREATH_Tab_Index[i] == 0) { 
//              if ((syncbreathCOLOR == 0xFF) || (syncbreathCOLOR == i)) {                
//                do {
//                  SN_WDT->FEED = 0x5AFA55AA;	
//                  randomcheck  = randomGenerate(0, 7);//51);//8-1);
//                } while (randomcheck == scanindex);
//                scanindex = randomcheck;
//               if (syncbreathCOLOR == 0xFF) 
//                 syncbreathCOLOR = i;                
//               }
//              }
//              color[i].r  = syncbreath == 0xFF ? cnstMXXcolor[scanindex].r : color[syncbreath].r; //wave_color_buffer_R_1[(runlitlednumber+8)%24]; //temp[0][0];
//              color[i].g  = syncbreath == 0xFF ? cnstMXXcolor[scanindex].g : color[syncbreath].g; //wave_color_buffer_G_1[(runlitlednumber+8)%24]; //temp[0][1];
//              color[i].b  = syncbreath == 0xFF ? cnstMXXcolor[scanindex].b : color[syncbreath].b; //wave_color_buffer_B_1[(runlitlednumber+8)%24]; //temp[0][2];  
              color[i].r  = colorTable[scanindex].r; // cnstMXXcolor[scanindex].r; //wave_color_buffer_R_1[(runlitlednumber+8)%24]; //temp[0][0];
              color[i].g  = colorTable[scanindex].g; // cnstMXXcolor[scanindex].g; //wave_color_buffer_G_1[(runlitlednumber+8)%24]; //temp[0][1];
              color[i].b  = colorTable[scanindex].b; // cnstMXXcolor[scanindex].b; //wave_color_buffer_B_1[(runlitlednumber+8)%24]; //temp[0][2];
              #endif
//           }             
         } else {
           color[i].r = user.profile[user.ProNO].led[i].color.r;
           color[i].g = user.profile[user.ProNO].led[i].color.g;
           color[i].b = user.profile[user.ProNO].led[i].color.b;           
         }
         #if 0 // unsync 
         bReload_MR[j*3]   =  (rPWM_BREATH_Value[i] * user.profile[user.ProNO].led[i].bright * color[i].r) >> 16;
         bReload_MR[j*3+1] =  (gPWM_BREATH_Value[i] * user.profile[user.ProNO].led[i].bright * color[i].g) >> 16;
         bReload_MR[j*3+2] =  (bPWM_BREATH_Value[i] * user.profile[user.ProNO].led[i].bright * color[i].b) >> 16; 
         #else  // sync 
         bReload_MR[j*3]   =  (rPWM_BREATH_Value[(syncbreath == 0xFF? i: syncbreath)] * user.profile[user.ProNO].led[i].bright * color[i].r) >> 16;
         bReload_MR[j*3+1] =  (gPWM_BREATH_Value[(syncbreath == 0xFF? i: syncbreath)] * user.profile[user.ProNO].led[i].bright * color[i].g) >> 16;
         bReload_MR[j*3+2] =  (bPWM_BREATH_Value[(syncbreath == 0xFF? i: syncbreath)] * user.profile[user.ProNO].led[i].bright * color[i].b) >> 16;          
         if (syncbreath == 0xFF) {
           syncbreath = i; 
         } else {
           rBREATH_Tab_Index[i] = rBREATH_Tab_Index[syncbreath] ;//== 0 ? 1 : rBREATH_Tab_Index[syncbreath];
           dbMS_LED_ReflashTimeFrame[S_LED_MODE_BREATH][i] = 0;
         }
         #endif       
         break;
       case S_LED_MODE_REACTION:
   //			LED_Mode_Reaction();
         #if 0 // unsync 
         bReload_MR[j*3]   =  (rPWM_REAT_Value[i] * user.profile[user.ProNO].led[i].bright * user.profile[user.ProNO].led[i].color.r) >> 16;
         bReload_MR[j*3+1] =  (gPWM_REAT_Value[i] * user.profile[user.ProNO].led[i].bright * user.profile[user.ProNO].led[i].color.g) >> 16;
         bReload_MR[j*3+2] =  (bPWM_REAT_Value[i] * user.profile[user.ProNO].led[i].bright * user.profile[user.ProNO].led[i].color.b) >> 16;
         #else  // sync 
         bReload_MR[j*3]   =  (rPWM_REAT_Value[(syncreact == 0xFF? i: syncreact)] * user.profile[user.ProNO].led[i].bright * user.profile[user.ProNO].led[i].color.r) >> 16;
         bReload_MR[j*3+1] =  (gPWM_REAT_Value[(syncreact == 0xFF? i: syncreact)] * user.profile[user.ProNO].led[i].bright * user.profile[user.ProNO].led[i].color.g) >> 16;
         bReload_MR[j*3+2] =  (bPWM_REAT_Value[(syncreact == 0xFF? i: syncreact)] * user.profile[user.ProNO].led[i].bright * user.profile[user.ProNO].led[i].color.b) >> 16;
         #endif
         if (syncreact == 0xFF) 
           syncreact = i;
         break;
       case S_LED_MODE_SPECTRUM:
   //			LED_Mode_Spectrum();
         #if 0 // unsync 
         bReload_MR[j*3]   =  rPWM_SPECT_Value[i] * user.profile[user.ProNO].led[i].bright / 255;
         bReload_MR[j*3+1] =  gPWM_SPECT_Value[i] * user.profile[user.ProNO].led[i].bright / 255;
         bReload_MR[j*3+2] =  bPWM_SPECT_Value[i] * user.profile[user.ProNO].led[i].bright / 255;  
         #else 
         bReload_MR[j*3]   =  rPWM_SPECT_Value[(syncspect == 0xFF? i: syncspect)] * user.profile[user.ProNO].led[i].bright / 255;
         bReload_MR[j*3+1] =  gPWM_SPECT_Value[(syncspect == 0xFF? i: syncspect)] * user.profile[user.ProNO].led[i].bright / 255;
         bReload_MR[j*3+2] =  bPWM_SPECT_Value[(syncspect == 0xFF? i: syncspect)] * user.profile[user.ProNO].led[i].bright / 255; 
         #endif 
         if (syncspect == 0xFF) 
           syncspect = i;
         break;
       case S_LED_MODE_STATIC:
   //			LED_Mode_Static();
         bReload_MR[j*3]   =  user.profile[user.ProNO].led[i].color.r * user.profile[user.ProNO].led[i].bright / 255; 
         bReload_MR[j*3+1] =  user.profile[user.ProNO].led[i].color.g * user.profile[user.ProNO].led[i].bright / 255;
         bReload_MR[j*3+2] =  user.profile[user.ProNO].led[i].color.b * user.profile[user.ProNO].led[i].bright / 255;
         break;
       case S_LED_MODE_NONE:
   //			LED_Mode_None();
         bReload_MR[j*3]   =  0; 
         bReload_MR[j*3+1] =  0;
         bReload_MR[j*3+2] =  0;
         break;   
//       case S_LED_MODE_BLINK:
//   //			LED_Mode_Blink();
//         bReload_MR[j*3]   =  rPWM_BLINK_Value * user.profile[user.ProNO].led[i].bright / 255;
//         bReload_MR[j*3+1] =  gPWM_BLINK_Value * user.profile[user.ProNO].led[i].bright / 255;
//         bReload_MR[j*3+2] =  bPWM_BLINK_Value * user.profile[user.ProNO].led[i].bright / 255;
//         break;     
       default:	
         break;
     }
   }
 } 
	LED_ReflashPWMDuty(); 
//}
  
//	switch(bLED_Mode)
//	{
//		case S_LED_MODE_BREATH:
//			LED_Mode_Breath();
//			break;
//		case S_LED_MODE_REACTION:
//			LED_Mode_Reaction();
//			break;
//		case S_LED_MODE_SPECTRUM:
//			LED_Mode_Spectrum();
//			break;
//		case S_LED_MODE_STATIC:
//			LED_Mode_Static();
//			break;
// 		case S_LED_MODE_NONE:
//			LED_Mode_None();
//			break;   
// 		case S_LED_MODE_BLINK:
//			LED_Mode_Blink();
//			break;     
//		default:	
//			break;
//	}
	
//	bMN_State = S_MAIN_IDLE_STATE;
	
}

/*****************************************************************************
* Function		: LED_Init
* Description	: LED_Init
* Input			: None
* Output		: None		
* Return		: None
* Note			: None
*****************************************************************************/
void LED_Init(void)
{
  uint8_t i;
//	LED_Mode_Breath_Init();
//	bLED_Mode = S_LED_MODE_BREATH;
	
//	LED_Mode_Spectrum_Init();
//	bLED_Mode = S_LED_MODE_SPECTRUM;							// LED Effect Init
//  flash_buttfer[1] = bLED_Mode = *((uint8_t *)MANUFACTURING_SATRT+1);
//  if (bLED_Mode == 0) {
//    bLED_Mode = 1;
//  }
//  bLED_Mode = ram.profile[0].led.effect;
  for (i =0; i < 3; i++) {
  LED_Mode_ReInit(i);
  }
	
//	LED_Mode_Reaction_Init();
//	bLED_Mode = S_LED_MODE_REACTION;
	
//	LED_Mode_Static_Init();
//	bLED_Mode = S_LED_MODE_STATIC;
}

/*****************************************************************************
* Function		: LED_Mode_ReInit
* Description	: LED_Mode_ReInit
* Input			: None
* Output		: None		
* Return		: None
* Note			: None
*****************************************************************************/
void LED_Mode_ReInit(uint8_t id)
{
//	uint8_t i;
	//PWM BUF Set To ZERO
//	for(i=0;i<6;i++)
//	{
//		rPWM_Buf[i] = 0x0;
//		rPWM_Buf[i] = 0x0;
//		rPWM_Buf[i] = 0x0;
//	}
//	dbMS_LED_ReflashTimeFrame = 0;
//	decode_DPI_Stage();
//	switch(bLED_Mode)
//	{
//		case S_LED_MODE_BREATH:
//			LED_Mode_Breath_Init();
//			break;
//		case S_LED_MODE_REACTION:
//			LED_Mode_Reaction_Init();
//			break;
//		case S_LED_MODE_SPECTRUM:
//			LED_Mode_Spectrum_Init();
//			break;
//		case S_LED_MODE_STATIC:
//    case S_LED_MODE_NONE:  
//			LED_Mode_Static_Init();
//			break;
//    case S_LED_MODE_BLINK:
//      LED_Mode_Blink_Init();
//      break;
//		default:	
//      bLED_Mode = S_LED_MODE_SPECTRUM;
//      LED_Mode_Spectrum_Init();
//			break;
//	}
LED_Mode_Breath_Init(id);
LED_Mode_Reaction_Init(id);
LED_Mode_Spectrum_Init(id);

}
/*****************************************************************************
* Function		: LED_Mode_Breath_Init
* Description	: LED_Mode_Breath_Init
* Input			: None
* Output		: None		
* Return		: None
* Note			: None
*****************************************************************************/
//void decode_DPI_Stage(void)
//{
//   switch (user.profile[user.ProNO].sensor.dpiStage) {
//     case 0 :
//		    bReload_MR[0] = 0xFF;
//		    bReload_MR[1] = 0;
//		    bReload_MR[2] = 0;       
//     break;
//     case 1 :
//		    bReload_MR[0] = 0xFF;
//		    bReload_MR[1] = 0xFF;
//		    bReload_MR[2] = 0;          
//     break; 
//     case 2 :
//		    bReload_MR[0] = 0;
//		    bReload_MR[1] = 0xFF;
//		    bReload_MR[2] = 0;          
//     break; 
//     case 3 :
//		    bReload_MR[0] = 0;
//		    bReload_MR[1] = 0xFF;
//		    bReload_MR[2] = 0xFF;          
//     break; 
//     case 4 :
//		    bReload_MR[0] = 0;
//		    bReload_MR[1] = 0;
//		    bReload_MR[2] = 0xFF;          
//     break;  
//     default :        
//     break;      
//   }
//}
/*****************************************************************************
* Function		: LED_Mode_Breath_Init
* Description	: LED_Mode_Breath_Init
* Input			: None
* Output		: None		
* Return		: None
* Note			: None
*****************************************************************************/
void LED_Mode_Breath_Init(uint8_t id)
{
  U8 i = 0;
	bLED_DataRefreshTime_Reload[S_LED_MODE_BREATH][id] = user.profile[user.ProNO].led[id].speed;//BREATH_REFRESH_TIME;
	rBREATH_Tab_Index[id] = 0;
//	LED_SettingDefaultColor();
  rPWM_BREATH_Value[id] = gPWM_BREATH_Value[id] = bPWM_BREATH_Value[id] = 0;
  for (i =0; i < 3; i++) {
    if (user.profile[user.ProNO].led[i].effect == user.profile[user.ProNO].led[id].effect) {
      if (i < id) {
        bLED_DataRefreshTime_Reload[S_LED_MODE_BREATH][id] = bLED_DataRefreshTime_Reload[S_LED_MODE_BREATH][i];        
      }
    } 
  }
  for (i =0; i < 3; i++) {
      bLED_DataRefreshTime_Reload[S_LED_MODE_BREATH][i] = bLED_DataRefreshTime_Reload[S_LED_MODE_BREATH][id];
      rBREATH_Tab_Index[i] = 0; 
  }    

  if (user.profile[user.ProNO].led[id].mode) { 
      if (rBREATH_Tab_Index[id] == 0) {           
        color[id].r = randomGenerate(0,255); 
        color[id].g = randomGenerate(0,255);
        color[id].b = randomGenerate(0,255); 
      }             
    }
//  } 
    syncbreathCOLOR = 0xFF;
}

/*****************************************************************************
* Function		: LED_Mode_Breath
* Description	: LED_Mode_Breath
* Input			: None
* Output		: None		
* Return		: None
* Note			: None
*****************************************************************************/
void LED_Mode_Breath(uint8_t id)
{
//	LED_ReflashPWMDuty();
if(dbMS_LED_ReflashTimeFrame[S_LED_MODE_BREATH][id] >= (8 + bLED_DataRefreshTime_Reload[S_LED_MODE_BREATH][id])){
  dbMS_LED_ReflashTimeFrame[S_LED_MODE_BREATH][id] = 0;
	LED_Effect_Breath(id);
}
}

/*****************************************************************************
* Function		: LED_Effect_Breath
* Description	: LED_Effect_Breath
* Input			: None
* Output		: None		
* Return		: None
* Note			: None
*****************************************************************************/
void LED_Effect_Breath(uint8_t id)
{
	
	if( rBREATH_Tab_Index[id] < BREATH_TABLE_LENGTH )
	{
		rPWM_BREATH_Value[id] = LED_BREATH_TABLE[rBREATH_Tab_Index[id]];	//(structLED_BreathEffect.bRed * LED_BREATH_TABLE[rTab_Index])>>8;	
		gPWM_BREATH_Value[id] = LED_BREATH_TABLE[rBREATH_Tab_Index[id]];	//(structLED_BreathEffect.bGreen * LED_BREATH_TABLE[rTab_Index])>>8;	
		bPWM_BREATH_Value[id] = LED_BREATH_TABLE[rBREATH_Tab_Index[id]++];	//(structLED_BreathEffect.bBlue * LED_BREATH_TABLE[rTab_Index++])>>8;		
	}
	else
	{
		rBREATH_Tab_Index[id] = 0;
//		LED_EffectRandomValueGen();
//		structLED_BreathEffect.bRed = *(RandomColor + (wLED_RandomValue &0xF));			//LED_R	
//		structLED_BreathEffect.bGreen = *(RandomColor + (wLED_RandomValue &0xF))>>8;	//LED_G
//		structLED_BreathEffect.bBlue = *(RandomColor + (wLED_RandomValue &0xF))>>16;	//LED_B
//	}
//    structLED_BreathEffect.bRed =   0xFF;//user.profile[0].savedLight[0].Chroma_effect.r;
//    structLED_BreathEffect.bGreen = 0xFF;//user.profile[0].savedLight[0].Chroma_effect.g;
//    structLED_BreathEffect.bBlue =  0xFF;//user.profile[0].savedLight[0].Chroma_effect.g;
//            if (rBREATH_Tab_Index[id] == 0) { 
              if ((syncbreathCOLOR == 0xFF) || (syncbreathCOLOR == id)) {                
                do {
                  SN_WDT->FEED = 0x5AFA55AA;	
                  randomcheck  = randomGenerate(0, 51);//8-1);
                } while (randomcheck == scanindex);
                scanindex = randomcheck;
               if (syncbreathCOLOR == 0xFF) 
                 syncbreathCOLOR = id;                
               }
//              }     
  }
  
//	for(i=1;i<6;i++)
//	{
//		rPWM_Buf[i] = rPWM_Value;
//		gPWM_Buf[i] = gPWM_Value;
//		bPWM_Buf[i] = bPWM_Value;
//	}
 
  
}

/*****************************************************************************
* Function		: LED_Mode_Spectrum_Init
* Description	: LED_Mode_Spectrum_Init
* Input			: None
* Output		: None		
* Return		: None
* Note			: None
*****************************************************************************/
void LED_Mode_Spectrum_Init(uint8_t id)
{
	rPWM_SPECT_Value[id] = PWM_MAX;
	gPWM_SPECT_Value[id] = 0;
	bPWM_SPECT_Value[id] = 0;
	bLED_DataRefreshTime_Reload[S_LED_MODE_SPECTRUM][id] = user.profile[user.ProNO].led[id].speed/5;//SPECTRUM_REFRESH_TIME;
	bLED_StepPWM = LED_SPECTRUM_PWM_STEP_VALUE;
	rSPECT_Tab_Index[id] = 0;
}

/*****************************************************************************
* Function		: LED_Mode_Spectrum
* Description	: LED_Mode_Spectrum
* Input			: None
* Output		: None		
* Return		: None
* Note			: None
*****************************************************************************/
void LED_Mode_Spectrum(U8 id)
{
//	uint8_t i;
	
//	LED_ReflashPWMDuty();
if(dbMS_LED_ReflashTimeFrame[S_LED_MODE_SPECTRUM][id] < (8 + bLED_DataRefreshTime_Reload[S_LED_MODE_SPECTRUM][id])){
   return;	
}
dbMS_LED_ReflashTimeFrame[S_LED_MODE_SPECTRUM][id] = 0;
	switch(rSPECT_Tab_Index[id])
	{
		case 0:			//-R+G Yellow-> Red
			LED_EffectSpectrumProcess(&rPWM_SPECT_Value[id], &gPWM_SPECT_Value[id], &rSPECT_Tab_Index[id],PWM_MAX);				
			break;
			//----------------------------------------------------------------
		
		case 1:			//-G+B		
			LED_EffectSpectrumProcess(&gPWM_SPECT_Value[id], &bPWM_SPECT_Value[id], &rSPECT_Tab_Index[id],PWM_MAX);
		break;
			//-----------------------------------------------------------

		case 2:			//-B+R				
			LED_EffectSpectrumProcess(&bPWM_SPECT_Value[id], &rPWM_SPECT_Value[id], &rSPECT_Tab_Index[id],PWM_MAX);			
		break;
		
		default:
			return;
	}
	
//	for(i=1;i<6;i++)
//	{
//		rPWM_Buf[i] = rPWM_Value;
//		gPWM_Buf[i] = gPWM_Value;
//		bPWM_Buf[i] = bPWM_Value;
//	}
}

/*****************************************************************************
* Function		: LED_EffectSpectrumProcess
* Description	: LED_EffectSpectrumProcess
* Input			: None
* Output		: None		
* Return		: None
* Note			: None
*****************************************************************************/
void LED_EffectSpectrumProcess(uint8_t* nNumber_Down, uint8_t* nNumber_UP,uint8_t* nRGB_Step,uint8_t nHalforMax)
{

	if((*nNumber_Down) != 0)
	{
		(*nNumber_Down) -= bLED_StepPWM;
	}
	
	if((*nNumber_UP) != nHalforMax)
	{
		(*nNumber_UP) += bLED_StepPWM;
	}
	else
	{
		(*nRGB_Step)++;
		if((*nRGB_Step) == 3)
		{
			(*nRGB_Step) = 0;
		}	
	}	
	
}

/*****************************************************************************
* Function		: LED_Mode_Reaction_Init
* Description	: LED_Mode_Reaction_Init
* Input			: None
* Output		: None		
* Return		: None
* Note			: None
*****************************************************************************/
void LED_Mode_Reaction_Init(uint8_t id)
{
//	if(bLED_Mode == S_LED_MODE_REACTION)
//		return;
	
	bLED_DataRefreshTime_Reload[S_LED_MODE_REACTION][id] = user.profile[user.ProNO].led[id].speed;//REACTION_REFRESH_TIME;
	if(bLED_ReativeJustMake == 0)
	{
		rREAT_Tab_Index[id] = 0;
		rPWM_REAT_Value[id] = gPWM_REAT_Value[id] = bPWM_REAT_Value[id] = 0;
	}
}

/*****************************************************************************
* Function		: LED_Mode_Reaction
* Description	: LED_Mode_Reaction
* Input			: None
* Output		: None		
* Return		: None
* Note			: None
*****************************************************************************/
void LED_Mode_Reaction(uint8_t id)
{
//	uint8_t i;

	uint16_t db_tabLED_BREACTIVE[80]=								//** Breath gamma table
	{
		  0,   0,   0,   3,   7,   11,   15,   19,   23,   27,   31,   35,  39,
		 43,  47,  50,  55,  58,  62,  70,  78,  86,  94,  101,	
		 109,  117,  125,  133,  141,  149,  156,  164,  172,  188,  196,
		 207,  219,  231,  243,  254,  266,  278,  290,  301,  313,  329,
		 345,  360,  376, 392, 407, 423, 439, 454, 470, 486, 494,
		513, 533, 552, 592, 611, 631, 650, 670, 690, 709, 730,
		750, 760, 780, 803, 827, 854, 882, 909, 937, 964, 988, 1024	
	};	
if(dbMS_LED_ReflashTimeFrame[S_LED_MODE_REACTION][id] < (8 + bLED_DataRefreshTime_Reload[S_LED_MODE_REACTION][id])){
   return;	
}
  dbMS_LED_ReflashTimeFrame[S_LED_MODE_REACTION][id] = 0;	
//	LED_ReflashPWMDuty();
	if(wLED_Status & mskLED_ReactionFlag)
	{	
		wLED_Status &= ~mskLED_ReactionFlag;	
		//** If bLED_ReativeJustMake == 0, reactive effect color change avalible.
		if(bLED_ReativeJustMake == 0)
		{
////			//Random color gen
////			LED_EffectRandomValueGen();
////			structLED_BreathEffect.bRed = *(RandomColor + (wLED_RandomValue &0xF));			//LED_R	
////			structLED_BreathEffect.bGreen = *(RandomColor + (wLED_RandomValue &0xF))>>8;	//LED_G
////			structLED_BreathEffect.bBlue = *(RandomColor + (wLED_RandomValue &0xF))>>16;	//LED_B	
//     structLED_BreathEffect.bRed =   user.profile[0].savedLight[0].Chroma_effect.r;
//     structLED_BreathEffect.bGreen = user.profile[0].savedLight[0].Chroma_effect.g;
//     structLED_BreathEffect.bBlue =  user.profile[0].savedLight[0].Chroma_effect.g;          
		}
		
		bLED_ReativeJustMake = 1;		
		rREAT_Tab_Index[id] = 79; //** Max index in the db_tabLED_BREATH table.
	}
	else
	{
		if(rREAT_Tab_Index > 0)
		{
			//**  Calculate color multiply gamma table2
			rPWM_REAT_Value[id]   = (*(db_tabLED_BREACTIVE + rREAT_Tab_Index[id]))>> 10; //** Div 1024, LED_R1 //(structLED_BreathEffect.bRed * (*(db_tabLED_BREACTIVE + rTab_Index)))>> 10; //** Div 1024, LED_R1 
			gPWM_REAT_Value[id]   = (*(db_tabLED_BREACTIVE + rREAT_Tab_Index[id]))>> 10; //** Div 1024, LED_G1//(structLED_BreathEffect.bGreen * (*(db_tabLED_BREACTIVE + rTab_Index)))>> 10; //** Div 1024, LED_G1
			bPWM_REAT_Value[id]   = (*(db_tabLED_BREACTIVE + rREAT_Tab_Index[id]))>> 10; //** Div 1024, LED_B1	//(structLED_BreathEffect.bBlue * (*(db_tabLED_BREACTIVE + rTab_Index)))>> 10; //** Div 1024, LED_B1	
			rREAT_Tab_Index[id]--;	
			
			//** To avoid system run the reactive of "Black Color"  
			if((rPWM_REAT_Value ==0) && (gPWM_REAT_Value == 0) && (bPWM_REAT_Value == 0))	
			{
				//**if R/G/B PWM=0 , leaving current path  
				rREAT_Tab_Index[id] = 0;
			}
			
			//** when LED dimming step match to Gamma table brightness [step40]
			//** clear bLED_ReativeJustMake flag and next key make will exchange LED color.
			if((rREAT_Tab_Index[id]<40) && (bLED_ReativeJustMake == 1))
			{	
				bLED_ReativeJustMake = 0;			
			}
		}				
	}	

//	for(i=1;i<6;i++)
//{
//		rPWM_Buf[i] = rPWM_Value;
//		gPWM_Buf[i] = gPWM_Value;
//		bPWM_Buf[i] = bPWM_Value;
//}
}

/*****************************************************************************
* Function		: LED_Mode_Wave_Init
* Description	: LED_Mode_Wave_Init
* Input			: None
* Output		: None		
* Return		: None
* Note			: None
*****************************************************************************/
void LED_Mode_Wave_Init(void)
{
	
//	bLED_DataRefreshTime_Reload = WAVE_REFRESH_TIME;
	
//		rPWM_Buf[i] = gamma32[bLED_Rbuf[i]];
//		gPWM_Buf[i] = gamma32[bLED_Gbuf[i]];
//		bPWM_Buf[i] = gamma32[bLED_Bbuf[i]];		
	
}

/*****************************************************************************
* Function		: LED_Mode_Wave
* Description	: LED_Mode_Wave
* Input			: None
* Output		: None		
* Return		: None
* Note			: None
*****************************************************************************/
void LED_Mode_Wave(void)
{	

}

/*****************************************************************************
* Function		: LED_Mode_Static_Init
* Description	: LED_Mode_Static_Init
* Input			: None
* Output		: None		
* Return		: None
* Note			: None
*****************************************************************************/
void LED_Mode_Static_Init(uint8_t id)
{
	bLED_DataRefreshTime_Reload[S_LED_MODE_STATIC][id] = user.profile[user.ProNO].led[id].speed;//STATIC_REFRESH_TIME;
}
/*****************************************************************************
* Function		: LED_Mode_Blink_Init
* Description	: LED_Mode_Blink_Init
* Input			: None
* Output		: None		
* Return		: None
* Note			: None
*****************************************************************************/
void LED_Mode_Blink_Init(uint8_t id)
{
	bLED_DataRefreshTime_Reload[S_LED_MODE_NONE][id] = BLINK_REFRESH_TIME;
}
/*****************************************************************************
* Function		: LED_Mode_Static
* Description	: LED_Mode_Static
* Input			: None
* Output		: None		
* Return		: None
* Note			: None
*****************************************************************************/
void LED_Mode_Static(U8 id)
{
//	LED_ReflashPWMDuty();
	LED_PWMBuf_Update(user.profile[0].led[id].color.r,user.profile[0].led[id].color.g,user.profile[0].led[id].color.b);			// White
}
/*****************************************************************************
* Function		: LED_Mode_Static
* Description	: LED_Mode_Static
* Input			: None
* Output		: None		
* Return		: None
* Note			: None
*****************************************************************************/
void LED_Mode_None(void)
{
	LED_ReflashPWMDuty();
	LED_PWMBuf_Update(0x0,0x0,0x0);			// White
}
/*****************************************************************************
* Function		: LED_Mode_Static
* Description	: LED_Mode_Static
* Input			: None
* Output		: None		
* Return		: None
* Note			: None
*****************************************************************************/
void LED_Mode_Blink(uint8_t id)
{
   U8 i = 0;
if(dbMS_LED_ReflashTimeFrame[S_LED_MODE_NONE][id] < (8 + bLED_DataRefreshTime_Reload[S_LED_MODE_NONE][id])){
   return;	
}
dbMS_LED_ReflashTimeFrame[S_LED_MODE_NONE][id] = 0;  
  
  if ((blinkCount & 0x01) == 0) {
    switch (pollingChange) {
      case 1:
        LED_PWMBuf_Update(0xFF,0,0);			// red 
        break;
      case 2:
        LED_PWMBuf_Update(0,0,0xFF);			// blue
        break;
      case 3:
        LED_PWMBuf_Update(0,0xFF,0);			// green
        break;
      case 4:
        LED_PWMBuf_Update(0xFF,0xFF,0xFF);			// White
        break; 
      default:
        break;          
    }
  } else {
    LED_PWMBuf_Update(0,0,0);			// off
  }
//	LED_ReflashPWMDuty();
//  if (id == 2) {
//    ++blinkCount;
//  } 
//  if (blinkCount >= 24) {
  if (++blinkCount >= 24) {
    pollingChange = 0;
    blinkCount =0;
//    bLED_Mode = user.profile[user.ProNO].led[id].effect;
    for (i =0; i < 3; i++) {
      bLED_Mode = user.profile[user.ProNO].led[i].effect;
    }
    wLED_Status |= mskLED_ModeChange;    
  } 
}
/*****************************************************************************
* Function		: LED_PWMBuf_Update
* Description	: LED_PWMBuf_Update
* Input			: None
* Output		: None		
* Return		: None
* Note			: None
*****************************************************************************/
void LED_PWMBuf_Update(uint8_t rpwm_buf,uint8_t gpwm_buf,uint8_t bpwm_buf)
{
	uint8_t i;
	for(i=1;i<6;i++)
	{
//		rPWM_Buf[i] = rpwm_buf;
//		gPWM_Buf[i] = gpwm_buf;
//		bPWM_Buf[i] = bpwm_buf;
     bReload_MR[i*3]   = rpwm_buf; 
     bReload_MR[i*3+1] = gpwm_buf; 
     bReload_MR[i*3+2] = bpwm_buf; 
	}
 
}

/*****************************************************************************
* Function		: LED_SettingDefaultColor
* Description	: LED_SettingDefaultColor
* Input			: None
* Output		: None		
* Return		: None
* Note			: None
*****************************************************************************/
void LED_SettingDefaultColor(void)
{
	structLED_BreathEffect.bRed =   user.profile[0].led[0].color.r;
	structLED_BreathEffect.bGreen = user.profile[0].led[0].color.g;
	structLED_BreathEffect.bBlue =  user.profile[0].led[0].color.b;
}

/*****************************************************************************
* Function		: LED_ReflashPWMDuty
* Description	: LED_ReflashPWMDuty
* Input			: None
* Output		: None		
* Return		: None
* Note			: None
*****************************************************************************/
void LED_ReflashPWMDuty(void)
{
//	uint8_t i;
//	for(i=1;i<6;i++)
//	{
//		bReload_MR[i*3]   = rPWM_Buf[i] * user.profile[0].savedLight[0].level / 255;
//		bReload_MR[i*3+1] = gPWM_Buf[i] * user.profile[0].savedLight[0].level / 255;
//		bReload_MR[i*3+2] = bPWM_Buf[i] * user.profile[0].savedLight[0].level / 255;
//	}
// 	SN_CT16B1->MR3 = ledPwmData[0].r;
//	SN_CT16B1->MR4 = ledPwmData[0].g;
//	SN_CT16B1->MR5 = ledPwmData[0].b;
	//DPI LED
	SN_CT16B1->MR0 =  user.profile[user.ProNO].sensor.stageColor[user.profile[user.ProNO].sensor.dpiStage].r;//DPICOLOR[user.profile[user.ProNO].sensor.dpiStage].r;//ledPwmData[1].r;
	SN_CT16B1->MR1 =  user.profile[user.ProNO].sensor.stageColor[user.profile[user.ProNO].sensor.dpiStage].g;//DPICOLOR[user.profile[user.ProNO].sensor.dpiStage].g;//ledPwmData[1].g;
	SN_CT16B1->MR2 =  user.profile[user.ProNO].sensor.stageColor[user.profile[user.ProNO].sensor.dpiStage].b;//DPICOLOR[user.profile[user.ProNO].sensor.dpiStage].b;//ledPwmData[1].b;
//	//Logo LED
//	SN_CT16B1->MR21 = ledPwmData[2].r;
//	SN_CT16B1->MR22 = ledPwmData[2].g;
//	SN_CT16B1->MR23 = ledPwmData[2].b;	
//	//LED1
//	SN_CT16B1->MR11 = ledPwmData[3].r;
//	SN_CT16B1->MR19 = ledPwmData[3].g;
//	SN_CT16B1->MR20 = ledPwmData[3].b;
//	//LED2
//	SN_CT16B1->MR14 = ledPwmData[4].r;
//	SN_CT16B1->MR15 = ledPwmData[4].g;
//	SN_CT16B1->MR16 = ledPwmData[4].b;
//	//LED3
//	SN_CT16B1->MR17 = ledPwmData[5].r;
//	SN_CT16B1->MR18 = ledPwmData[5].g;
//	SN_CT16B1->MR10 = ledPwmData[5].b;  
	//RollerLED
	SN_CT16B1->MR3 = bReload_MR[3];
	SN_CT16B1->MR4 = bReload_MR[4];
	SN_CT16B1->MR5 = bReload_MR[5];
	//DPI LED
//	SN_CT16B1->MR0 = bReload_MR[0];
//	SN_CT16B1->MR1 = bReload_MR[1];
//	SN_CT16B1->MR2 = bReload_MR[2];
	//Logo LED
	SN_CT16B1->MR21 = bReload_MR[6];
	SN_CT16B1->MR22 = bReload_MR[7];
	SN_CT16B1->MR23 = bReload_MR[8];	
	//LED1
	SN_CT16B1->MR17 = SN_CT16B1->MR14 = SN_CT16B1->MR11 = bReload_MR[9];
	SN_CT16B1->MR18 = SN_CT16B1->MR15 = SN_CT16B1->MR19 = bReload_MR[10];
	SN_CT16B1->MR10 = SN_CT16B1->MR16 = SN_CT16B1->MR20 = bReload_MR[11];
	//LED2
//	SN_CT16B1->MR14 = bReload_MR[12];
//	SN_CT16B1->MR15 = bReload_MR[13];
//	SN_CT16B1->MR16 = bReload_MR[14];
//	//LED3
//	SN_CT16B1->MR17 = bReload_MR[15];
//	SN_CT16B1->MR18 = bReload_MR[16];
//	SN_CT16B1->MR10 = bReload_MR[17];  
}

