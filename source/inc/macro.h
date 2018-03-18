#ifndef _MACRO_H_
#define _MACRO_H_
#include "main.h"
#include "storage.h"


#define MAX_MACRO    (100)
#define MAX_ACTIVE_MACRO NUMBER_OF_BUTTONS//(10)
#define MACRO_BUFFER_SIZE (64)

#define B2W(h,l)       ((h<<8)|(l)) // Make two  bytes to a word       
#define B2DW(h,hl,l,ll) ((h<<24)|(hl<<16)|(l<<8)|ll)  // Make four bytes to double word


//Macro Event IDs
#define BUTTON             0x01            // Mouse button  make
#define CURSOR             0x02            // Mouse Curser,X and Y
#define KEY_MAKE           0x03            // Keyboard key pressed
#define KEY_BREAK          0x04            // Keyboard Key released 
#define POWER_MAKE         0x05            // Power Key pressed
#define POWER_BREAK        0x06            // Power key released
#define MEDIA_MAKE         0x07            // Media key make
#define MEDIA_BREAK        0x08            // Media key released

#define SCROLL_WHEEL       0x10            // Mouse button  released
#define DELAY_1            0x20            // Delay(rang=1ms-255ms)
#define DELAY_2            0x21            // Delay(rang=1ms-1.05 minutes)
#define DELAY_3            0x22            // Delay(rang=1ms-4.66 hours)
#define DELAY_4            0x23            // Delay(rang=1ms-49.7 days)

//Macro key information
#define MAX_KEY   0x08
#define MACRO_KEY_EVEN    (1<<0)
#define MACRO_POWER_EVEN  (1<<1)
#define MACRO_MEDIA_EVEN  (1<<2)
#define MACRO_BUTTON_EVEN (1<<3)
#define MACRO_SCROLL_EVEN (1<<4)



#define MACRO_MAX_NUMBERS    (100)

#define ID_NA                (0)

#define RETURN_FAIL      (0)
#define RETURN_WAIT      (2)
#define RETURN_SUCCESS   (1)

#define WAIT_NONE        (0)
#define WAIT_ONE         (1)
#define WAIT_TWO         (2)
#define WAIT_THREE       (3)

#define MACRO_EP1_SIZE   (8)
#define MACRO_EP2_SIZE   (16)
#define MACRO_EP3_SIZE   (8+1)

typedef __packed struct _ACTIVE_MACRO_{
	U16 id;        // 
//	U8 buttonIdx;  // the index of button ,indicate current macro 's button
	U8 type;       // 1:  Run the ID macro "0xNN" times  2.Repeat the ID macro until button is released. 3.Repeat the ID macro until button is pressed again.
	U8 repeatTimes;  // Repeat timers
	U8 count;      // Count every finish count
	U8 flag;       // active or not
	U32 timer;     // timer to control time
	U32 miniDelay;     // Delay between evens
	U32 offset;    // Offset of macro data	
	U32 popIndex;  // pop buffer index
	U32 pushIndex; // push buffer index
  U32 readcount; // Total count	
	U32 popCount;  // Total pop count
	U8  bufferFilled;// The numbers of filled buffer bytes
	U32 dataAddr;    // Data address
	U32 dataSize;    // Data size
	U8  stepFlag;        // 
}ACTIVE_MACRO;

typedef __packed struct _BUTTON_TO_MACRO_{
	U8 active;
	U8 activeIndex;
}BUTTON_TO_MACRO;

typedef __packed struct _MACRO_{
	__packed struct{
		U16  id    :15;
		U16  error :1;
	}id;
	//U16 id;                 // ID 
	U16 dataBlockAddr;                 // Macro     
  U32 dataSize;
  U16 nameBlockAddr;
  U32 nameSize;     	
}MACRO;
typedef __packed struct _MACRO_TABLE_{
	MACRO macro[MACRO_MAX_NUMBERS];
	U16 macroNumbers;                       // Total numbers of the macro
//	BLOCK_ASSIGN block;
	U8 macroMiniDelay;
	U32 recycleSizes;
	U16 crc;
}MACRO_TABLE;

typedef __packed struct _ERASE_FLASH_{
	bool dataEraseFlag;
	U16 dataBlockStart;
	U16 dataBlockNumbers;
	bool nameEraseFlag;
	U16 nameBlockStart;
	U16 nameBlockNumbers;	
}ERASE_FLASH;
extern U8 macroEven;
extern U8 macroKeyQueue[MAX_KEY];
extern ACTIVE_MACRO macroActive[MAX_ACTIVE_MACRO];
extern U8 macroButton;          //Backup the Mouse button state from macro data
extern U8 macroScroll;          // Backup the scroll data from macro data
extern S16 macroXCursor;
extern S16 macroYCursor;
extern U8 memoryManageStaue;                                 // 	Macro Memory Management 
extern U8 macroMemoryCheck;                                  // Check the macro memory 
extern MACRO_TABLE macroTable;
extern bool protocolWait;                          // 
extern ERASE_FLASH exterFlashErase;                          // Manage the exter flash erase operation

extern U8  Macro_Ep1_In[MACRO_EP1_SIZE];                        // Macro end point 1 in 
extern U8  Macro_Ep2_In[MACRO_EP2_SIZE];                        // Macro end point 2 in 
extern U8  Macro_Ep3_In[MACRO_EP3_SIZE];                        // Macro end point 3 in 

void activeMacro(U8 type,U8 btnIdx,U16 id,U8 times);
void macroProcess(void);
void deActiveMacro(U8 idx);
U8 buttonIsRunning(U8 btnIdx);
void resetRepeattimes(U8 btnIdx);
bool setMallocMacro(U16 id, U32 size);
bool getMallocMacro(U16 id,U8* pData);
U8 setMacroData(U16 id,U32 offset,U8*pData,U8 size);
U8 setMacroName(U16 id,U16 totalSize,U16 offset,U8* pData,U8 size);
bool getMacroData(U16 id,U32 offset,U8* pData,U8 size);
bool getMacroName(U16 id,U8* totalSize,U32 offset,U8* pData,U8 size);
U8 macroDelete(U16 id);
void setMacroMinDelay(U8 delay);
U8 getMacroMinDelay(void);
void getMacroList(U16 offset,U8* totalSize,U8 *pData,U8 size);
bool checkMacroCRC(U16 id);
void checkMacroDataCRC(void);
U16 getMacroNumber(void);
void macroEngine(void);
void GetRamDataDump(U32 addr,U8* pData,U8 size);
void checkMacroPowerUp(void);
void macroStepGo(U8 btnIdx);
void stopHIDeven(void);
#endif
