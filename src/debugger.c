#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include "debugger.h"
#include "cpu.h"
#include "disassembler.h"
#include "util/hashset.h"

bool stepping;
volatile bool interrupted = false;
CPU* _cpu;
#define MAX_BREAKPOINTS 0x100
Breakpoint breakpoints[MAX_BREAKPOINTS];
uint16_t numBreakpoints;

void handleInterrupt(int _)
{
	if(stepping)
	{
		printf("\nQuitting\n");
		exit(0);
	}
	else
	{
		interrupted = true;
		paus();
	}
}

char* trim(char* str)
{
	if(str == NULL)
		return NULL;

	while(isspace(str[0]))
		++str;
	int i = strlen(str) - 1;
	while(isspace(str[i]))
		--i;
	str[i + 1] = '\0';

	return str;
}

int parseRange(char* str, long* a, long* b, int base)
{
	if(str == NULL || *str == '\0')
		return -1;
	
	char* srange1 = trim(strtok(str, "-"));
	char* srange2 = trim(strtok(NULL, ""));
	char* invalid1 = NULL;
	char* invalid2 = NULL;

	errno = 0;
	*a = strtol(srange1, &invalid1, base);
	if(srange2 != NULL && *srange2 != '\0')
	{
		*b = strtol(srange2, &invalid2, base);
		invalid2 = "";
	}
	else
	{
		*b = *a;
	}

	if(invalid1 == NULL || *invalid1 != '\0' || invalid2 == NULL || *invalid2 != '\0')
	{
		return -1;
	}
	else if(errno != 0)
	{
		perror("strtol");
		return -1;
	}
	else
	{
		return 0;
	}
}

int runDebugger(CPU* cpu, uint16_t entrypoint, bool stopAtEntry)
{
	stepping = stopAtEntry;
	_cpu = cpu;

	signal(SIGINT, handleInterrupt);

	_cpu->PC = entrypoint;
	_cpu->running = true;

	while(_cpu->running)
	{
		do
		{
			interrupted = false;
			char strbuf[20];
			instructionToString(strbuf, _cpu->mem, _cpu->PC);
			printf("%04lX: %s\n", _cpu->PC, strbuf);

			if(interrupted)
				printf("Interrupted. Press ^C again to quit\n");
		} while(interrupted);

		if(stepping)
		{
			while(true)
			{
				printf("> ");
				fflush(stdin);

				char* line = NULL;
				size_t len;
				ssize_t nread = getline(&line, &len, stdin);

				char* cmd = strtok(line, " \n\t");
				if(cmd == NULL)
					cmd = "";

				if(strcmp(cmd, "") == 0 || strcmp(cmd, "s") == 0 || strcmp(cmd, "step") == 0)
				{
					break;
				}
				else if(strcmp(cmd, "c") == 0 || strcmp(cmd, "cont") == 0)
				{
					cont();
					break;
				}
				else if(strcmp(cmd, "r") == 0 || strcmp(cmd, "reg") == 0)
				{
					printRegisters();
				}
				else if(strcmp(cmd, "m") == 0 || strcmp(cmd, "mem") == 0)
				{
					char* srange = trim(strtok(NULL, "=\n"));
					char* sval = trim(strtok(NULL, ""));

					char* srangeMin = trim(strtok(srange, "-"));
					char* srangeMax = trim(strtok(NULL, ""));

					// surely this can be made less convoluted
					char* invalid1 = NULL;
					char* invalid2 = NULL;
					char* invalid3 = NULL;
					errno = 0;
					long rangeMin = strtol(srangeMin, &invalid1, 16);
					long rangeMax;
					if(srangeMax == NULL)
						srangeMax = srangeMin;
					rangeMax = strtol(srangeMax, &invalid2, 16);
					long val;
					if(sval != NULL)
						val = strtol(sval, &invalid3, 16);

					if(errno != 0)
						perror("strtol");
					else if(invalid1 == NULL || invalid2 == NULL || *invalid1 != '\0' || *invalid2 != '\0' || (sval != NULL && (invalid3 == NULL || *invalid3 != '\0')))
						printf("Invalid Syntax\n");
					else
					{
						if(rangeMin > USHRT_MAX || rangeMax > USHRT_MAX || (sval != NULL && val > UCHAR_MAX))
							printf("Values out of range\n");
						else if(sval == NULL)
							printMemory(rangeMin, rangeMax);
						else
							setMemory(rangeMin, rangeMax, val);
					}

				}
				else if(strcmp(cmd, "h") == 0 || strcmp(cmd, "help") == 0)
				{
					listCommands();
				}
				else if(strcmp(cmd, "b") == 0 || strcmp(cmd, "break") == 0)
				{
					char* on_off_s = trim(strtok(NULL, " \t"));
					if(!on_off_s || on_off_s[0] == '\0')
					{
						printBreakpoints();
						goto command_done;
					}
					bool on_off;
					bool remove = false;
					if(strcmp(on_off_s, "rm") == 0)
					{
						remove = true;
					}
					else if(strcmp(on_off_s, "on") == 0)
						on_off = true;
					else if(strcmp(on_off_s, "off") == 0)
						on_off = false;
					else
					{
						printf("Invalid arguments\n");
						goto command_done;
					}

					char* addr_s = trim(strtok(NULL, " \t"));
					char* invalid = NULL;
					long addr = strtol(addr_s, &invalid, 16);
					if(invalid == NULL || *invalid != '\0')
						printf("Invalid Syntax");
					else if(addr > USHRT_MAX || addr < 0)
						printf("Address out of range");
					else
					{
						if(remove)
							removeBreakpoint(addr);
						else
							setBreakpoint((Breakpoint) {addr, on_off, trim(strtok(NULL, ""))});
					}
				}
				else if(strcmp(cmd, "d") == 0 || strcmp(cmd, "disassemble"))
				{
					char * srange = trim(strtok(NULL, " \t\n"));
					uint16_t rangeMin, rangeMax;
					if(srange == NULL || *srange == '\0')
						rangeMin = rangeMax = cpu->PC;
					else
					{
						char* srangeMin = trim(strtok(srange, "-"));
						char* srangeMax = trim(strtok(NULL, ""));
						char* invalid1 = NULL;
						char* invalid2 = NULL;

						if(srangeMax != NULL)
						{
							rangeMin = strtol(srangeMin, &invalid1, 16);
							rangeMax = strtol(srangeMax, &invalid2, 16);
						}
					}

					printDisassemble(cpu->mem, rangeMin, rangeMax);
				}
				else
				{
					printf("Unknown command. Type 'help' for a list of commands\n");
				}

				command_done:
				free(line);
			}
		}
		
		step();
	}

	_cpu = NULL;
	return 0;
}

void listCommands()
{
	printf("List of available commands:\n");
	printf("help/h \t\t\t Show list of commands\n");
	printf("cont/c \t\t\t Continue program execution\n");
	printf("step/s \t\t\t Step to next instruction\n");
	printf("reg/r \t\t\t\t Show register content\n");
	printf("mem/m <range>[=val] \t\t Read/Set Memory at specified range\n");
	printf("break/b <on/off/rm> <addr> [label] \t Set Breakpoint at address\n");
	printf("disassemble/d <range> \t\t Disassemble Memory region\n");
}

void paus()
{
	stepping = true;
}

void cont()
{
	stepping = false;
}

void step()
{
	uint8_t opcode = _cpu->mem[_cpu->PC];
	Breakpoint* bp = getBreakpoint(_cpu->PC);
	_cpu->PC++;
	if(bp && !stepping)
	{
		stepping = true;
		if(bp->label)
			printf("[Breakpoint \"%s\"]\n", bp->label);
		else
		 	printf("[Breakpoint]\n");
	}
	executeInstruction(_cpu, opcode);
}

void printRegisters()
{
	printf("Currents register states:\n");
	printf("PC=0x%04lX\n", _cpu->PC);
	printf("SP=0x%04lX\n", _cpu->SP);
	printf("A=0x%02X\tF=0x%02X\t(PSW)\n", _cpu->A, _cpu->F);
	printf("B=0x%02X\tC=0x%02X\t(BC)\n", _cpu->B, _cpu->C);
	printf("D=0x%02X\tE=0x%02X\t(DE)\n", _cpu->D, _cpu->E);
	printf("H=0x%02X\tL=0x%02X\t(HL)\n", _cpu->H, _cpu->L);
}

void printMemory(uint16_t min, uint16_t max)
{
	// this could be nicer
	uint8_t prev = ~_cpu->mem[min];
	int skipping = 0;
	for(uint16_t i = min; i <= max; ++i)
	{
		uint8_t val = _cpu->mem[i];
		if(val == prev)
		{
			++skipping;
			if(skipping == 2)
				printf("...\n");
		}
		else
		{
			if(skipping == 1)
				printf("%04X: 0x%02X\n", i - 1, prev);
			skipping = 0;
			prev = val;
			printf("%04X: 0x%02X\n", i, val);
		}
	}
	if(skipping)
		printf("%04X: 0x%02X\n", max, _cpu->mem[max]);
}

void setMemory(uint16_t min, uint16_t max, uint8_t val)
{
	for(uint16_t i = min; i <= max; ++i)
	{
		_cpu->mem[i] = val;
	}
	printMemory(min, max);
}

int _compareBreakpointsSearch(const void* a, const void* b)
{
	return *(uint16_t*)a - ((Breakpoint*)b)->addr;
}

int _compareBreakpointsSort(const void* a, const void* b)
{
	return ((Breakpoint*)a)->addr - ((Breakpoint*)b)->addr;
}

Breakpoint* getBreakpoint(uint16_t addr)
{
	return bsearch(&addr, breakpoints, numBreakpoints, sizeof(Breakpoint), &_compareBreakpointsSearch);
}

void setBreakpoint(Breakpoint bp)
{
	Breakpoint* entry = getBreakpoint(bp.addr);
	if(!entry && numBreakpoints >= MAX_BREAKPOINTS)
	{
		printf("Breakpoint limit reached (%d)\n", numBreakpoints);
		return;
	}
	if(entry)
		entry->enabled = bp.enabled;
	else
	{
		breakpoints[numBreakpoints] = (Breakpoint) {bp.addr, bp.enabled, NULL};
		entry = &breakpoints[numBreakpoints++];
	}
	if(bp.label)
	{
		if(entry->label)
			free(entry->label);
		entry->label = malloc(strlen(bp.label + 1));
		if(entry->label)
			strcpy(entry->label, bp.label);
	}

	qsort(breakpoints, numBreakpoints, sizeof(Breakpoint), &_compareBreakpointsSort);

}

void removeBreakpoint(uint16_t addr)
{
	Breakpoint* bpp = getBreakpoint(addr);
	if(!bpp)
		return;
	
	free(bpp->label);
	*bpp = breakpoints[numBreakpoints];
	--numBreakpoints;
	qsort(breakpoints, numBreakpoints, sizeof(Breakpoint), &_compareBreakpointsSort);
}

void printBreakpoints()
{
	printf("Breakpoints:\nAddr | En. | Label\n");
	for(int i = 0; i < numBreakpoints; i++)
	{
		Breakpoint bp = breakpoints[i];
		printf("%04X | %s   | %s\n", bp.addr, bp.enabled ? "*" : " ", bp.label ? bp.label : "");
	}
}