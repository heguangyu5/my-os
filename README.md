[JamesM's kernel development tutorials: Roll your own toy UNIX-clone OS](http://www.jamesmolloy.co.uk/tutorial_html/index.html)

    readelf -s kernel  | egrep 'main|monitor_write'
    objdump -S kernle > /tmp/a.txt

# 虚拟环境

- x86-32
- grub-0.97

# 关键点

1. grub根据menu.lst的kernel命令将指定kernel的前8192字节读到buffer里,然后查找MULTIBOOT标记.

        boot.s 里写入了 MULTIBOOT_MAGIC 标记

    然后根据kernel的ELF header找到执行入口

        link.ld里指明了entry是_start

    然后将kernel(code+data+bss)加载到内存1M处

        stage2/builtins.c
        stage2/boot.c
        stage2/mb_header.h
        stage2/i386-elf.h

2. grub根据menu.lst的module命令将指定的module加载到内存

        initrd

3. grub执行boot命令

        multi_boot ((int) entry_addr, (int) &mbi);

    multi\_boot将 0x2BADB002 存入 %eax, 将 mbi 的内存地址存入 %ebx, 然后 call entry\_addr

        movl $0x2BADB002, %eax
        movl 0x8(%esp), %ebx
        call *0x4(%esp)

    如果在执行入口处设break_point,那么就会看到eax和ebx的值正如上边所述,然后在boot.s里

        push %esp
        push %ebx
        call main

    使得main函数有了这样的参数

        int main(struct multiboot *mboot_ptr, u32int initial_stack)

4. x86-32内存寻址(MMU)先要过段机制,然后过分页机制,最后才能得到内存的物理地址

    简单地将内存段设成0-4G,然后启用分页.

    至少需要设置5个段,一个全为0的,一个kernel code, 一个kernel data, 一个user code, 一个user data.

    后边实现syscall时还需要一个tss_entry,也要加到gdt里,加起来一共6个段.

    在内存里开一块区域保存段设置

        gdt_entry_t gdt_entries[6];

    并把它的内存地址放到gdtr里

        gdt_flush:
            lgdt (%eax)

5. 有3种类型的中断,一种来自CPU(ISR),一种来自PIC(IRQ),还有一种来自程序自身(int)

    当中断发生时,要有一个callback来处理,最多可以有256个中断

    在内存里开一块区域保存每个中断的callback

        idt_entry_t idt_entries[256];

    并把它的内存地址放到idtr里

        idt_flush:
            lidt (%eax)

    中断调用有权限控制,比如为了在user mode能调用 int $0x80, 需将此中断callback设为可以user mode调用,否则就报GPF

        idt_set(128, (u32int)isr128, 0x08, 0xEE);

6. 将内存分页(4K),然后使用bitmap记录内存的使用情况,这样512字节的bitmap就可以记录16M内存的使用情况

        512 * 8 * 4K = 16M

7. 使用两级page table来map内存,先map内核使用的内存,这样使得virt addr = phys addr

8. clone出kernel page directory,这样后边再map新的内存时能分得出来哪些是kernel的,哪些是其它的

9. 启用分页就是把 page_directory 的物理地址存入cr3,然后在cr0里打开分页标记

10. 之前我们一直使用grub设置的stack,把stack移到虚拟地址里来,使用page table管理起来,这样每clone一个新的page directory时,就新创建了一个stack

11. 使用PIT,可以实现多任务切换

12. 进入user mode其实就是修改段寄存器,指向gdt里设置的 user code 和 user data

    在user mode要想访问硬件资源,必须通过中断进入kernel mode才可以

    在user mode可以执行kernel的代码(之前设page table时设的权限),但当写内存或在屏幕上显示字符时,没权限,报错了
