#include "osdefs.h"
#include "asm.h"
#include "int_handlers.h"
#include "mem.h"
#include "picirq.h"

// https://wiki.osdev.org/GDT_Tutorial
void set_segdesc(struct segment_descriptor* sd, uint32_t base, uint32_t limit, uint16_t flag) {
    // Create the high 32 bit segment
    sd->high_part  =  limit       & 0x000F0000; // set limit bits 19:16
    sd->high_part |= (flag <<  8) & 0x00F0FF00; // set type, p, dpl, s, g, d/b, l and avl fields
    sd->high_part |= (base >> 16) & 0x000000FF; // set base bits 23:16
    sd->high_part |=  base        & 0xFF000000; // set base bits 31:24
    // Create the low 32 bit segment
    sd->low_part |= base  << 16;                // set base bits 15:0
    sd->low_part |= limit  & 0x0000FFFF;        // set limit bits 15:0
}

void init_gdt() { // 重新设置gdt表
    struct segment_descriptor* gdt = (struct segment_descriptor*)GDT_ADDR;
    
    // NULL descriptor
    set_segdesc(gdt, 0, 0, 0);
    // 后面只分页, 分段当做不存在?
    set_segdesc(gdt+1, 0, 0xffffffff, GDT_CODE_PL0); // 内核代码段
    set_segdesc(gdt+2, 0, 0xffffffff, GDT_DATA_PL0); // 内核数据段
    set_segdesc(gdt+3, 0, 0xffffffff, GDT_CODE_PL3); // 用户代码段
    set_segdesc(gdt+4, 0, 0xffffffff, GDT_DATA_PL3); // 用户数据段

    struct {uint16_t limit; uint32_t addr;} __attribute__((packed)) gdtr;
    gdtr.limit = 5 * sizeof(struct segment_descriptor) - 1;
    gdtr.addr = gdt;
    
    asm volatile("lgdt %0\n\t"
                 "jmp $0x08,$x\n\t" :: "m" (gdtr));
    // 重新加载后要刷新段寄存器
    asm volatile("x: mov $0x10, %ax\n\t"
                 "mov %ax, %ds\n\t"
                 "mov %ax, %es\n\t"
                 "mov %ax, %fs\n\t"
                 "mov %ax, %gs\n\t"
                 "mov %ax, %ss\n\t");
}

/*
    https://wiki.osdev.org/Setting_Up_Paging

    初始化物理内存管理.
    先分页.
    二级页表 10 10 12 ; 4KB一页, 页表项4字节, 一页有1024项, 一个页表映射4MB.
    qemu默认内存为128MB, 为了节省探测内存大小的工作, 就不改qemu设置了.
    所以当做物理内存有128MB, 不考虑虚拟内存管理, 最多只需要32个页表, 页目录表1个就够.
    我们这里简单起见只用2个页表, 管理8MB内存.
    0x0~0xfff 放页目录表. 0x1000~0x1fff 放第一个页表. 0x2000~0x2fff 放第二个页表.
    第一个页表映射 0x0~0x3fffff 第二个映射 0x400000~0x7fffff
    还是保证逻辑地址等于物理地址
    第一个页表的前三项映射范围就是上面的三个表, 如果画个图会发现关系有点乱...
    页目录表和页表目前均看做数组.
*/
void init_pmm() {
    uint32_t* page_directory = 0x0;

    uint32_t* first_page_table = 0x1000;
    uint32_t* second_page_table = 0x2000;

    for (uint32_t i = 0; i < 1024; i++) {
        first_page_table[i] = (i * 0x1000) | 3; // supervisor level, read/write, present.
    }
    for (uint32_t i = 0; i < 1024; i++) {
        second_page_table[i] = (0x400000 + i * 0x1000) | 3;
    }

    page_directory[0] = ((uint32_t)first_page_table) | 3; // // supervisor level, read/write, present.
    page_directory[1] = ((uint32_t)second_page_table) | 3;

    // 加载页目录表起始地址到 cr3
    
    asm volatile ("xor %eax, %eax\n\t");
    asm volatile ("mov %0, %%eax\n\t"
                  "mov %%eax, %%cr3" :: "m" (page_directory));
    
    // 设置cr0开启分页
    
    asm volatile ("xor %eax, %eax\n\t");
    asm volatile ("mov %cr0, %eax\n\t"
                  "or $0x80000000,%eax\n\t"
                  "mov %eax,%cr0");
}

void init_timer() { // 抄书, 设置定时器PIT(Programmable Interval Timer)
    outb(PIT_CTRL, 0x34);
    outb(PIT_CNT0, 0x9c);
    outb(PIT_CNT0, 0x2e); //大概是10ms
    pic_enable(IRQ(INT_TIMER));
}

void init_kbd() {
    pic_enable(IRQ(INT_KEYBOARD));
}

// 设置中断门描述符　https://www.logix.cz/michal/doc/i386/chp09-05.htm#09-05
void set_intrdesc(struct idt_entry_t *id, void* isr, uint16_t cs, uint8_t attr) {
    id->isr_low = (uint32_t)isr & 0xffff;
    id->kernel_cs = cs;
    id->reserved = 0;
    id->attributes = attr;
    id->isr_high = (uint32_t)isr >> 16;
}

struct idt_entry_t *idt = (struct idt_entry_t*)IDT_ADDR;

void init_idt() { //初始化中断表
    for (int i = 0; i < 255; i++) {
        set_intrdesc(idt + i, 0, 0, 0);
    }
    
    // intel cpu内部中断
    set_intrdesc(idt + 0, divide_zero_fault_handler0, KERNEL_CS, AR_INTGATE32);
    set_intrdesc(idt + 1, debug_exception_handler1, KERNEL_CS, AR_INTGATE32);
    set_intrdesc(idt + 3, breakpoint_trap_handler3, KERNEL_CS, AR_INTGATE32);
    set_intrdesc(idt + 4, overflow_trap_handler4, KERNEL_CS, AR_INTGATE32);
    set_intrdesc(idt + 5, bounds_check_fault_handler5, KERNEL_CS, AR_INTGATE32);
    set_intrdesc(idt + 6, invalid_opcode_fault_handler6, KERNEL_CS, AR_INTGATE32);
    set_intrdesc(idt + 7, coprocessor_exception_handler7, KERNEL_CS, AR_INTGATE32);
    set_intrdesc(idt + 8, double_fault_handler8, KERNEL_CS, AR_INTGATE32);
    set_intrdesc(idt + 9, CSO_exception_handler9, KERNEL_CS, AR_INTGATE32);
    set_intrdesc(idt + 10, invalid_tss_fault_handler10, KERNEL_CS, AR_INTGATE32);
    set_intrdesc(idt + 11, SNP_exception_handler11, KERNEL_CS, AR_INTGATE32);
    set_intrdesc(idt + 12, stack_exception_handler12, KERNEL_CS, AR_INTGATE32);
    set_intrdesc(idt + 13, general_protection_exception_handler13, KERNEL_CS, AR_INTGATE32);
    set_intrdesc(idt + 14, page_fault_handler14, KERNEL_CS, AR_INTGATE32);
    set_intrdesc(idt + 16, coprocessor_exception_handler16, KERNEL_CS, AR_INTGATE32);
    
    // PIC设置
    for (int i = 0x20; i <= 0x2f; i++) {
        set_intrdesc(idt+i, interrupt_handler_default, KERNEL_CS, AR_INTGATE32);
    }

    // 时钟中断
    set_intrdesc(idt+0x20, interrupt_handler_0x20, KERNEL_CS, AR_INTGATE32);
    // 键盘中断
    set_intrdesc(idt+0x21, interrupt_handler_0x21, KERNEL_CS, AR_INTGATE32);
    // 暂定为系统调用中断号
    set_intrdesc(idt+0x80, interrupt_handler_0x80, KERNEL_CS, AR_INTGATE32+0x60);
    
    struct {uint16_t limit; uint32_t addr;} __attribute__((packed)) idtr;
    idtr.limit = 255 * sizeof(struct idt_entry_t) - 1; // 有效字节数-1
    idtr.addr = idt;
    asm volatile ("lidt %0" :: "m" (idtr));
}
