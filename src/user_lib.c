#include <user_lib.h>
#include <stdarg.h>
#include <osdefs.h>

int _sys_clock() {
    int current_clock;
    asm volatile("int $0x80" : "=b" (current_clock) : "a" (sys_clock));
    return current_clock;
}

static inline void _sys_putchar(uint8_t c) {
    asm volatile("int $0x80" :: "a" (sys_putchar), "b" (c));
}

static inline void _sys_putchar_color(uint8_t c, uint8_t fColor, uint8_t bColor) {
    asm volatile("int $0x80" :: "a" (sys_putchar_color), "b" (c), "c" (fColor), "D" (bColor));
}

static inline void _sys_puts_nl(uint8_t *s, int flag) {
    asm volatile("int $0x80" :: "a" (sys_puts_nl), "b" (s), "c" (flag));
}

static inline void _sys_putint(unsigned int num, int base) {
    asm volatile("int $0x80" :: "a" (sys_putint), "b" (num), "c" (base));
}

static inline uint8_t _sys_getch() {
    uint8_t c;
    asm volatile("int $0x80" : "=b" (c) : "a" (sys_getch));
    return c;
}

int printf(uint8_t* fmt, ...) {
    va_list arg;
    va_start(arg,fmt);
    while (*fmt != '\0') {
        if (*fmt != '%') {
            _sys_putchar(*fmt);
        } else {
            fmt++;
            switch (*fmt) {
            case 'c':
                _sys_putchar(va_arg(arg, int));
                break;
            case 's':
                _sys_puts_nl(va_arg(arg, char*), 0);
                break;
            case 'd':
                _sys_putint(va_arg(arg, int), 10);
                break;
            case 'x':
                _sys_puts_nl("0x", 0);
                _sys_putint(va_arg(arg, int), 16);
                break;
            case 'b': // 标准的printf没有这个
                _sys_puts_nl("0b", 0);
                _sys_putint(va_arg(arg, int), 2);
                break;
            case '%':
                _sys_putchar('%');
                break;
            default:
                break;
            }
        }
        fmt++;
    }
    va_end(arg);
}

void sleep(int ms) {
    int clock = _sys_clock();
    while (ms > 0) {
        ms -= (_sys_clock() - clock) * 10;
        clock = _sys_clock();
    }
}

void putchar(uint8_t c) {
    _sys_putchar(c);
}

void putchar_color(uint8_t c, enum Color fColor, enum Color bColor) {
    _sys_putchar_color(c, fColor, bColor);
}

void puts_color(uint8_t* s, enum Color fColor, enum Color bColor) {
    while ((*s) != '\0') {
        putchar_color(*s++, fColor, bColor);
    }
}

void puts(uint8_t* s) {
    _sys_puts_nl(s, TRUE);
}

void moveto(int row, int col) {
    asm volatile("int $0x80" :: "a" (sys_moveto), "b" (row), "c" (col*2));
}

void write_screen(uint8_t p, uint8_t c) {
    asm volatile("int $0x80" :: "a" (sys_write_screen), "b" (p), "c" (c));
}

void clsscr() {
    asm volatile("int $0x80" :: "a" (sys_clsscr));
}

int add_task(uint32_t entry) {
    int ret = -1;
    asm volatile("int $0x80" : "=b" (ret) : "a" (sys_add_task), "b" (entry));
    return ret;
}

void switch_enable(bool enable) {
    asm volatile("int $0x80" :: "a" (sys_switch_enable), "b" (enable));
}

uint8_t getch() {
    return _sys_getch();
}

uint8_t getchar() {
    char c;
    while ((c = getch()) == '\xff') {}
    putchar(c);
    return c;
}

void gets(char *s) {
    char c;
    while ((c = getchar()) != '\n') {
        if (c != '\b')
            *(s++) = c;
        else s--;
    }
    *s = '\0';
}

int strcmp(const char *p1, const char *p2) {
    const uint8_t *s1 = (const uint8_t *) p1;
    const uint8_t *s2 = (const uint8_t *) p2;
    uint8_t c1, c2;
    do {
        c1 = *s1++;
        c2 = *s2++;
        if (c1 == '\0')
	        return c1 - c2;
    } while (c1 == c2);
    return c1 - c2;
}
