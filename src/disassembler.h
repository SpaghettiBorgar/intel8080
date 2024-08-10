#include <stdint.h>

static const char* DISASSEMBLY_TABLE[0x100] = {
/*        x0/x8       x1/x9       x2/xA       x3/xB       x4/xC      x5/xD       x6/xE       x7/xF       */
/* 0x */  "NOP",      "LXI B",    "STAX B",   "INX B",    "INR B",   "DCR B",    "MVI B",    "RLC",
          "NOP*",     "DAD B",    "LDAX B",   "DCX B",    "INR C",   "DCR C",    "MVI C",    "RRC",
/* 1x */  "NOP*",     "LXI D",    "STAX D",   "INX D",    "INR D",   "DCR D",    "MVI D",    "RAL",
          "NOP*",     "DAD D",    "LDAX D",   "DCX D",    "INR E",   "DCR E",    "MVI E",    "RAR",
/* 2x */  "NOP*",     "LXI H",    "SHLD",     "INX H",    "INR H",   "DCR H",    "MVI H",    "DAA",
          "NOP*",     "DAD H",    "LHLD",     "DCX H",    "INR L",   "DCR L",    "MVI L",    "CMA",
/* 3x */  "NOP*",     "LXI SP",   "STA",      "INX SP",   "INR M",   "DCR M",    "MVI M",    "STC",
          "NOP*",     "DAD SP",   "LDA",      "DCX SP",   "INR A",   "DCR A",    "MVI A",    "CMC",
/* 4x */  "MOV B B",  "MOV B C",  "MOV B D",  "MOV B E",  "MOV B H", "MOV B L",  "MOV B M",  "MOV B A",
          "MOV C B",  "MOV C C",  "MOV C D",  "MOV C E",  "MOV C H", "MOV C L",  "MOV C M",  "MOV C A",
/* 5x */  "MOV D B",  "MOV D C",  "MOV D D",  "MOV D E",  "MOV D H", "MOV D L",  "MOV D M",  "MOV D A",
          "MOV E B",  "MOV E C",  "MOV E D",  "MOV E E",  "MOV E H", "MOV E L",  "MOV E M",  "MOV E A",
/* 6x */  "MOV H B",  "MOV H C",  "MOV H D",  "MOV H E",  "MOV H H", "MOV H L",  "MOV H M",  "MOV H A",
          "MOV L B",  "MOV L C",  "MOV L D",  "MOV L E",  "MOV L H", "MOV L L",  "MOV L M",  "MOV L A",
/* 7x */  "MOV M B",  "MOV M C",  "MOV M D",  "MOV M E",  "MOV M H", "MOV M L",  "HLT",      "MOV M A",
          "MOV A B",  "MOV A C",  "MOV A D",  "MOV A E",  "MOV A H", "MOV A L",  "MOV A M",  "MOV A A",
/* 8x */  "ADD B",    "ADD C",    "ADD D",    "ADD E",    "ADD H",   "ADD L",    "ADD M",    "ADD A",
          "ADC B",    "ADC C",    "ADC D",    "ADC E",    "ADC H",   "ADC L",    "ADC M",    "ADC A",
/* 9x */  "SUB B",    "SUB C",    "SUB D",    "SUB E",    "SUB H",   "SUB L",    "SUB M",    "SUB A",
          "SBB B",    "SBB C",    "SBB D",    "SBB E",    "SBB H",   "SBB L",    "SBB M",    "SBB A",
/* Ax */  "ANA B",    "ANA C",    "ANA D",    "ANA E",    "ANA H",   "ANA L",    "ANA M",    "ANA A",
          "XRA B",    "XRA C",    "XRA D",    "XRA E",    "XRA H",   "XRA L",    "XRA M",    "XRA A",
/* Bx */  "ORA B",    "ORA C",    "ORA D",    "ORA E",    "ORA H",   "ORA L",    "ORA M",    "ORA A",
          "CMP B",    "CMP C",    "CMP D",    "CMP E",    "CMP H",   "CMP L",    "CMP M",    "CMP A",
/* Cx */  "RNZ",      "POP B",    "JNZ",      "JMP",      "CNZ",     "PUSH B",   "ADI",      "RST 0",
          "RZ",       "RET",      "JZ",       "NOP*",     "CZ",      "CALL",     "ACI",      "RST 1",
/* Dx */  "RNC",      "POP D",    "JNC",      "OUT",      "CNC",     "PUSH D",   "SUI",      "RST 2",
          "RC",       "RET*",     "JC",       "IN",       "CC",      "NOP*",     "SBI",      "RST 3",
/* Ex */  "RPO",      "POP H",    "JPO",      "XTHL",     "CPO",     "PUSH H",   "ANI",      "RST 4",
          "RPE",      "PCHL",     "JPE",      "XCHG",     "CPE",     "NOP*",     "XRI",      "RST 5",
/* Fx */  "RP",       "POP PSW",  "JP ",      "DI",       "CP",      "PUSH PSW", "ORI",      "RST 6",
          "RM",       "SPHL",     "JM",       "EI",       "CM",      "NOP*",     "CPI",      "RST 7"
};

static const uint8_t INSTRUCTION_LENGTHS[0x100] = {
	1, 3, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
	1, 3, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
	1, 3, 3, 1, 1, 1, 2, 1, 1, 1, 3, 1, 1, 1, 2, 1,
	1, 3, 3, 1, 1, 1, 2, 1, 1, 1, 3, 1, 1, 1, 2, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 3, 3, 3, 1, 2, 1, 1, 1, 3, 1, 3, 3, 2, 1,
	1, 1, 3, 2, 3, 1, 2, 1, 1, 1, 3, 2, 3, 1, 2, 1,
	1, 1, 3, 1, 3, 1, 2, 1, 1, 1, 3, 1, 3, 1, 2, 1,
	1, 1, 3, 1, 3, 1, 2, 1, 1, 1, 3, 1, 3, 1, 2, 1
};

static const uint8_t INSTRUCTION_CYCLES[0x100] = {
	4,  10, 7,  5,  5,  5,  7,  4,  4,  10, 7,  5,  5,  5,  7,  4,
	4,  10, 7,  5,  5,  5,  7,  4,  4,  10, 7,  5,  5,  5,  7,  4,
	4,  10, 16, 5,  5,  5,  7,  4,  4,  10, 16, 5,  5,  5,  7,  4,
	4,  10, 13, 5,  10, 10, 10, 4,  4,  10, 13, 5,  5,  5,  7,  4,
	5,  5,  5,  5,  5,  5,  7,  5,  5,  5,  5,  5,  5,  5,  7,  5,
	5,  5,  5,  5,  5,  5,  7,  5,  5,  5,  5,  5,  5,  5,  7,  5,
	5,  5,  5,  5,  5,  5,  7,  5,  5,  5,  5,  5,  5,  5,  7,  5,
	7,  7,  7,  7,  7,  7,  7,  7,  5,  5,  5,  5,  5,  5,  7,  5,
	4,  4,  4,  4,  4,  4,  7,  4,  4,  4,  4,  4,  4,  4,  7,  4,
	4,  4,  4,  4,  4,  4,  7,  4,  4,  4,  4,  4,  4,  4,  7,  4,
	4,  4,  4,  4,  4,  4,  7,  4,  4,  4,  4,  4,  4,  4,  7,  4,
	4,  4,  4,  4,  4,  4,  7,  4,  4,  4,  4,  4,  4,  4,  7,  4,
	5,  10, 10, 10, 11, 11, 7,  11, 5,  10, 10, 10, 11, 17, 7,  11,
	5,  10, 10, 10, 11, 11, 7,  11, 5,  10, 10, 10, 11, 17, 7,  11,
	5,  10, 10, 18, 11, 11, 7,  11, 5,  5,  10, 4,  11, 17, 7,  11,
	5,  10, 10, 4,  11, 11, 7,  11, 5,  5,  10, 4,  11, 17, 7,  11 
};

void instructionToString(char* buf, uint8_t* mem, uint16_t addr);

void printDisassemble(uint8_t* mem, uint16_t startAddr, uint16_t endAddr);