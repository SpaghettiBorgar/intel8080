#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include "cpu.h"
#include "architecture.h"
#include "bios.h"

uint8_t tmp8;
uint16_t tmp16;

int initCPU(CPU* cpu, uint32_t memsize)
{
	if(memsize > 0x10000)
	{
		perror("Memory size exceeds 16 bit address space");
		return -1;
	}

	cpu->interrupts_enabled = true;
	cpu->running = false;
	cpu->PC = cpu->SP = cpu->A = cpu->B = cpu->C =
		cpu->D = cpu->E = cpu->H = cpu->L = 0;
	cpu->F = FLAGS_INIT;

	cpu->memsize = memsize;
	cpu->mem = (uint8_t*) malloc(memsize * sizeof(uint8_t));
	if(cpu->mem == NULL)
	{
		perror("malloc() failed");
		return -1;
	}

	memset(cpu->mem, 0, cpu->memsize);
	memset(cpu->ports, 0, sizeof(cpu->ports));

	return 0;
}

int loadBinary(CPU* cpu, char* path, uint16_t offset)
{
	int fd = open(path, O_RDONLY);
	if(fd < 0)
	{
		perror("Error Opening file");
		return -1;
	}
	
	ssize_t programsize = read(fd, &cpu->mem[offset], cpu->memsize - offset);
	if(programsize < 0)
	{
		perror("Error Reading file");
		return -1;
	}
	close(fd);

	return programsize;
}

void portIn(CPU* cpu, uint8_t num)
{
	printf("IN %02X <- %02X\n", num, cpu->ports[num]);
}

void portOut(CPU* cpu, uint8_t num)
{
	switch(num)
	{
		case 1:
			putchar(cpu->ports[1]);
			fflush(stdout);
			break;
		default:
			printf("OUT %02X -> %02X\n", num, cpu->ports[num]);
	}
}

void hook(CPU* cpu, uint16_t addr)
{
	switch(addr)
	{
		case 0x0005:
			if(cpu->C == 0x09)
				printString(cpu, addr);
			break;
		default:
			printf("Special return used at %04X\n", addr);
	}
}

void executeInstruction(CPU* cpu, uint8_t instruction)
{
	#define mem cpu->mem
	#define ports cpu->ports
	#define interrupts_enabled cpu->interrupts_enabled
	#define _SP cpu->SP
	#define PC cpu->PC
	#define A cpu->A
	#define F cpu->F
	#define B cpu->B
	#define C cpu->C
	#define D cpu->D
	#define E cpu->E
	#define H cpu->H
	#define L cpu->L
	#define running cpu->running
	#define _portIn(num) portIn(cpu, num);
	#define _portOut(num) portOut(cpu, num);
	#define _hook(addr) hook(cpu, addr)
	
		instruction_switch

	#undef mem
	#undef ports
	#undef interrupts_enabled
	#undef _SP
	#undef PC
	#undef A
	#undef F
	#undef B
	#undef C
	#undef D
	#undef E
	#undef H
	#undef L
	#undef running
	#undef _portIn
	#undef _portOut
	#undef _hook
}

void injectSyscalls(CPU* cpu)
{
	// Inject a non-standard return instructiona (0xD9: RET*) so it behaves just like a normal return except we know we should check if a syscall was done
	cpu->mem[0x0005] = 0xD9;
}