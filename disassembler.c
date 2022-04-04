#include "disassembler.h"

const char* next_instruction(uint8_t * mem, int index)
{
	return DISASSEMBLY_TABLE[mem[index]];
}