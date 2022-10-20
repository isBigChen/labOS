#ifndef _USER_LIB_H_
#define _USER_LIB_H_

#include <osdefs.h>

int printf(uint8_t* fmt, ...);
void putchar(uint8_t c);
void putchar_color(uint8_t c, enum Color fColor, enum Color bColor);
void puts_color(uint8_t* s, enum Color fColor, enum Color bColor);
int _sys_clock();
void sleep(int ms);
void moveto(int row, int col);
uint8_t getch();
uint8_t getchar();
void gets(char *s);
void clsscr();
void switch_enable(bool enable);
int add_task(uint32_t entry);
int strcmp(const char *p1, const char *p2);
void write_screen(uint8_t p, uint8_t c);
#endif