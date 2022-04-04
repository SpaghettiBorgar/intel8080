#include <stdio.h>
#include "disassembler.h"

int instructionToString(char* buf, uint8_t* mem, int addr)
{
	const uint8_t opcode = mem[addr];
	switch(INSTRUCTION_LENGTHS[opcode])
	{
		case 1:
			return sprintf(buf, "%s", DISASSEMBLY_TABLE[opcode]);
		case 2:
			return sprintf(buf, "%s 0x%02X", DISASSEMBLY_TABLE[opcode], mem[addr + 1]);
		case 3:
			return sprintf(buf, "%s 0x%02X 0x%02X", DISASSEMBLY_TABLE[opcode], mem[addr + 1], mem[addr + 1]);
	}
}