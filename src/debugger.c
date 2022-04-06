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

bool stepping = true;
volatile bool interrupted = false;
CPU* _cpu;

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

int runDebugger(CPU* cpu, uint16_t entrypoint)
{
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
			printf("%04X: %s\n", _cpu->PC, strbuf);

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
					step();
				}
				else if(strcmp(cmd, "c") == 0 || strcmp(cmd, "cont") == 0)
				{
					cont();
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
				else
				{
					printf("Unknown command. Type 'help' for a list of commands\n");
				}
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
	printf("help/h \t\t Show list of commands\n");
	printf("cont/c \t\t Continue program execution\n");
	printf("step/s \t\t Step to next instruction\n");
	printf("reg/r \t\t\t Show register content\n");
	printf("mem/m <range>[=val] \t Read/Set Memory at specified range\n");
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
	_cpu->PC++;
	executeInstruction(_cpu, opcode);
}

void printRegisters()
{
	printf("Currents register states:\n");
	printf("PC=0x%04X\n", _cpu->PC);
	printf("SP=0x%04X\n", _cpu->SP);
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