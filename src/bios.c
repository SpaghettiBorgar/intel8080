#include <stdio.h>
#include "bios.h"

void printString(CPU* cpu, uint16_t addr)
{
	uint16_t str_addr = (cpu->D << 8) | cpu->E;
	char c;
	while ((c = cpu->mem[str_addr++]) != '$')
		putc(c, stdout);
	fflush(stdout);
}
