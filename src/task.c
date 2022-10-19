#include <task.h>

/* 
预定是user task的内核栈. 按手册应该必须有一个. 应该放到系统页面.
When an interrupt (be it fault, IRQ, or software interrupt) happens 
while the CPU is in user mode, the CPU needs to know 
where the kernel stack is located. 
This location is stored in the ESP0 (0 for ring 0) entry of the TSS. 
*/
uint8_t *user_stack = 0x7f0000; // 分页后或许需要一个链接脚本

void task_init(struct tss_struct* kernel_task_tss) {
    kernel_task_tss->cr3 = 0x0;
    kernel_task_tss->cs = 1*8;
    kernel_task_tss->cs_reserved = 0;
    kernel_task_tss->ds = 2*8;
    kernel_task_tss->ds_reserved = 0;
    kernel_task_tss->eax = 0;
    kernel_task_tss->ebp = 0;
    kernel_task_tss->ebx = 0;
    kernel_task_tss->ecx = 0;
    kernel_task_tss->edi = 0;
    kernel_task_tss->edx = 0;
    kernel_task_tss->eflags = 0x202; // IF = 1
    kernel_task_tss->eip = 0;
    kernel_task_tss->es = 2*8;
    kernel_task_tss->es_reserved = 0;
    kernel_task_tss->esi = 0;
    kernel_task_tss->esp0 = 0x1f0000;
    kernel_task_tss->esp1 = 0;
    kernel_task_tss->esp2 = 0;
    kernel_task_tss->esp = 0x1f0000;
    kernel_task_tss->fs = 2*8;
    kernel_task_tss->fs_reserved = 0;
    kernel_task_tss->gs = 2*8;
    kernel_task_tss->gs_reserved = 0;
    kernel_task_tss->iomap = 0x4000;
    kernel_task_tss->ldt = 0;
    kernel_task_tss->ldt_reserved = 0;
    kernel_task_tss->ss0 = 2*8;
    kernel_task_tss->ss0_reserved = 0;
    kernel_task_tss->ss1 = 0;
    kernel_task_tss->ss1_reserved = 0;
    kernel_task_tss->ss2 = 0;
    kernel_task_tss->ss2_reserved = 0;
    kernel_task_tss->ss = 2*8;
    kernel_task_tss->ss_reserved = 0;
    kernel_task_tss->trap = 0;
}

// https://wiki.osdev.org/Getting_to_Ring_3
// 要加上RPL
#define USER_CODE_SEGMENT (3*8|3)
#define USER_DATA_SEGMENT (4*8|3)

#define KERNEL_CODE_SEGMENT (1*8)
#define KERNEL_DATA_SEGMENT (2*8)

void user_task_init(struct tss_struct* user_task_tss, uint32_t task_addr, uint8_t* stack) {
    user_task_tss->cr3 = 0x0;
    user_task_tss->cs = USER_CODE_SEGMENT;
    user_task_tss->cs_reserved = 0;
    user_task_tss->ds = USER_DATA_SEGMENT;
    user_task_tss->ds_reserved = 0;
    user_task_tss->eax = 0x233;
    user_task_tss->ebp = 0;
    user_task_tss->ebx = 0;
    user_task_tss->ecx = 0;
    user_task_tss->edi = 0;
    user_task_tss->edx = 0;
    user_task_tss->eflags = 0x202; // IF = 1
    user_task_tss->eip = USER_LOAD_ADDR;
    user_task_tss->es = USER_DATA_SEGMENT;
    user_task_tss->es_reserved = 0;
    user_task_tss->esi = 0;
    user_task_tss->esp0 = 0x1f0000; // QAQ 应该指向末尾 esp往下减! (Ring0跳转时加载的esp)
    user_task_tss->esp1 = 0;
    user_task_tss->esp2 = 0;
    user_task_tss->esp = &user_stack[1023]; // 指向用户栈起始位置
    user_task_tss->fs = USER_DATA_SEGMENT;
    user_task_tss->fs_reserved = 0;
    user_task_tss->gs = USER_DATA_SEGMENT;
    user_task_tss->gs_reserved = 0;
    user_task_tss->iomap = 0x4000;
    user_task_tss->ldt = 0; // wiki说现代系统都不用ldt直接设成0 不太懂
    user_task_tss->ldt_reserved = 0;
    user_task_tss->ss0 = KERNEL_DATA_SEGMENT;
    user_task_tss->ss0_reserved = 0;
    user_task_tss->ss1 = 0;
    user_task_tss->ss1_reserved = 0;
    user_task_tss->ss2 = 0;
    user_task_tss->ss2_reserved = 0;
    user_task_tss->ss = USER_DATA_SEGMENT;
    user_task_tss->ss_reserved = 0;
    user_task_tss->trap = 0;
}

