#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>

#include "disassembler.h"
#include "architecture.h"
#include "cpu.h"
#include "debugger.h"

uint16_t tmp;
CPU cpu;

int main(int argc, char *argv[])
{	
	if(argc < 2)
	{
		printf("Usage: %s <file>\n", argv[0]);
		return -1;
	}
	
	if(initCPU(&cpu, 0x10000) < 0)
		return -1;
	printf("CPU initialized\n");

	uint16_t programstart = 0x100;

	printf("Loading Program %s\n", argv[1]);
	int programsize = loadBinary(&cpu, argv[1], programstart);
	if(programsize < 0)
		return -1;
	printf("read %zd bytes\n", programsize);

	printf("Starting Debugger at entrypoint %d\n", programstart);
	runDebugger(&cpu, programstart);

	printf("CPU halted\n");

	return 0;
}