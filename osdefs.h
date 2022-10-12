#ifndef _OSDEFS_H_
#define _OSDEFS_H_

#define OS_NAME "labOS"

// 强制内联
#define __always_inline inline __attribute__((always_inline))

typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;

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

//目前只用了没有涉及特权转换的中断栈层次 
// https://www.logix.cz/michal/doc/i386/chp09-06.htm#09-06-01-01
struct interrupt_frame {
    uint32_t tf_eip;
    uint16_t tf_cs;
    uint16_t tf_padding4;
    uint32_t tf_eflags;
};

#endif