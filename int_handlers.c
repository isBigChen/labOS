#include "osdefs.h"
#include "asm.h"
#include "screen.h"

// https://gcc.gnu.org/onlinedocs/gcc-7.5.0/gcc/x86-Function-Attributes.html#x86-Function-Attributes
__attribute__ ((interrupt))
void interrupt_handler_0x20(struct interrupt_frame *frame) {
    static int cnt = 1;
    outb(0xA0,0x20);
    /* Send EOI to master PIC */
    outb(0x20,0x20);
    cnt++;
    if (cnt == 100) {
        puts(OS_NAME);
        cnt = 1;
    }
}

__attribute__ ((interrupt))
void interrupt_handler_0x80(struct interrupt_frame *frame) {
    puts("80");
}

__attribute__ ((interrupt))
void interrupt_handler_default(struct interrupt_frame *frame) {
    /* IRQ # on slave PIC send EOI to slave */
    outb(0xA0,0x20);
    /* Send EOI to master PIC */
    outb(0x20,0x20);
    // puts("default handler");
}

// 许多中断放到栈上的东西有差别　暂时不管
// 参考https://www.logix.cz/michal/doc/i386/chp09-08.htm#09-08
// 有push error code的要加上一个error_code参数, 不然iret应该会出错
__attribute__ ((interrupt))
void divide_zero_fault_handler0(struct interrupt_frame *frame) {
    puts("0 Divide Zero");
    while (1);
}

__attribute__ ((interrupt))
void debug_exception_handler1(struct interrupt_frame *frame) {
    puts("1 Debug Exception");
    while (1);
}

__attribute__ ((interrupt))
void breakpoint_trap_handler3(struct interrupt_frame *frame) {
    puts("3 Breakpoint");
    while (1);
}

__attribute__ ((interrupt))
void overflow_trap_handler4(struct interrupt_frame *frame) {
    puts("4 Overflow");
    while (1);
}

__attribute__ ((interrupt))
void bounds_check_fault_handler5(struct interrupt_frame *frame) {
    puts("5 Bounds Check");
    while (1);
}

__attribute__ ((interrupt))
void invalid_opcode_fault_handler6(struct interrupt_frame *frame) {
    puts("6 Invalid Opcode");
    while (1);
}

__attribute__ ((interrupt))
void coprocessor_exception_handler7(struct interrupt_frame *frame) {
    puts("7 Coprocessor Not Available");
    while (1);
}

__attribute__ ((interrupt))
void double_fault_handler8(struct interrupt_frame *frame, uint32_t error_code) {
    puts("8 Double Fault");
    while (1);
}

__attribute__ ((interrupt))
void CSO_exception_handler9(struct interrupt_frame *frame) {
    puts("9 Coprocessor Segment Overrun");
    while (1);
}

__attribute__ ((interrupt))
void invalid_tss_fault_handler10(struct interrupt_frame *frame, uint32_t error_code) {
    puts("10 Invalid TSS");
    while (1);
}

__attribute__ ((interrupt))
void SNP_exception_handler11(struct interrupt_frame *frame, uint32_t error_code) {
    puts("11 Segment Not Present");
    while (1);
}

__attribute__ ((interrupt))
void stack_exception_handler12(struct interrupt_frame *frame, uint32_t error_code) {
    puts("12 Stack Exception");
    while (1);
}

__attribute__ ((interrupt))
void general_protection_exception_handler13(struct interrupt_frame *frame, uint32_t error_code) {
    printf("%x %x %x\n", frame->tf_eip, frame->tf_cs, error_code);
    puts("13 General Protection Exception");
    while (1);
}

__attribute__ ((interrupt))
void page_fault_handler14(struct interrupt_frame *frame, uint32_t error_code) {
    puts("14 Page Fault");
    while (1);
}

__attribute__ ((interrupt))
void coprocessor_exception_handler16(struct interrupt_frame *frame) {
    puts("16 Coprocessor Error");
    while (1);
}