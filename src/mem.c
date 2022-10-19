#include <osdefs.h>

void memset(uint8_t* addr, uint8_t val, int offset) {
    for (int i = 0; i < offset; i++) {
        addr[i] = val;
    }
}

void memcpy(uint8_t* src, uint8_t* dst, int size) {
    for (int i = 0; i < size; i++) {
        dst[i] = src[i];
    }
}
