#ifndef _USER_LIB_H_
#define _USER_LIB_H_

#include <osdefs.h>

int printf(uint8_t* fmt, ...);
void putchar(uint8_t c);
uint32_t _sys_clock();
void sleep(uint32_t ms);
void moveto(int row, int col);
uint8_t getch();
void puts(uint8_t* s);
#endif