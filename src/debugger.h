#include <stdint.h>
#include "cpu.h"

int runDebugger(CPU* cpu, uint16_t entrypoint);

void listCommands();

void paus();

void cont();

void step();

void printRegisters();

void printMemory(uint16_t min, uint16_t max);

void setMemory(uint16_t min, uint16_t max, uint8_t val);