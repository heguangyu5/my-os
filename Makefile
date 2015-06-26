CFLAGS=-nostdlib -nostdinc -fno-builtin -fno-stack-protector -m32
ASFLAGS=--32

OBJS=boot.o main.o common-s.o common.o gdt-idt-s.o gdt-idt.o isr.o monitor.o timer.o kheap.o paging-s.o paging.o
 
all: $(OBJS)
	ld -T link.ld -m elf_i386 -o kernel $(OBJS)
	sudo mount -o loop floppy.img /mnt
	sudo cp kernel /mnt
	sudo umount /mnt
boot.o: boot.s
	as -o boot.o boot.s $(ASFLAGS)
main.o: main.c
	gcc -c main.c $(CFLAGS)
common-s.o: common.s
	as -o common-s.o common.s $(ASFLAGS)
common.o: common.c
	gcc -c common.c $(CFLAGS)
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
kheap.o: kheap.c
	gcc -c kheap.c $(CFLAGS)
paging-s.o: paging.s
	as -o paging-s.o paging.s $(ASFLAGS)
paging.o: paging.c
	gcc -c paging.c $(CFLAGS)
run-floppy:
	bochs -q
clean:
	rm *.o kernel
