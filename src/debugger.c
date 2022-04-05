#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "debugger.h"
#include "cpu.h"
#include "disassembler.h"

int runDebugger(CPU* cpu, uint16_t entrypoint)
{
	cpu->PC = entrypoint;
	cpu->running = true;

	while(cpu->running)
	{
		uint8_t opcode = cpu->mem[cpu->PC];

		char strbuf[20];
		
		instructionToString(strbuf, cpu->mem, cpu->PC);
		printf("%04X: %s\n", cpu->PC, strbuf);

		cpu->PC++;

		executeInstruction(cpu, opcode);
	}
}