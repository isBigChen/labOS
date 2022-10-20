#include <osdefs.h>

/*
可以参考[ATAPI](https://wiki.osdev.org/ATAPI), 
[PCI IDE Controller](https://wiki.osdev.org/PCI_IDE_Controller)
*/

/* waitdisk - wait for disk ready */
static void
waitdisk(void) { //状态和命令寄存器返回0x40表示准备就绪
    while ((inb(0x1F7) & 0xC0) != 0x40)
        /* do nothing */;
}

/* readsect - read @cnt sectors at @secno into @dst */
static void readsect(void *dst, uint32_t secno) {
    // wait for disk to be ready
    waitdisk();

    outb(0x1F2, 1);                            // 要读写的扇区数，1
    outb(0x1F3, secno & 0xFF);                 // LBA(logical block address)7-0位
    outb(0x1F4, (secno >> 8) & 0xFF);          // 15-8位
    outb(0x1F5, (secno >> 16) & 0xFF);         // 23-16位
    outb(0x1F6, ((secno >> 24) & 0xF) | 0xE0); // 第7位必须为1;第6位为1=LBA模式;第5位必须为1;第4位为0主盘;LBA27-24位
    outb(0x1F7, 0x20);                         // cmd 0x20 - PIO Read, read sectors

    // wait for disk to be ready
    waitdisk();

    // read a sector
    insl(0x1F0, dst, 512 / 4); //除以4，因为以dw为单位
}

void read_sectors(void *dst, uint32_t secno, int cnt) {
    void *dst_tmp = dst;
    for (int i = 0; i < cnt; i++, dst_tmp += 512) {
        readsect(dst_tmp, secno + i);
    }
}