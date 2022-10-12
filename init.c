#include "osdefs.h"
#include "asm.h"
#include "int_handlers.h"

// https://wiki.osdev.org/GDT_Tutorial
void set_segdesc(struct segment_descriptor* sd, uint32_t base, uint32_t limit, uint16_t flag)
{
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
    set_segdesc(gdt, 0, 0, 0);
    set_segdesc(gdt+1, 0, 0x007fffff, GDT_CODE_PL0);
    set_segdesc(gdt+2, 0, 0x007fffff, GDT_DATA_PL0);
    set_segdesc(gdt+3, 0, 0x007fffff, GDT_CODE_PL3);
    set_segdesc(gdt+4, 0, 0x007fffff, GDT_DATA_PL3);

    struct {uint16_t limit; uint32_t addr;} __attribute__((packed)) gdtr;
    gdtr.limit = 5*sizeof(struct segment_descriptor)-1;
    gdtr.addr = gdt;
    
    asm volatile("lgdt %0\n\t"
                 "jmp $0x08,$x\n\t" :: "m" (gdtr));
    //$x是符号，下面内嵌汇编的x符号
    // 重新加载后要刷新段寄存器
    asm volatile("x: mov $0x10, %ax\n\t"
                 "mov %ax, %ds\n\t"
                 "mov %ax, %es\n\t"
                 "mov %ax, %fs\n\t"
                 "mov %ax, %gs\n\t"
                 "mov %ax, %ss\n\t");
}

void init_pic() {            /* 抄30os书. 初始化PIC 可编程中断控制器 */
    outb(PIC0_IMR, 0xff);    /* 禁止所有中断 */
    outb(PIC1_IMR, 0xff);    /* 禁止所有中断 */
    outb(PIC0_ICW1, 0x11);   /* 边沿触发模式（ edge trigger mode） */
    outb(PIC0_ICW2, 0x20);   /* IRQ0-7由INT 0x20-0x27接收 0x00~0x1f由cpu内部产生*/
    outb(PIC0_ICW3, 1<<2);   /* PIC1由IRQ2连接 */
    outb(PIC0_ICW4, 0x01);   /* 无缓冲区模式 */
    outb(PIC1_ICW1, 0x11);   /* 边沿触发模式（ edge trigger mode） */
    outb(PIC1_ICW2, 0x28);   /* IRQ8-15由INT 0x28-0x2f接收 */
    outb(PIC1_ICW3, 2);      /* PIC1由IRQ2连接 */
    outb(PIC1_ICW4, 0x01);   /* 无缓冲区模式 */
    outb(PIC0_IMR, 0xfc);    /* 11111011 PIC1以外全部禁止 */
    outb(PIC1_IMR, 0xff);    /* 11111111 禁止所有中断 */
}

void init_timer() { // 抄书, 设置定时器PIT(Programmable Interval Timer)
    outb(PIT_CTRL, 0x34);
    outb(PIT_CNT0, 0x9c);
    outb(PIT_CNT0, 0x2e); //大概是10ms
    outb(PIC0_IMR, 0xfa); // PIC1, PIT
    outb(PIC1_IMR, 0xff);
}

//// https://wiki.osdev.org/Interrupts_tutorial
// 设置中断门描述符　https://www.logix.cz/michal/doc/i386/chp09-05.htm#09-05
//isr is the interrupe function's address
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
    // 暂定为系统调用中断号
    set_intrdesc(idt+0x80, interrupt_handler_0x80, KERNEL_CS, AR_INTGATE32+0x60);
    
    struct {uint16_t limit; uint32_t addr;} __attribute__((packed)) idtr;
    idtr.limit = 255 * sizeof(struct idt_entry_t) - 1; // 有效字节数-1
    idtr.addr = idt;
    asm volatile ("lidt %0" :: "m" (idtr));
}
