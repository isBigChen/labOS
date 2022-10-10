#ifndef _MEM_H_
#define _MEM_H_

#include "osdefs.h"

void memset(uint8_t* addr, uint8_t val, int offset);
void memcpy(uint8_t* src, uint8_t* dst, int size);

#endif