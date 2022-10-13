#include "osdefs.h"
#include "asm.h"
#include "screen.h"
#include "init.h"
#include "picirq.h"

void main(){
    init_scr();   // 初始化显示
    init_gdt();   // 重新设置GDT 
    init_pmm();   // 初始化物理内存管理, 目前只有分页
    init_idt();   // 初始化及设置IDT
    init_pic();   // 初始化PIC
    init_kbd();
    init_timer(); // 初始化定时器
    _sti();       // 开中断
    while (1){
        _hlt();
    }
}
