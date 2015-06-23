#include "monitor.h"

int main(void *ptr)
{
	monitor_clear();
	monitor_write("Hello, World!");
	return 0;
}
