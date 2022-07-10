#include <stdio.h>
#include "bios.h"

void printString(CPU* cpu, uint16_t addr)
{
	printf(&cpu->mem[(cpu->D << 8) | cpu->E]);
	fflush(stdout);
}