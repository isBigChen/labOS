#include "osdefs.h"
#include "asm.h"

static uint8_t pic0_mask = 0xfb, pic1_mask = 0xff;
static uint8_t did_init = 0;

static void pic_setmask(uint8_t mask0, uint8_t mask1) {
    pic0_mask = mask0;
    pic1_mask = mask1;
    
    if (did_init) {
        outb(PIC0_IMR, mask0);
        outb(PIC1_IMR, mask1);
    }
}

void pic_enable(unsigned int irq_num) {
    if (irq_num < 8) { // set mask of PIC 0
        pic_setmask(pic0_mask & ~(1 << irq_num), pic1_mask);
    } else { // set mask of PIC 1
        pic_setmask(pic0_mask, pic1_mask & ~(1 << (irq_num - 8)));
    }
}

void pic_ack() {
    /* IRQ # on slave PIC send EOI to slave */
    outb(0xA0,0x20);
    /* Send EOI to master PIC */
    outb(0x20,0x20);
}

void init_pic() {            /* 抄30os书. 初始化PIC 可编程中断控制器 */
    did_init = 1;
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
    /* PIC0: 11111011 PIC1以外全部禁止; PIC1: 11111111 禁止所有中断 */
    pic_setmask(pic0_mask, pic1_mask);
}