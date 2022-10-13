#ifndef _PICIRQ_H_
#define _PICIRQ_H_

#define IRQ_OFFSET 0x20
#define IRQ(int_num) (int_num - IRQ_OFFSET)

void pic_enable(unsigned int irq_num);
void pic_ack(void);
void init_pic(void);

#endif