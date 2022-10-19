BIN_DIR = bin
BOOT_DIR = boot
OBJ_DIR = obj
SRC_DIR = src

LIBS = main.o screen.o mem.o picirq.o init.o kbd.o int_handlers.o task.o sys_call.o
USER_LIBS = user.o user_lib.o

OSIMG = $(BIN_DIR)/testos.img
BOOT_BIN = $(BIN_DIR)/boot.bin
SETUP_BIN = $(BIN_DIR)/setup.bin
KERNEL_BIN = $(BIN_DIR)/kernel.bin
USER_BIN = $(BIN_DIR)/user.bin

V = @
OS_NAME = $(shell uname)

GCC_FLAGS = -O -march=i386 -m32 -g -fno-builtin -fno-PIC -Wall -nostdinc -fno-stack-protector -ffreestanding
# -O优化才能真正inline...
GCC_FLAGS += -ffunction-sections
# 函数分段配合ld脚本固定main函数位置
GCC_FLAGS += -mgeneral-regs-only -mno-red-zone
# https://gcc.gnu.org/onlinedocs/gcc-7.5.0/gcc/x86-Function-Attributes.html#x86-Function-Attributes
GCC_FLAGS += -Iinclude

$(BIN_DIR)/%.bin: $(BOOT_DIR)/%.s
	$(V)mkdir -p $(@D)
	$(V)nasm $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(V)mkdir -p $(@D)
	$(V)gcc $< $(GCC_FLAGS) -c -o $@

$(USER_BIN): $(foreach dir,$(USER_LIBS),$(OBJ_DIR)/$(dir))
	$(V)mkdir -p $(@D)
	$(V)ld -nostdlib -Ttools/user.ld $^ -o $@.elf
	$(V)objdump -S $@.elf > $@.asm
	$(V)objcopy -S -O binary $@.elf $@

$(KERNEL_BIN): $(foreach dir,$(LIBS),$(OBJ_DIR)/$(dir))
ifeq ($(OS_NAME), Linux)
	$(V)mkdir -p $(@D)
	$(V)ld -nostdlib -Ttools/kernel.ld $^ -o $@.elf
	$(V)objdump -S $@.elf > $@.asm
	$(V)objcopy -S -O binary $@.elf $@
else
	$(V)echo "Need Fix"
endif

$(OSIMG): $(BOOT_BIN) $(SETUP_BIN) $(KERNEL_BIN) $(USER_BIN)
	$(V)dd if=/dev/zero of=$@ count=100
	$(V)dd if=$(BOOT_BIN) of=$@ conv=notrunc
	$(V)dd if=$(SETUP_BIN) of=$@ seek=1 conv=notrunc
	$(V)dd if=$(KERNEL_BIN) of=$@ seek=2 conv=notrunc
	$(V)dd if=$(USER_BIN) of=$@ seek=33 conv=notrunc

run: $(OSIMG)
	$(V)qemu-system-i386 -drive file=$(OSIMG),media=disk,format=raw -monitor stdio

clean:
	$(V)-rm -rf $(BIN_DIR)
	$(V)-rm -rf $(OBJ_DIR)
