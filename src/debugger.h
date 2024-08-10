#include <stdint.h>
#include "cpu.h"

typedef struct
{
	uint16_t addr;
	bool enabled;
	char* label;
} Breakpoint;

int runDebugger(CPU* cpu, uint16_t entrypoint, bool stopAtEntry);

void listCommands();

void paus();

void cont();

void step();

void printRegisters();

void printMemory(uint16_t min, uint16_t max);

void setMemory(uint16_t min, uint16_t max, uint8_t val);

Breakpoint* getBreakpoint(uint16_t addr);

void setBreakpoint(Breakpoint bp);

void removeBreakpoint(uint16_t addr);

int parseRange(char* str, long* a, long* b, int base);

void printBreakpoints();