#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_
#include "type.h"

#define KEY_IN    (1<<0)
#define MACRO_IN  (1<<1)
#define TURBO_IN  (1<<2)
typedef __packed struct _KEY_HID_IN_{
	U8 keyIn[8];               // Key hid in data
	U8 macroIn[8];             // Macro hid in data
	U8 turboIn[8];             // Turbo hid in data  
	U8 numbers;                // Total numbers of active key even
}KEY_HID_IN;

extern KEY_HID_IN hid_key;
void fillKeyHid(U8* epBuf,U8 param);
#endif
