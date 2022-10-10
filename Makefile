OSIMG = labOS.img
KERNEL = main.o
LIBS = screen.o mem.o init.o int_handlers.o
V = @
OS_NAME = $(shell uname)

GCC_FLAGS = -O -march=i386 -m32 -g -fno-builtin -fno-PIC -Wall -nostdinc -fno-stack-protector -ffreestanding
# -O优化才能真正inline...
GCC_FLAGS += -ffunction-sections
# 函数分段配合ld脚本固定main函数位置
GCC_FLAGS += -mgeneral-regs-only -mno-red-zone
# https://gcc.gnu.org/onlinedocs/gcc-7.5.0/gcc/x86-Function-Attributes.html#x86-Function-Attributes

.DEFAULT_GOAL = all

boot.o: boot.s
	$(V)nasm $< -o $@
setup.o: setup.s
	$(V)nasm $< -o $@

%.o: %.c
	$(V)gcc $< $(GCC_FLAGS) -c -o $@

main.out: $(KERNEL) $(LIBS)
ifeq ($(OS_NAME), Linux)
	$(V)ld -nostdlib -T main.ld $< $(LIBS) -o $@.elf
	$(V)objdump -S $@.elf > $@.asm
	$(V)objcopy -S -O binary $@.elf $@
else
	$(V)echo "need fix"
endif

$(OSIMG): boot.o setup.o main.out
	$(V)dd if=/dev/zero of=$@ count=100
	$(V)dd if=$< of=$@ conv=notrunc
	$(V)dd if=setup.o of=$@ seek=1 conv=notrunc
	$(V)dd if=main.out of=$@ seek=2 conv=notrunc

all: $(OSIMG)

clean:
	$(V)rm *.img
	$(V)rm *.o
	$(V)rm *.out
	$(V)rm *.asm
	$(V)rm *.elf