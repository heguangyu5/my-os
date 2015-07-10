CFLAGS=-nostdlib -nostdinc -fno-builtin -fno-stack-protector -m32
ASFLAGS=--32

OBJS=boot.o common-s.o main.o common.o gdt-idt-s.o gdt-idt.o isr.o monitor.o timer.o kheap.o paging-s.o paging.o ordered-array.o fs.o initrd.o process.o task.o syscall.o syscall-s.o
 
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
ordered-array.o: ordered-array.c
	gcc -c ordered-array.c $(CFLAGS)
fs.o: fs.c
	gcc -c fs.c $(CFLAGS)
initrd.o: initrd.c
	gcc -c initrd.c $(CFLAGS)
process.o: process.s
	as -o process.o process.s $(ASFLAGS)
task.o: task.c
	gcc -c task.c $(CFLAGS)
syscall.o: syscall.c
	gcc -c syscall.c $(CFLAGS)
syscall-s.o: syscall.s
	as -o syscall-s.o syscall.s $(ASFLAGS)
mk-initrd:
	-rm mk-initrd initrd.img
	gcc -Wall -o mk-initrd mk-initrd.c
	./mk-initrd file1.txt file-1.txt file2.txt file-2.txt
	sudo mount -o loop floppy.img /mnt
	sudo cp initrd.img /mnt/initrd
	sudo umount /mnt
run-floppy:
	bochs -q
clean:
	rm *.o kernel
