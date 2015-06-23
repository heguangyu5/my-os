#include "common.h"

static u16int *video_memory = (u16int *)0xB8000;

static u8int x = 0;
static u8int y = 0;

// bg: black fg: white
static u16int	attr  = 0x0f00;
static u16int	blank = 0x0f20;

static void move_cursor()
{
	u16int pos = y * 80 + x;
	outb(0x3D4, 14); // cursor pos high
	outb(0x3D5, pos >> 8);
	outb(0x3D4, 15); // cursor pos low
	outb(0x3D5, pos);
}

static void scroll()
{
	if (y < 25) {
		return;
	}
	int i;
	for (i = 0 * 80; i < 24 * 80; i++) {
		video_memory[i] = video_memory[i+80];
	}

	for (i = 24 * 80; i < 25 * 80; i++) {
		video_memory[i] = blank;
	}

	y = 24;
}

void monitor_put(char c)
{
	u16int *pos;

	if (c == 0x08 && x) { // backspace
		x--;
	} else if (c == 0x09) { // tab
		x = (x+8) & ~(8-1); // FIXME: how does this work?
	} else if (c == '\r') {
		x = 0;
	} else if (c == '\n') {
		x = 0;
		y++;
	} else if (c >= ' ') {
		pos = video_memory + y * 80 + x;		
		*pos = attr | c;
		x++;
	}

	if (x >= 80) {
		x = 0;
		y++;
	}

	scroll();
	move_cursor();
}

void monitor_clear()
{
	int i;
	for (i = 0; i< 80 * 25; i++) {
		video_memory[i] = blank;
	}

	x = 0;
	y = 0;
	move_cursor();
}

void monitor_write(char *s)
{
	int i = 0;
	while (s[i]) {
		monitor_put(s[i]);
		i++;
	}
}
