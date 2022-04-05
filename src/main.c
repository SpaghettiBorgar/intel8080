#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>

#include "disassembler.h"
#include "architecture.h"

uint16_t tmp;
uint8_t mem[0x10000];
uint8_t ports[0x100];
bool interrupts_enabled = true;

int main(int argc, char *argv[])
{
	printf("Entering\n");
	
	if(argc < 2)
	{
		printf("Usage: %s <file>\n", argv[0]);
		return -1;
	}
	
	int fd = open(argv[1], O_RDONLY);
	if(fd < 0)
	{
		perror("Error Opening file");
		return -1;
	} 

	ssize_t programsize = 0x100 + read(fd, &mem[0x100], 0x10000 - 0x100);
	if(programsize < 0)
	{
		perror("Error Reading file");
		return -1;
	}
	close(fd);

	printf("Read %zd bytes\n", programsize);

	SP = A = F = B = C = D = E = H = L = 0;
	PC = 0x100;

	bool running = true;

	while(running && PC < programsize)
	{
		uint8_t instruction = mem[PC];
		char strbuf[20];
		instructionToString(strbuf, mem, PC);
		printf("%04X: %s\n", PC, strbuf);

		++PC;

		instruction_switch

	}

	printf("Program terminated\n");

	return 0;
}