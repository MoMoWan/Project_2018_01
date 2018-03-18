#ifndef _PROFILE_H_
#define _PROFILE_H_
#include "type.h"
#include "led_driver.h"
#include "button.h"

#define PROFILE_UP    0x01
#define PROFILE_DOWN  0x02
#define PROFILE_NUM 0x03
bool  newProfile(U8 id);
bool deleteProfile(U8 id);
U8 getProfileNumber(void);
void loadProfileLed(void);
bool switchProfile(U8 type,U8 num);
bool getProfileName(U8 id,U8* totalSize,U16 offset,U8* pData,U8 size);
U8 setProfileName(U8 id,U16 totalSize,U16 offset,U8* pData,U8 size);
void profileBtnTest(U8 num);
void checkAllProfileName(void);
#endif
