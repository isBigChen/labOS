#ifndef _SCREEN_H_
#define _SCREEN_H_

#include <osdefs.h>

void enable_cursor(uint8_t cursor_start, uint8_t cursor_end);
void disable_cursor();
void update_cursor(int x, int y);
uint16_t get_cursor_position(void);

void kern_init_scr();
void kern_clsscr();
void kern_moveto(int x, int y);
void kern_putchar_color(uint8_t c, enum Color fcolor, enum Color bcolor);
void kern_putchar(uint8_t c);
void kern_puts(uint8_t* s);
void kern_puts_nl(uint8_t* s, int flag);
void kern_puts_color(uint8_t* s, enum Color fcolor, enum Color bcolor);
void kern_putint(unsigned int num, int base);
int kern_printf(uint8_t* fmt, ...);

#endif