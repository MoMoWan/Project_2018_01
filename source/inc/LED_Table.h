
#ifndef __LED_TABLE_H__
#define __LED_TABLE_H__
#include <SN32F240B.h>
#include "LED_Const.h"

extern const uint32_t RandomColor[16];

#define BREATH_TABLE_LENGTH	140
extern const uint8_t LED_BREATH_TABLE[];

extern const uint8_t LED_LIGHT_PARAM[];

#endif //__LED_TABLE_H__


