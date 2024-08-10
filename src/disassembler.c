#include <stdio.h>
#include "disassembler.h"

void instructionToString(char* buf, uint8_t* mem, uint16_t addr)
{
	const uint8_t opcode = mem[addr];
	switch(INSTRUCTION_LENGTHS[opcode])
	{
		case 1:
			sprintf(buf, "%s", DISASSEMBLY_TABLE[opcode]);
			return;
		case 2:
			sprintf(buf, "%s 0x%02X", DISASSEMBLY_TABLE[opcode], mem[addr + 1]);
			return;
		case 3:
			sprintf(buf, "%s 0x%02X 0x%02X", DISASSEMBLY_TABLE[opcode], mem[addr + 1], mem[addr + 2]);
			return;
	}
}

void printDisassemble(uint8_t* mem, uint16_t startAddr, uint16_t endAddr)
{
	uint16_t i = startAddr;
	while(i < endAddr)
	{
		char strbuf[20];
		instructionToString(strbuf, mem, i);
		printf(strbuf);
		i += INSTRUCTION_CYCLES[mem[i]];
	}
}