#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct CPU
{
	uint32_t memsize;
	uint8_t* mem;
	uint8_t ports[0x100];
	bool interrupts_enabled;

#ifdef USEREGISTERS
	register uint_fast8_t PC asm("r12");
	register uint_fast8_t SP asm("r13");
	register uint_fast8_t A  asm("r14");
	register uint_fast8_t F	 asm("r15");
#else
	uint_fast16_t PC;
	uint_fast16_t SP;
	uint_fast8_t A;
	uint_fast8_t F;
#endif
	uint_fast8_t B;
	uint_fast8_t C;
	uint_fast8_t D;
	uint_fast8_t E;
	uint_fast8_t H;
	uint_fast8_t L;

	bool running;
} CPU;

int initCPU(CPU* cpu, uint32_t memsize);

int loadBinary(CPU* cpu, char* path, uint16_t offset);

void executeInstruction(CPU* cpu, uint8_t instruction);

void injectSyscalls(CPU* cpu);