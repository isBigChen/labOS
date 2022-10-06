V := @
OSIMG := labOS.img

all: $(OSIMG)

boot.o: boot.s
	$(V)nasm $< -o $@

setup.o: setup.s
	$(V)nasm $< -o $@

main.o: main.c
	$(V)gcc main.c -march=i386 -m32 -fno-builtin -fno-PIC -Wall -nostdinc -fno-stack-protector -ffreestanding -c -o main.o

main.out: main.o
	$(V)ld -nostdlib -T main.ld $< -o $@

$(OSIMG): boot.o setup.o main.out
	$(V)dd if=/dev/zero of=$@ count=100
	$(V)dd if=$< of=$@ conv=notrunc
	$(V)dd if=setup.o of=$@ seek=1 conv=notrunc
	$(V)dd if=main.out of=$@ seek=2 conv=notrunc

clean:
	$(V)rm *.img
	$(V)rm *.o
	$(V)rm *.out
