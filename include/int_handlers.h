#ifndef _INT_HANDLERS_H_
#define _INT_HANDLERS_H_

#include <osdefs.h>

#define INT_TIMER    0x20
#define INT_KEYBOARD 0x21

__attribute__ ((interrupt))
void divide_zero_fault_handler0(struct interrupt_frame *frame);

__attribute__ ((interrupt))
void debug_exception_handler1(struct interrupt_frame *frame);

__attribute__ ((interrupt))
void breakpoint_trap_handler3(struct interrupt_frame *frame);

__attribute__ ((interrupt))
void overflow_trap_handler4(struct interrupt_frame *frame);

__attribute__ ((interrupt))
void bounds_check_fault_handler5(struct interrupt_frame *frame);

__attribute__ ((interrupt))
void invalid_opcode_fault_handler6(struct interrupt_frame *frame);

__attribute__ ((interrupt))
void coprocessor_exception_handler7(struct interrupt_frame *frame);

__attribute__ ((interrupt))
void double_fault_handler8(struct interrupt_frame *frame, uint32_t error_code);

__attribute__ ((interrupt))
void CSO_exception_handler9(struct interrupt_frame *frame);

__attribute__ ((interrupt))
void invalid_tss_fault_handler10(struct interrupt_frame *frame, uint32_t error_code);

__attribute__ ((interrupt))
void SNP_exception_handler11(struct interrupt_frame *frame, uint32_t error_code);

__attribute__ ((interrupt))
void stack_exception_handler12(struct interrupt_frame *frame, uint32_t error_code);

__attribute__ ((interrupt))
void general_protection_exception_handler13(struct interrupt_frame *frame, uint32_t error_code);

__attribute__ ((interrupt))
void page_fault_handler14(struct interrupt_frame *frame, uint32_t error_code);

__attribute__ ((interrupt))
void coprocessor_exception_handler16(struct interrupt_frame *frame);

// 默认处理
__attribute__ ((interrupt))
void interrupt_handler_default(struct interrupt_frame *frame);

// 时钟中断
__attribute__ ((interrupt))
void interrupt_handler_0x20(struct interrupt_frame *frame);

// 键盘中断
__attribute__ ((interrupt))
void interrupt_handler_0x21(struct interrupt_frame *frame);

// 暂定是系统调用中断号
__attribute__ ((interrupt))
void interrupt_handler_0x80(struct interrupt_frame *frame);

#endif