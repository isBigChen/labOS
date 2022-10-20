#include <task.h>
#include <screen.h>

/* 
预定是user task的内核栈. 按手册应该必须有一个. 应该放到系统页面.
When an interrupt (be it fault, IRQ, or software interrupt) happens 
while the CPU is in user mode, the CPU needs to know 
where the kernel stack is located. 
This location is stored in the ESP0 (0 for ring 0) entry of the TSS. 
Linux0.11源码叫这个名字
*/
uint8_t *user_stack = 0x1f0000;

int tasks_count = 0;
struct task_struct user_tasks[16] = {0};

int add_task(uint32_t entry) {
    user_tasks[tasks_count].eip = entry;
    user_tasks[tasks_count].esp = 0x7f1000 - (0x10000 * tasks_count);
    return tasks_count++;
}

void tmp_task_init(struct tss_struct* tmp_task_tss) {
    tmp_task_tss->cr3 = 0x0;
    tmp_task_tss->cs = 1*8;
    tmp_task_tss->cs_reserved = 0;
    tmp_task_tss->ds = 2*8;
    tmp_task_tss->ds_reserved = 0;
    tmp_task_tss->eax = 0;
    tmp_task_tss->ebp = 0;
    tmp_task_tss->ebx = 0;
    tmp_task_tss->ecx = 0;
    tmp_task_tss->edi = 0;
    tmp_task_tss->edx = 0;
    tmp_task_tss->eflags = 0x202; // IF = 1
    tmp_task_tss->eip = 0;
    tmp_task_tss->es = 2*8;
    tmp_task_tss->es_reserved = 0;
    tmp_task_tss->esi = 0;
    tmp_task_tss->esp0 = 0x1f0000;
    tmp_task_tss->esp1 = 0;
    tmp_task_tss->esp2 = 0;
    tmp_task_tss->esp = 0x1f0000;
    tmp_task_tss->fs = 2*8;
    tmp_task_tss->fs_reserved = 0;
    tmp_task_tss->gs = 2*8;
    tmp_task_tss->gs_reserved = 0;
    tmp_task_tss->iomap = 0x4000;
    tmp_task_tss->ldt = 0;
    tmp_task_tss->ldt_reserved = 0;
    tmp_task_tss->ss0 = 2*8;
    tmp_task_tss->ss0_reserved = 0;
    tmp_task_tss->ss1 = 0;
    tmp_task_tss->ss1_reserved = 0;
    tmp_task_tss->ss2 = 0;
    tmp_task_tss->ss2_reserved = 0;
    tmp_task_tss->ss = 2*8;
    tmp_task_tss->ss_reserved = 0;
    tmp_task_tss->trap = 0;
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
    user_task_tss->esp0 = &user_stack[1023]; // Ring0跳转时加载的esp. Linux0.11也叫这个
    user_task_tss->esp1 = 0;
    user_task_tss->esp2 = 0;
    user_task_tss->esp = 0x7f1000;// 指向用户栈起始位置. 应该是内存分配器(TODO)分配的空间
    user_task_tss->fs = USER_DATA_SEGMENT;
    user_task_tss->fs_reserved = 0;
    user_task_tss->gs = USER_DATA_SEGMENT;
    user_task_tss->gs_reserved = 0;
    user_task_tss->iomap = 0x4000;
    user_task_tss->ldt = 0; // wiki说现代x86系统都不用ldt直接设成0 不太懂
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

