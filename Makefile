all: main.o boot.o
	ld -T link.ld -m elf_i386 -o kernel boot.o main.o
	sudo mount -o loop floppy.img /mnt
	sudo cp kernel /mnt
	sudo umount /mnt
main.o:
	gcc -c main.c -nostdlib -nostdinc -fno-builtin -fno-stack-protector -m32
boot.o:
	as -o boot.o boot.s --32
run-floppy:
	bochs -q
clean:
	rm *.o kernel
