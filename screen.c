#include "osdefs.h"
#include "asm.h"
#include "screen.h"
#include "stdarg.h"

#define VGA_WIDTH  (80*2)
#define VGA_HEIGHT 25

//显存起始地址
static uint8_t* screen_buf = (uint8_t*)0xb8000;

// 当前光标位置 x行y列
static int cur_x, cur_y;

/* 以下4个函数复制自 osdev */

void enable_cursor(uint8_t cursor_start, uint8_t cursor_end)
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);
	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

void disable_cursor()
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

// It would be faster to instead only update it after printing an entire string.
void update_cursor(int x, int y)
{
    // 对应到显存的位置需要除以2
	uint16_t pos = x * (VGA_WIDTH / 2) + y / 2;
	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}
//pos = y * VGA_WIDTH + x.
//To obtain the coordinates, just calculate: y = pos / VGA_WIDTH; x = pos % VGA_WIDTH;.
uint16_t get_cursor_position(void)
{
    uint16_t pos = 0;
    outb(0x3D4, 0x0F);
    pos |= inb(0x3D5);
    outb(0x3D4, 0x0E);
    pos |= ((uint16_t)inb(0x3D5)) << 8;
    return pos;
}

/* 接下来应该会有非常多的bug */

// 初始化光标位置和显存
void init_scr() {
    cur_x = 1, cur_y = 0;
    clsscr();
    // 参数意为让光标的显示范围为第x像素行到第y像素行，一行有16个像素，此为仅显示最后一行的像素点
    enable_cursor(15, 16);
}

// 清空显存
void clsscr() {
    for (int i = 0; i < VGA_HEIGHT * VGA_WIDTH; i += 2) {
        //screen_buf[i] = '\0';
        screen_buf[i+1] = Black<<4 | Gray;
    }
}

// 移动到x行y列
void moveto(int x, int y) {
    cur_y = ABS(y);
    if (cur_y >= VGA_WIDTH) { // 换行
        cur_y %= VGA_WIDTH;
        x++;
    }
    if (ABS(x) == VGA_HEIGHT) { // 判断是否最后一行
        cur_x = VGA_HEIGHT - 1;
        screen_scroll_up(1);  // 其他内容向上滚动一行
    } else {
        cur_x = ABS(x);
    }
    update_cursor(cur_x, cur_y);
}

void screen_scroll_up(int line) {
    int src = line * VGA_WIDTH, dst = 0;
    for (; src < VGA_HEIGHT * VGA_WIDTH; src++, dst++) {
        screen_buf[dst] = screen_buf[src];
    }
    while (dst < VGA_HEIGHT * VGA_WIDTH) {
        screen_buf[dst] = '\0';
        screen_buf[dst+1] = Black;
        dst += 2;
    }
}

// 第一字节为ascii码 第二字节低4位前景色高3位背景色
void putchar_color(uint8_t c, enum Color fcolor, enum Color bcolor) {
    if (c == '\n') { // 换行
        moveto(cur_x+1, 0);
        return;
    }

    int pos = cur_y+cur_x*VGA_WIDTH;
    screen_buf[pos] = c;
    screen_buf[pos+1] = bcolor<<4|fcolor;
    moveto(cur_x, cur_y+2);
}

void putchar(uint8_t c) {
    putchar_color(c, Gray, Black);
}

// flag 控制是否换行
void puts_nl(uint8_t* s, int flag) {
    for (; *s; s++) {
        putchar(*s);
    }
    if (flag) {
        moveto(cur_x+1, 0);
    }
}

void puts(uint8_t* s) {
    puts_nl(s, TRUE);
}

void puts_color(uint8_t* s, enum Color fcolor, enum Color bcolor) {
    for (; *s; s++) {
        putchar_color(*s, fcolor, bcolor);
    }
    moveto(cur_x+1, 0);
}

// 将num转成base进制的字符串然后输出 应该只会用到2, 10, 16. 最大0xffffffff
void putint(unsigned int num, int base) {
    const char *tlb = "0123456789abcdef";
    char res[50];
    res[49] = '\0';
    int i = 48;

    if (num == 0) {
        putchar('0');
    }
    while (num) {
        res[i--] = tlb[num%base], num /= base;
    }
    puts_nl(res+i+1, FALSE);
}

// 简化版的printf
int printf(uint8_t* fmt, ...) {
    va_list arg;
    va_start(arg,fmt);
    while (*fmt != '\0') {
        if (*fmt != '%') {
            putchar(*fmt);
        } else {
            fmt++;
            switch (*fmt) {
            case 'c':
                putchar(va_arg(arg, int));
                break;
            case 's':
                puts_nl(va_arg(arg, char*), FALSE);
                break;
            case 'd':
                putint(va_arg(arg, int), 10);
                break;
            case 'x':
                puts_nl("0x", FALSE);
                putint(va_arg(arg, int), 16);
                break;
            case 'b': // 标准的printf没有这个
                puts_nl("0b", FALSE);
                putint(va_arg(arg, int), 2);
                break;
            case '%':
                putchar('%');
                break;
            default:
                break;
            }
        }
        fmt++;
    }
    va_end(arg);
}