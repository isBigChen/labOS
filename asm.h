#ifndef _ASM_H_
#define _ASM_H_

#include "osdefs.h"

static inline void _hlt() __attribute__((always_inline));
static inline void outb(uint16_t port, uint8_t val) __attribute__((always_inline));
static inline uint8_t inb(uint16_t port) __attribute__((always_inline));
static inline void _sti() __attribute__((always_inline));

static inline void _hlt() {
    asm volatile ("hlt");
}

static inline void _sti() {
    asm volatile ("sti");
}

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ( "inb %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

#endif