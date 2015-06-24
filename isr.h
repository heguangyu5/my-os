#include "common.h"

typedef struct { 
	u32int ds; // push ds
	u32int edi, esi, ebp, esp, ebx, edx, ecx, eax; // pusha
	u32int int_no, err_code; // pushed by isrX
	u32int eip, cs, eflags, useresp, ss; // pushed by the processor automatically
} registers_t;
