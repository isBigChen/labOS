#ifndef _KBD_H_
#define _KBD_H_

#include <osdefs.h>

#define NO 0

extern const uint8_t normal_keymap[128];
extern const uint8_t shift_keymap[128];
extern const char * test_keymap[128];

uint8_t switch_alpha_case(uint8_t c);

#endif