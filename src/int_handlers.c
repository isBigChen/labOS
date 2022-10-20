#include <osdefs.h>
#include <screen.h>
#include <picirq.h>
#include <kbd.h>
#include <mem.h>
#include <task.h>

uint32_t kern_clock = 1; // 触发时钟中断的次数
uint32_t old_clock = 1;

int tasks_ptr = 1, time_limit = 500;
bool switched = 0, switch_enable = 0;

// https://gcc.gnu.org/onlinedocs/gcc-7.5.0/gcc/x86-Function-Attributes.html#x86-Function-Attributes
// 时钟中断
__attribute__ ((interrupt))
void interrupt_handler_0x20(struct interrupt_frame_wpt *frame) {
    // 直接修改中断栈帧实现多任务切换
    // 进入TestosMain后 user_tasks = {TestosMain, task1, task2, task3}
    if (tasks_count > 1 && switch_enable) {
        if (!switched) {
            // switch_enable后首先跳转到task1
            frame->tf_eip = user_tasks[tasks_ptr].eip;
            frame->tf_esp = user_tasks[tasks_ptr].esp;
            old_clock = kern_clock;
            switched = 1;
        } else {
            time_limit--;
            if (old_clock + 10 == kern_clock) {
                // 每10个时钟切换一次任务
                old_clock = kern_clock;

                // 先记录下刚才跑到哪了
                user_tasks[tasks_ptr].eip = frame->tf_eip;
                user_tasks[tasks_ptr].esp = frame->tf_esp;

                // 保证只在 task1, task2, task3 之间切换
                tasks_ptr = tasks_ptr % (tasks_count - 1) + 1;
                frame->tf_eip = user_tasks[tasks_ptr].eip;
                frame->tf_esp = user_tasks[tasks_ptr].esp;
            } 
            if (time_limit == 0) {
                // 5 秒后复位到user_tasks[0]即TestosMain并修改标志
                tasks_ptr = 1;
                time_limit = 500;
                tasks_count = 1;
                frame->tf_eip = user_tasks[0].eip;
                frame->tf_esp = user_tasks[0].esp;
                switched = 0;
                switch_enable = FALSE;
            }
        }
    }
    set_seg_regs();
    pic_ack();
    kern_clock++;
    reset_seg_regs();
}

// 键盘输入缓冲区(数组循环队列)
extern uint8_t kern_buf[BUF_SIZE], kern_buf_pos, kern_buf_cur;

__attribute__ ((interrupt))
void interrupt_handler_0x21(struct interrupt_frame *frame) {
    pic_ack();

    static bool fSHIFT = 0;
    static bool fCAPSLOCK = 0;

    if ((inb(PS2_STAT) & 0x01) == 0) { // check 8042 output buffer status
        return;
    }
    uint8_t c = inb(PS2_DATA);
    
    // For test only
    // if (c != 0xE0) {
    //     kern_printf("[TEST] Key [%s] is %s, code %x\n",
    //         test_keymap[c & ~0x80], (c & 0x80) == 0 ? "pressed" : "released", c);
    // }

    if (c == 0x2a || c == 0x36) { // shift pressed
        fSHIFT = 1;
    }
    if (c == 0xaa || c == 0xb6) { // shift released 
        fSHIFT = 0;
    }
    if (c == 0x3a) { // handle capslock
        fCAPSLOCK = ~fCAPSLOCK;
    }

    if (!(c & 0x80)) { // pressed then print the char
        uint8_t output = fSHIFT ? shift_keymap[c & ~0x80] : normal_keymap[c & ~0x80];
        if (output != NO) {
            // kern_putchar_color(fCAPSLOCK ? switch_alpha_case(output) : output, Red, Blue);
            kern_buf[kern_buf_pos] = output;
            kern_buf_pos = (kern_buf_pos + 1) % BUF_SIZE;
        }
    }
}

// 定义于sys_call.c
extern __attribute__ ((interrupt)) void interrupt_handler_0x80(struct interrupt_frame *frame);

__attribute__ ((interrupt))
void interrupt_handler_default(struct interrupt_frame *frame) {
    pic_ack();
    kern_puts("default handler");
}

// 许多异常放到栈上的东西有差别　暂时不管
// 参考https://www.logix.cz/michal/doc/i386/chp09-08.htm#09-08
// 有push error code的要加上一个error_code参数, 不然iret应该会出错
__attribute__ ((interrupt))
void divide_zero_fault_handler0(struct interrupt_frame *frame) {
    kern_puts("0 Divide Zero");
    while (1);
}

__attribute__ ((interrupt))
void debug_exception_handler1(struct interrupt_frame *frame) {
    kern_puts("1 Debug Exception");
    while (1);
}

__attribute__ ((interrupt))
void breakpoint_trap_handler3(struct interrupt_frame *frame) {
    kern_puts("3 Breakpoint");
    while (1);
}

__attribute__ ((interrupt))
void overflow_trap_handler4(struct interrupt_frame *frame) {
    kern_puts("4 Overflow");
    while (1);
}

__attribute__ ((interrupt))
void bounds_check_fault_handler5(struct interrupt_frame *frame) {
    kern_puts("5 Bounds Check");
    while (1);
}

__attribute__ ((interrupt))
void invalid_opcode_fault_handler6(struct interrupt_frame *frame) {
    kern_puts("6 Invalid Opcode");
    while (1);
}

__attribute__ ((interrupt))
void coprocessor_exception_handler7(struct interrupt_frame *frame) {
    kern_puts("7 Coprocessor Not Available");
    while (1);
}

__attribute__ ((interrupt))
void double_fault_handler8(struct interrupt_frame *frame, uint32_t error_code) {
    kern_puts("8 Double Fault");
    while (1);
}

__attribute__ ((interrupt))
void CSO_exception_handler9(struct interrupt_frame *frame) {
    kern_puts("9 Coprocessor Segment Overrun");
    while (1);
}

__attribute__ ((interrupt))
void invalid_tss_fault_handler10(struct interrupt_frame *frame, uint32_t error_code) {
    kern_puts("10 Invalid TSS");
    while (1);
}

__attribute__ ((interrupt))
void SNP_exception_handler11(struct interrupt_frame *frame, uint32_t error_code) {
    kern_puts("11 Segment Not Present");
    while (1);
}

__attribute__ ((interrupt))
void stack_exception_handler12(struct interrupt_frame *frame, uint32_t error_code) {
    kern_puts("12 Stack Exception");
    while (1);
}

// 目前最常见的是这位 ORZ
__attribute__ ((interrupt))
void general_protection_exception_handler13(struct interrupt_frame *frame, uint32_t error_code) {
    kern_printf("%x %x %x\n", frame->tf_eip, frame->tf_cs, error_code);
    kern_puts("13 General Protection Exception");
    while (1);
}

__attribute__ ((interrupt))
void page_fault_handler14(struct interrupt_frame *frame, uint32_t error_code) {
    kern_puts("14 Page Fault");
    kern_printf("Error code: %x\n", error_code);
    while (1);
}

__attribute__ ((interrupt))
void coprocessor_exception_handler16(struct interrupt_frame *frame) {
    kern_puts("16 Coprocessor Error");
    while (1);
}
