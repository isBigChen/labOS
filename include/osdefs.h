#ifndef _OSDEFS_H_
#define _OSDEFS_H_

#define OS_NAME "testos"

typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
typedef unsigned char bool;

// 强制内联
#define __always_inline inline __attribute__((always_inline))
static inline void _hlt() {
    asm volatile ("hlt");
}

static inline void _sti() {
    asm volatile ("sti");
}

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ( "inb %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

static inline void insl(uint32_t port, void *addr, int cnt) {
    asm volatile (
        "cld;"
        "repne; insl;"
        : "=D" (addr), "=c" (cnt)
        : "d" (port), "0" (addr), "1" (cnt)
        : "memory", "cc");
}

static inline void set_seg_regs() {
    asm volatile ("push %ds\n\t" // 中断只会自动改ss到0x10. 数据段要自己改一下.
                  "push %es\n\t"
                  "push %fs\n\t"
                  "push %gs\n\t"
                  //"pushal\n\t"
                  "mov $0x10, %edx\n\t"
                  "mov %dx, %ds\n\t"
                  "mov %dx, %es");
}

static inline void reset_seg_regs() {
    asm volatile (//"popal\n\t"
                  "pop %gs\n\t"
                  "pop %fs\n\t"
                  "pop %es\n\t"
                  "pop %ds");
}

enum Color {Black, Blue, Green, Cyan, Red, \
            Purple, Brown, Gray, Dark_Gray, \
            Light_Blue, Light_Green, Light_Cyan, \
            Light_Red, Light_Purple, Yellow, White};

#define ABS(x) ((x)>=0?(x):-(x))

#define TRUE 1
#define FALSE 0

// https://wiki.osdev.org/GDT_Tutorial
#define SEG_DESCTYPE(x)  ((x) << 0x04) // Descriptor type (0 for system, 1 for code/data)
#define SEG_PRES(x)      ((x) << 0x07) // Present
#define SEG_SAVL(x)      ((x) << 0x0C) // Available for system use
#define SEG_LONG(x)      ((x) << 0x0D) // Long mode
#define SEG_SIZE(x)      ((x) << 0x0E) // Size (0 for 16-bit, 1 for 32)
#define SEG_GRAN(x)      ((x) << 0x0F) // Granularity (0 for 1B - 1MB, 1 for 4KB - 4GB)
#define SEG_PRIV(x)     (((x) &  0x03) << 0x05)   // Set privilege level (0 - 3)
 
#define SEG_DATA_RD        0x00 // Read-Only
#define SEG_DATA_RDA       0x01 // Read-Only, accessed
#define SEG_DATA_RDWR      0x02 // Read/Write
#define SEG_DATA_RDWRA     0x03 // Read/Write, accessed
#define SEG_DATA_RDEXPD    0x04 // Read-Only, expand-down
#define SEG_DATA_RDEXPDA   0x05 // Read-Only, expand-down, accessed
#define SEG_DATA_RDWREXPD  0x06 // Read/Write, expand-down
#define SEG_DATA_RDWREXPDA 0x07 // Read/Write, expand-down, accessed
#define SEG_CODE_EX        0x08 // Execute-Only
#define SEG_CODE_EXA       0x09 // Execute-Only, accessed
#define SEG_CODE_EXRD      0x0A // Execute/Read
#define SEG_CODE_EXRDA     0x0B // Execute/Read, accessed
#define SEG_CODE_EXC       0x0C // Execute-Only, conforming
#define SEG_CODE_EXCA      0x0D // Execute-Only, conforming, accessed
#define SEG_CODE_EXRDC     0x0E // Execute/Read, conforming
#define SEG_CODE_EXRDCA    0x0F // Execute/Read, conforming, accessed
 
#define GDT_CODE_PL0 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                     SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
                     SEG_PRIV(0)     | SEG_CODE_EXRD
 
#define GDT_DATA_PL0 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                     SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
                     SEG_PRIV(0)     | SEG_DATA_RDWR
 
#define GDT_CODE_PL3 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                     SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
                     SEG_PRIV(3)     | SEG_CODE_EXRD
 
#define GDT_DATA_PL3 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                     SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
                     SEG_PRIV(3)     | SEG_DATA_RDWR

#define KERNEL_CS 0x8
#define KERNEL_DS 0x10
#define USER_CS 0x18
#define USER_DS 0x20

struct segment_descriptor {
    uint32_t low_part, high_part;
};

// 随便设的　暂时不考虑空间浪费的问题
#define GDT_ADDR 0x3000

#define PIC0_ICW1 0x20
#define PIC0_OCW2 0x20
#define PIC0_IMR  0x21
#define PIC0_ICW2 0x21
#define PIC0_ICW3 0x21
#define PIC0_ICW4 0x21
#define PIC1_ICW1 0xa0
#define PIC1_OCW2 0xa0
#define PIC1_IMR  0xa1
#define PIC1_ICW2 0xa1
#define PIC1_ICW3 0xa1
#define PIC1_ICW4 0xa1

#define IDT_ADDR 0x4000

// https://wiki.osdev.org/Interrupts_tutorial
// ISR = Interrupt Service Routine
struct idt_entry_t {
	uint16_t    isr_low;      // The lower 16 bits of the ISR's address
	uint16_t    kernel_cs;    // The GDT segment selector that the CPU will load into CS before calling the ISR
	uint8_t     reserved;     // Set to zero
	uint8_t     attributes;   // Type and attributes; see the IDT page
	uint16_t    isr_high;     // The higher 16 bits of the ISR's address
} __attribute__((packed));

// 特权0中断属性 特权3用户级要加0x60
#define AR_INTGATE32 0x008e

// Timer的端口　https://wiki.osdev.org/Programmable_Interval_Timer#I/O%20Ports
#define PIT_CTRL 0x43
#define PIT_CNT0 0x40

// Keyboard端口
#define PS2_DATA 0x60
#define PS2_CMD  0x64
#define PS2_STAT 0x64

// https://www.logix.cz/michal/doc/i386/chp09-06.htm#09-06-01-01
struct interrupt_frame {
    uint32_t tf_eip;
    uint16_t tf_cs;
    uint16_t tf_padding4;
    uint32_t tf_eflags;
};

// 有特权级转换的中断frame
struct interrupt_frame_wpt {
    uint32_t tf_eip;
    uint16_t tf_cs;
    uint16_t tf_padding4;
    uint32_t tf_eflags;
    uint32_t tf_esp; // 从TSS得到的ss:esp
    uint16_t tf_ss;
};

/*
任务相关

https://wiki.osdev.org/Task_State_Segment
For each CPU which executes processes possibly wanting to do system calls via interrupts, 
one TSS is required. The only interesting fields are SS0 and ESP0. 
Whenever a system call occurs, the CPU gets the SS0 and ESP0-value in its TSS and assigns the stack-pointer to it. 
So one or more kernel-stacks need to be set up for processes doing system calls.
*/ 

struct tss_struct { // 104字节
/* SS0, SS1, SS2: The Segment Selectors used to load the stack 
                  when a privilege level change occurs from a lower privilege level to a higher one.
ESP0, ESP1, ESP2: The Stack Pointers used to load the stack 
                  when a privilege level change occurs from a lower privilege level to a higher one. */
    uint16_t   link, link_reserved;
    uint32_t   esp0;
    uint16_t   ss0, ss0_reserved;
    uint32_t   esp1;
    uint16_t   ss1, ss1_reserved;
    uint32_t   esp2;
    uint16_t   ss2, ss2_reserved;

    uint32_t   cr3;
    uint32_t   eip;
    uint32_t   eflags;
    uint32_t   eax;
    uint32_t   ecx;
    uint32_t   edx;
    uint32_t   ebx;
    uint32_t   esp;
    uint32_t   ebp;
    uint32_t   esi;
    uint32_t   edi;

    uint16_t   es, es_reserved;
    uint16_t   cs, cs_reserved;
    uint16_t   ss, ss_reserved;
    uint16_t   ds, ds_reserved;
    uint16_t   fs, fs_reserved;
    uint16_t   gs, gs_reserved;
    uint16_t   ldt, ldt_reserved;

    uint16_t   trap;
    uint16_t   iomap;
};

#define AR_TSS32 0x0089 //  P1 DPL00 010 B0 1

// 系统调用表
// eax 放功能编号. ebx, ecx, edi, esi, 栈放参数. edx保留
enum sys_call_tlb {
    sys_clsscr,
    sys_putchar,
    sys_putchar_color,
    sys_puts_nl,
    sys_putint,
    sys_moveto,
    sys_clock,
    sys_getch,
    sys_write_screen,
    sys_add_task,
    sys_switch_enable,
};

#define BUF_SIZE 100

#endif