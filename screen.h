#include "osdefs.h"

#ifndef _SCREEN_H_
#define _SCREEN_H_

void enable_cursor(uint8_t cursor_start, uint8_t cursor_end);
void disable_cursor();
void update_cursor(int x, int y);
uint16_t get_cursor_position(void);

void init_scr();
void clsscr();
void moveto(int x, int y);
void putchar_color(uint8_t c, enum Color fcolor, enum Color bcolor);
void putchar(uint8_t c);
void puts(uint8_t* s);
void puts_nl(uint8_t* s, int flag);
void puts_color(uint8_t* s, enum Color fcolor, enum Color bcolor);
void putint(unsigned int num, int base);
int printf(uint8_t* fmt, ...);

#endif
