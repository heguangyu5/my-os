CFLAGS=-nostdlib -nostdinc -fno-builtin -fno-stack-protector -m32
ASFLAGS=--32

OBJS=boot.o common.o main.o monitor.o

all: $(OBJS)
	ld -T link.ld -m elf_i386 -o kernel $(OBJS)
	sudo mount -o loop floppy.img /mnt
	sudo cp kernel /mnt
	sudo umount /mnt
boot.o:
	as -o boot.o boot.s $(ASFLAGS)
common.o:
	as -o common.o common.s $(ASFLAGS)
main.o:
	gcc -c main.c $(CFLAGS)
monitor.o:
	gcc -c monitor.c $(CFLAGS)
run-floppy:
	bochs -q
clean:
	rm *.o kernel
