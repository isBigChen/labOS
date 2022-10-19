#ifndef _TASK_H_
#define _TASK_H_

#include <osdefs.h>

#define USER_LOAD_ADDR 0x400000

void task_init(struct tss_struct* kernel_task_tss);
void user_task_init(struct tss_struct* tss, uint32_t task_addr, uint8_t* stack);

#endif