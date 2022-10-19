#include <user_lib.h>
#include <stdarg.h>

uint32_t _sys_clock() {
    uint32_t current_clock;
    asm volatile("int $0x80" : "=b" (current_clock) : "a" (sys_clock));
    return current_clock;
}

static inline void _sys_putchar(uint8_t c) {
    asm volatile("int $0x80" :: "a" (sys_putchar), "b" (c));
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

void sleep(uint32_t ms) {
    uint32_t clock = _sys_clock();
    while (ms > 0) {
        // 可能有溢出风险
        ms -= (_sys_clock() - clock) * 10;
        clock = _sys_clock();
    }
}

void putchar(uint8_t c) {
    _sys_putchar(c);
}

void puts(uint8_t* s) {
    _sys_puts_nl(s, TRUE);
}

void moveto(int row, int col) {
    asm volatile("int $0x80" :: "a" (sys_moveto), "b" (row), "c" (col*2));
}

uint8_t getch() {
    return _sys_getch();
}
