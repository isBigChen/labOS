#include <osdefs.h>
#include <screen.h>
#include <picirq.h>

extern int kern_clock;
extern uint8_t kern_buf[BUF_SIZE], kern_buf_pos, kern_buf_cur;

__attribute__ ((interrupt))
void interrupt_handler_0x80(struct interrupt_frame *frame) {
    set_seg_regs();
    int no, arg1, arg2, arg3, arg4;
    asm volatile ("mov %%eax, %0" : "=m" (no) :);
    asm volatile ("mov %%ebx, %0" : "=m" (arg1) :);
    asm volatile ("mov %%ecx, %0" : "=m" (arg2) :);
    asm volatile ("mov %%edi, %0" : "=m" (arg3) :);
    asm volatile ("mov %%esi, %0" : "=m" (arg4) :);
    switch (no)
    {
    case sys_clock:
        // 最后编译出的指令碰巧没有push, pop ebx. 所以用ebx放返回值
        asm volatile ("mov %0, %%ebx" :: "m" (kern_clock));
        break;
    case sys_putchar:
        kern_putchar(arg1);
        break;
    case sys_putint:
        kern_putint(arg1, arg2);
        break;
    case sys_puts_nl:
        kern_puts_nl((uint8_t*)arg1, arg2);
        break;
    case sys_moveto:
        kern_moveto(arg1, arg2);
        break;
    case sys_clsscr:
        kern_clsscr();
        break;
    case sys_getch:
        if (kern_buf_cur != kern_buf_pos) {
            asm volatile ("mov %0, %%ebx" :: "m" (kern_buf[kern_buf_cur]));
            kern_buf_cur = (kern_buf_cur + 1) % BUF_SIZE;
        } else {
            asm volatile ("mov %0, %%ebx" :: "N" ('\0'));
        }
        break;
    default:
        break;
    }
    reset_seg_regs();
}
