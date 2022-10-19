#ifndef _INIT_H_
#define _INIT_H_

#include "osdefs.h"

void set_segdesc(struct segment_descriptor* sd, uint32_t base, uint32_t limit, uint16_t flag);
void init_gdt();
void init_pmm();
void init_pic();
void init_timer();
void set_intrdesc(struct idt_entry_t *id, void* isr, uint16_t cs, uint8_t attr);

#endif