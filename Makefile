CFLAGS=-nostdlib -nostdinc -fno-builtin -fno-stack-protector -m32
ASFLAGS=--32

OBJS=boot.o main.o common.o gdt-idt-s.o gdt-idt.o isr.o monitor.o timer.o
 
all: $(OBJS)
	ld -T link.ld -m elf_i386 -o kernel $(OBJS)
	sudo mount -o loop floppy.img /mnt
	sudo cp kernel /mnt
	sudo umount /mnt
boot.o: boot.s
	as -o boot.o boot.s $(ASFLAGS)
main.o: main.c
	gcc -c main.c $(CFLAGS)
common.o: common.s
	as -o common.o common.s $(ASFLAGS)
gdt-idt-s.o: gdt-idt.s
	as -o gdt-idt-s.o gdt-idt.s $(ASFLAGS)
gdt-idt.o: gdt-idt.c
	gcc -c gdt-idt.c $(CFLAGS)
isr.o: isr.c
	gcc -c isr.c $(CFLAGS)
monitor.o: monitor.c
	gcc -c monitor.c $(CFLAGS)
timer.o: timer.c
	gcc -c timer.c $(CFLAGS)
run-floppy:
	bochs -q
clean:
	rm *.o kernel
