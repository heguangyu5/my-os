#ifndef COMMON_H
#define COMMON_H

typedef unsigned int 	u32int;
typedef			 int 	s32int;
typedef unsigned short	u16int;
typedef 		 short  s16int;
typedef unsigned char	u8int;
typedef 		 char	s8int;

void outb(u16int port, u8int value);
u8int inb(u16int port);
u16int inw(u16int port);

void memcpy(void *dest, void *src, u32int len);
void memset(void *dest, u8int val, u32int len);

#define PANIC(msg) panic(msg, __FILE__, __LINE__)
#define ASSERT(b) ((b) ? (void)0 : panic_assert(__FILE__, __LINE__, #b))

void panic(char *msg, char *file, u32int line);
void panic_assert(char *file, u32int line, char *desc);

#endif
