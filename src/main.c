#include <osdefs.h>
#include <init.h>

static inline void move_to_user_task() {
    // 只能从任务切换到任务, 所以先加载一个临时tss (gdt[5]. 索引为5*8|0)
    // 0x30 为 user_task_tss (gdt[6]) 即 TestosMain
    asm volatile("ltr %%ax\n\t"
                 "jmp $0x30, $0" :: "a" (5*8));
}

void kern_main() {
    /*
     TODO 暂时没了
    */
    init_scr();           // 初始化显示
    init_gdt();           // 重新设置GDT; 添加第一个用户程序
    init_pmm();           // 初始化物理内存管理, 目前只有分页
    init_idt();           // 初始化及设置IDT
    init_pic();           // 初始化PIC
    init_kbd();           // 初始化键盘
    init_timer();         // 初始化定时器
    load_user_program();  // 加载用户程序到用户段
    _sti();               // 开中断
    move_to_user_task();  // 进入ring3用户程序
    while (1) {
        _hlt();
    }
}
