#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include "debugger.h"
#include "cpu.h"
#include "disassembler.h"

bool stepping = true;
volatile bool interrupted = false;

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
		stepping = true;
	}
}

int runDebugger(CPU* cpu, uint16_t entrypoint)
{
	signal(SIGINT, handleInterrupt);

	cpu->PC = entrypoint;
	cpu->running = true;

	while(cpu->running)
	{
		uint8_t opcode = cpu->mem[cpu->PC];

		do
		{
			interrupted = false;
			char strbuf[20];
			instructionToString(strbuf, cpu->mem, cpu->PC);
			printf("%04X: %s\n", cpu->PC, strbuf);

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

				char* tok = strtok(line, " \n\t");
				if(tok == NULL)
					tok = "";

				if(strcmp(tok, "") == 0 || strcmp(tok, "s") == 0 || strcmp(tok, "step") == 0)
				{
					break;
				}
				else if(strcmp(tok, "c") == 0 || strcmp(tok, "cont") == 0)
				{
					stepping = false;
					break;
				}
				else if(strcmp(tok, "r") == 0 || strcmp(tok, "reg") == 0)
				{
					printf("Currents register states:\n");
					printf("PC=0x%04X\n", cpu->PC);
					printf("SP=0x%04X\n", cpu->SP);
					printf("A=0x%02X\tF=0x%02X\t(PSW)\n", cpu->A, cpu->F);
					printf("B=0x%02X\tC=0x%02X\t(BC)\n", cpu->B, cpu->C);
					printf("D=0x%02X\tE=0x%02X\t(DE)\n", cpu->D, cpu->E);
					printf("H=0x%02X\tL=0x%02X\t(HL)\n", cpu->H, cpu->L);
				}
				else if(strcmp(tok, "h") == 0 || strcmp(tok, "help") == 0)
				{
					printf("List of available commands:\n");
					printf("help/h:\t Show list of commands\n");
					printf("cont/c:\t Continue program execution\n");
					printf("step/s:\t Step to next instruction\n");
					printf("reg/r:\t Show register content\n");
				}
				else
				{
					printf("Unknown command. Type 'help' for a list of commands\n");
				}
			}
		}
		
		cpu->PC++;

		executeInstruction(cpu, opcode);
	}
}
