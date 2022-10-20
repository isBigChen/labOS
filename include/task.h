#ifndef _TASK_H_
#define _TASK_H_

#include <osdefs.h>

#define USER_LOAD_ADDR 0x400000

struct task_struct {
    uint32_t   eip;
    uint32_t   esp;
    uint32_t   eflags;
    uint32_t   eax;
    uint32_t   ecx;
    uint32_t   edx;
    uint32_t   ebx;
    uint32_t   ebp;
    uint32_t   esi;
    uint32_t   edi;
};

extern int tasks_count;
extern struct task_struct user_tasks[16];

void tmp_task_init(struct tss_struct* tmp_task_tss);
void user_task_init(struct tss_struct* tss, uint32_t task_addr, uint8_t* stack);
int add_task(uint32_t entry);

#endif