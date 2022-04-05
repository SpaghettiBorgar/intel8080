#define NOP()		{ }
#define LXI(RP)		{ RP( mem[PC] | mem[PC + 1] << 8 ); PC += 2; }
#define STAX(RP)	{ mem[RP##_] = A; }
#define INX(RP)		{ RP(RP##_ + 1); }
#define INR(D)		{ F = F & ~FLAG_AUXC | (D ^ D + 1) & FLAG_AUXC; ++D; SZP(D); }
#define DCR(D)		{ F = F & ~FLAG_AUXC | (D ^ D - 1) & FLAG_AUXC; --D; SZP(D); }
#define MVI(D)		{ D = mem[PC++]; }
#define RLC()		{ uint16_t tmp = A << 1 | A >> 7; F = F & ~FLAG_CARRY | A >> 7; A = tmp; }
#define DAD(RP)		{ uint16_t tmp = HL_; HL(HL_ + RP##_); F = F & ~FLAG_CARRY | tmp > HL_; }
#define LDAX(RP)	{ A = mem[RP##_]; }
#define DCX(RP)		{ RP(RP##_ - 1); }
#define RRC()		{ uint16_t tmp = A >> 1 | (A & 1) << 7; F = F & ~FLAG_CARRY | A & 1; A = tmp; }
#define RAL()		{ uint16_t tmp = A << 1 | F & 1; F = F & ~FLAG_CARRY | A >> 7; A = tmp; }
#define RAR()		{ uint16_t tmp = A >> 1 | (F & 1) << 7; F = F & ~FLAG_CARRY | A & 1; A = tmp; }
#define SHLD()		{ uint16_t tmp = mem[PC] | mem[PC + 1] >> 8; mem[tmp] = L; mem[tmp + 1] = H; PC += 2; }
#define DAA()		{ uint16_t tmp = F & FLAG_CARRY; ADD((F & FLAG_AUXC || (A & 0x0F) > 0x09) * 0x06 + (F & FLAG_CARRY || A > 0x9F || A >= 0x90 && (A & 0x0F) > 0x09) * 0x60); F = F & ~FLAG_CARRY | tmp; }
#define LHLD()		{ uint16_t tmp = mem[PC] | mem[PC + 1] >> 8; L = mem[tmp]; H = mem[tmp + 1]; PC += 2; }
#define CMA()		{ A = ~A; }
#define STA()		{ mem[mem[PC] | mem[PC + 1] << 8] = A; PC += 2; }
#define STC()		{ F |= FLAG_CARRY; }
#define LDA()		{ A = mem[mem[PC] | mem[PC + 1] << 8]; PC += 2; }
#define CMC()		{ F ^= FLAG_CARRY; }
#define HLT()		{ running = false; }
#define MOV(D,S)	{ D = S; }
#define ADD(S)		{ uint16_t tmp = A + S; F &= ~(FLAG_CARRY | FLAG_AUXC); F |= (tmp > 0xFF) | ((tmp ^ A ^ S) & 0x10); A = tmp; SZP(A); }
#define ADC(S)		{ ADD((S + (F & FLAG_CARRY))) }
#define SUB(S)		{ ADD(~S); CMC(); }
#define SBB(S)		{ ADD((~S + !(F & FLAG_CARRY))); CMC(); }
#define ANA(S)		{ A &= S; F &= ~FLAG_CARRY; SZP(A); }
#define XRA(S)		{ A ^= S; F &= ~FLAG_CARRY; SZP(A); }
#define ORA(S)		{ A |= S; F &= ~FLAG_CARRY; SZP(A); }
#define CMP(S)		{ uint16_t tmp = A + ~S; F &= ~(FLAG_CARRY | FLAG_AUXC); F |= (tmp <= 0xFF) | ((tmp ^ A ^ ~S) & 0x10); SZP(tmp); }
#define RNZ()		{ if(!(F & FLAG_ZERO)) RET(); }
#define POP(RP)		{ RP(mem[_SP] | mem[_SP + 1] << 8); _SP += 2; }
#define JNZ()		{ if(!(F & FLAG_ZERO)) JMP(); }
#define JMP()		{ PC = mem[PC] | mem[PC + 1] << 8; }
#define CNZ()		{ if(!(F & FLAG_ZERO)) CALL(); }
#define PUSH(RP)	{ mem[_SP - 2] = RP##_ & 0xFF; mem[_SP - 1] = RP##_ >> 8; _SP -= 2; }
#define ADI()		{ ADD(mem[++PC]); }
#define RST(N)		{ mem[_SP - 1] = PC >> 8; mem[_SP - 2] = PC & 0xFF; _SP -= 2; PC = 0x08 * N; }
#define RZ()		{ if(F & FLAG_ZERO) RET(); }
#define RET()		{ PC = mem[_SP] | mem[_SP + 1] << 8; _SP += 2; }
#define JZ()		{ if(F & FLAG_ZERO) JMP(); }
#define CZ()		{ if(F & FLAG_ZERO) CALL(); }
#define CALL()		{ mem[_SP - 1] = PC >> 8; mem[_SP - 2] = PC & 0xFF; _SP -= 2; JMP(); }
#define ACI()		{ ADC(mem[++PC]); }
#define RNC()		{ if(!(F & FLAG_CARRY)) RET(); }
#define JNC()		{ if(!(F & FLAG_CARRY)) JMP(); }
#define OUT()		{ ports[mem[++PC]] = A; }
#define CNC()		{ if(!(F & FLAG_CARRY)) CALL(); }
#define SUI()		{ SUB(mem[++PC]); }
#define RC()		{ if(F & FLAG_CARRY) RET(); }
#define JC()		{ if(F & FLAG_CARRY) JMP(); }
#define IN()		{ A = ports[mem[++PC]]; }
#define CC()		{ if(F & FLAG_CARRY) CALL(); }
#define SBI()		{ SBB(mem[++PC]); }
#define RPO()		{ if(F & FLAG_PARITY) RET(); }
#define JPO()		{ if(F & FLAG_PARITY) JMP(); }
#define XTHL()		{ uint16_t tmp = _SP; _SP = HL_; HL(tmp); }
#define CPO()		{ if(F & FLAG_PARITY) CALL(); }
#define ANI()		{ A &= mem[++PC]; F = F & ~(FLAG_CARRY | FLAG_AUXC); SZP(A); }
#define RPE()		{ if(!(F & FLAG_PARITY)) RET(); }
#define PCHL()		{ uint16_t tmp = PC; PC = HL_; HL(tmp); }
#define JPE()		{ if(!(F & FLAG_PARITY)) JMP(); }
#define XCHG()		{ uint16_t tmp = HL_; HL(DE_); DE(tmp); }
#define CPE()		{ if(!(F & FLAG_PARITY)) CALL(); }
#define XRI()		{ A ^= mem[++PC]; F = F & ~(FLAG_CARRY | FLAG_AUXC); SZP(A); }
#define RP()		{ if(!(F & FLAG_SIGN)) RET(); }
#define JP()		{ if(!(F & FLAG_SIGN)) JMP(); }
#define DI()		{ interrupts_enabled = false; }
#define CP()		{ if(!(F & FLAG_SIGN)) CALL(); }
#define ORI()		{ A |= mem[++PC]; F = F & ~(FLAG_CARRY | FLAG_AUXC); SZP(A); }
#define RM()		{ if(F & FLAG_SIGN) RET(); }
#define SPHL()		{ SP(HL_); }
#define JM()		{ if(F & FLAG_SIGN) JMP(); }
#define EI()		{ interrupts_enabled = true; }
#define CM()		{ if(F & FLAG_SIGN) CALL(); }
#define CPI()		{ CMP(mem[++PC]); }

#define M		mem[HL_]
#define PSW_	( (A << 8) | F )
#define PSW(v)	{ A = (v) >> 8; F = v & 0xFF; }
#define BC_		( (B << 8) | C )
#define BC(v)	{ B = (v) >> 8; C = v & 0xFF; }
#define DE_		( (D << 8) | E )
#define DE(v)	{ D = (v) >> 8; E = v & 0xFF; }
#define HL_		( (H << 8) | L )
#define HL(v)	{ H = (v) >> 8; L = v & 0xFF; }
#define SP_		(_SP)
#define SP(v)	{ _SP = (v); }

#define FLAGS_INIT	0b00000010
#define FLAG_SIGN 	0b10000000
#define FLAG_ZERO 	0b01000000
#define FLAG_AUXC 	0b00010000
#define FLAG_PARITY	0b00000100
#define FLAG_CARRY	0b00000001

#define SZP(R)	{ F = (F & ~(FLAG_SIGN | FLAG_ZERO | FLAG_PARITY)) | (R & 0b10000000) | (R == 0) << 6 | (1 ^ R ^ R>>1 ^ R>>2 ^ R>>3 ^ R>>4 ^ R>>5 ^ R>>6 ^ R>>7) & 1;}

#define opc(code, instr)	case code: instr; break;

#define instruction_switch \
	switch(instruction) \
	{ \
		opc(0x00, NOP()); \
		opc(0x01, LXI(BC)); \
		opc(0x02, STAX(BC)); \
		opc(0x03, INX(BC)); \
		opc(0x04, INR(B)); \
		opc(0x05, DCR(B)); \
		opc(0x06, MVI(B)); \
		opc(0x07, RLC()); \
		opc(0x08, NOP()); \
		opc(0x09, DAD(BC)); \
		opc(0x0a, LDAX(BC)); \
		opc(0x0b, DCX(BC)); \
		opc(0x0c, INR(C)); \
		opc(0x0d, DCR(C)); \
		opc(0x0e, MVI(C)); \
		opc(0x0f, RRC()); \
		opc(0x10, NOP()); \
		opc(0x11, LXI(DE)); \
		opc(0x12, STAX(DE)); \
		opc(0x13, INX(DE)); \
		opc(0x14, INR(D)); \
		opc(0x15, DCR(D)); \
		opc(0x16, MVI(D)); \
		opc(0x17, RAL()); \
		opc(0x18, NOP()); \
		opc(0x19, DAD(DE)); \
		opc(0x1a, LDAX(DE)); \
		opc(0x1b, DCX(DE)); \
		opc(0x1c, INR(D)); \
		opc(0x1d, DCR(D)); \
		opc(0x1e, MVI(D)); \
		opc(0x1f, RAR()); \
		opc(0x20, NOP()); \
		opc(0x21, LXI(HL)); \
		opc(0x22, SHLD()); \
		opc(0x23, INX(HL)); \
		opc(0x24, INR(H)); \
		opc(0x25, DCR(H)); \
		opc(0x26, MVI(H)); \
		opc(0x27, DAA()); \
		opc(0x28, NOP()); \
		opc(0x29, DAD(HL)); \
		opc(0x2a, LHLD()); \
		opc(0x2b, DCX(HL)); \
		opc(0x2c, INR(L)); \
		opc(0x2d, DCR(L)); \
		opc(0x2e, MVI(L)); \
		opc(0x2f, CMA()); \
		opc(0x30, NOP()); \
		opc(0x31, LXI(SP)); \
		opc(0x32, STA()); \
		opc(0x33, INX(SP)); \
		opc(0x34, INR(M)); \
		opc(0x35, DCR(M)); \
		opc(0x36, MVI(M)); \
		opc(0x37, STC()); \
		opc(0x38, NOP()); \
		opc(0x39, DAD(SP)); \
		opc(0x3a, LDA()); \
		opc(0x3b, DCX(SP)); \
		opc(0x3c, INR(A)); \
		opc(0x3d, DCR(A)); \
		opc(0x3e, MVI(A)); \
		opc(0x3f, CMC()); \
		opc(0x40, MOV(B,B)); \
		opc(0x41, MOV(B,C)); \
		opc(0x42, MOV(B,D)); \
		opc(0x43, MOV(B,E)); \
		opc(0x44, MOV(B,H)); \
		opc(0x45, MOV(B,L)); \
		opc(0x46, MOV(B,M)); \
		opc(0x47, MOV(B,A)); \
		opc(0x48, MOV(C,B)); \
		opc(0x49, MOV(C,C)); \
		opc(0x4a, MOV(C,D)); \
		opc(0x4b, MOV(C,E)); \
		opc(0x4c, MOV(C,H)); \
		opc(0x4d, MOV(C,L)); \
		opc(0x4e, MOV(C,M)); \
		opc(0x4f, MOV(C,A)); \
		opc(0x50, MOV(D,B)); \
		opc(0x51, MOV(D,C)); \
		opc(0x52, MOV(D,D)); \
		opc(0x53, MOV(D,E)); \
		opc(0x54, MOV(D,H)); \
		opc(0x55, MOV(D,L)); \
		opc(0x56, MOV(D,M)); \
		opc(0x57, MOV(D,A)); \
		opc(0x58, MOV(E,B)); \
		opc(0x59, MOV(E,C)); \
		opc(0x5a, MOV(E,D)); \
		opc(0x5b, MOV(E,E)); \
		opc(0x5c, MOV(E,H)); \
		opc(0x5d, MOV(E,L)); \
		opc(0x5e, MOV(E,M)); \
		opc(0x5f, MOV(E,A)); \
		opc(0x60, MOV(H,B)); \
		opc(0x61, MOV(H,C)); \
		opc(0x62, MOV(H,D)); \
		opc(0x63, MOV(H,E)); \
		opc(0x64, MOV(H,H)); \
		opc(0x65, MOV(H,L)); \
		opc(0x66, MOV(H,M)); \
		opc(0x67, MOV(H,A)); \
		opc(0x68, MOV(L,B)); \
		opc(0x69, MOV(L,C)); \
		opc(0x6a, MOV(L,D)); \
		opc(0x6b, MOV(L,E)); \
		opc(0x6c, MOV(L,H)); \
		opc(0x6d, MOV(L,L)); \
		opc(0x6e, MOV(L,M)); \
		opc(0x6f, MOV(L,A)); \
		opc(0x70, MOV(M,B)); \
		opc(0x71, MOV(M,C)); \
		opc(0x72, MOV(M,D)); \
		opc(0x73, MOV(M,E)); \
		opc(0x74, MOV(M,H)); \
		opc(0x75, MOV(M,L)); \
		opc(0x76, HLT()); \
		opc(0x77, MOV(M,A)); \
		opc(0x78, MOV(A,B)); \
		opc(0x79, MOV(A,C)); \
		opc(0x7a, MOV(A,D)); \
		opc(0x7b, MOV(A,E)); \
		opc(0x7c, MOV(A,H)); \
		opc(0x7d, MOV(A,L)); \
		opc(0x7e, MOV(A,M)); \
		opc(0x7f, MOV(A,A)); \
		opc(0x80, ADD(B)); \
		opc(0x81, ADD(C)); \
		opc(0x82, ADD(D)); \
		opc(0x83, ADD(E)); \
		opc(0x84, ADD(H)); \
		opc(0x85, ADD(L)); \
		opc(0x86, ADD(M)); \
		opc(0x87, ADD(A)); \
		opc(0x88, ADC(B)); \
		opc(0x89, ADC(C)); \
		opc(0x8a, ADC(D)); \
		opc(0x8b, ADC(E)); \
		opc(0x8c, ADC(H)); \
		opc(0x8d, ADC(L)); \
		opc(0x8e, ADC(M)); \
		opc(0x8f, ADC(A)); \
		opc(0x90, SUB(B)); \
		opc(0x91, SUB(C)); \
		opc(0x92, SUB(D)); \
		opc(0x93, SUB(E)); \
		opc(0x94, SUB(H)); \
		opc(0x95, SUB(L)); \
		opc(0x96, SUB(M)); \
		opc(0x97, SUB(A)); \
		opc(0x98, SBB(B)); \
		opc(0x99, SBB(C)); \
		opc(0x9a, SBB(D)); \
		opc(0x9b, SBB(E)); \
		opc(0x9c, SBB(H)); \
		opc(0x9d, SBB(L)); \
		opc(0x9e, SBB(M)); \
		opc(0x9f, SBB(A)); \
		opc(0xa0, ANA(B)); \
		opc(0xa1, ANA(C)); \
		opc(0xa2, ANA(D)); \
		opc(0xa3, ANA(E)); \
		opc(0xa4, ANA(H)); \
		opc(0xa5, ANA(L)); \
		opc(0xa6, ANA(M)); \
		opc(0xa7, ANA(A)); \
		opc(0xa8, XRA(B)); \
		opc(0xa9, XRA(C)); \
		opc(0xaa, XRA(D)); \
		opc(0xab, XRA(E)); \
		opc(0xac, XRA(H)); \
		opc(0xad, XRA(L)); \
		opc(0xae, XRA(M)); \
		opc(0xaf, XRA(A)); \
		opc(0xb0, ORA(B)); \
		opc(0xb1, ORA(C)); \
		opc(0xb2, ORA(D)); \
		opc(0xb3, ORA(E)); \
		opc(0xb4, ORA(H)); \
		opc(0xb5, ORA(L)); \
		opc(0xb6, ORA(M)); \
		opc(0xb7, ORA(A)); \
		opc(0xb8, CMP(B)); \
		opc(0xb9, CMP(C)); \
		opc(0xba, CMP(D)); \
		opc(0xbb, CMP(E)); \
		opc(0xbc, CMP(H)); \
		opc(0xbd, CMP(L)); \
		opc(0xbe, CMP(M)); \
		opc(0xbf, CMP(A)); \
		opc(0xc0, RNZ()); \
		opc(0xc1, POP(BC)); \
		opc(0xc2, JNZ()); \
		opc(0xc3, JMP()); \
		opc(0xc4, CNZ()); \
		opc(0xc5, PUSH(BC)); \
		opc(0xc6, ADI()); \
		opc(0xc7, RST(0)); \
		opc(0xc8, RZ()); \
		opc(0xc9, RET()); \
		opc(0xca, JZ()); \
		opc(0xcb, JMP()); \
		opc(0xcc, CZ()); \
		opc(0xcd, CALL()); \
		opc(0xce, ACI()); \
		opc(0xcf, RST(1)); \
		opc(0xd0, RNC()); \
		opc(0xd1, POP(DE)); \
		opc(0xd2, JNC()); \
		opc(0xd3, OUT()); \
		opc(0xd4, CNC()); \
		opc(0xd5, PUSH(DE)); \
		opc(0xd6, SUI()); \
		opc(0xd7, RST(2)); \
		opc(0xd8, RC()); \
		opc(0xd9, RET()); \
		opc(0xda, JC()); \
		opc(0xdb, IN()); \
		opc(0xdc, CC()); \
		opc(0xdd, CALL()); \
		opc(0xde, SBI()); \
		opc(0xdf, RST(3)); \
		opc(0xe0, RPO()); \
		opc(0xe1, POP(HL)); \
		opc(0xe2, JPO()); \
		opc(0xe3, XTHL()); \
		opc(0xe4, CPO()); \
		opc(0xe5, PUSH(HL)); \
		opc(0xe6, ANI()); \
		opc(0xe7, RST(4)); \
		opc(0xe8, RPE()); \
		opc(0xe9, PCHL()); \
		opc(0xea, JPE()); \
		opc(0xeb, XCHG()); \
		opc(0xec, CPE()); \
		opc(0xed, CALL()); \
		opc(0xee, XRI()); \
		opc(0xef, RST(5)); \
		opc(0xf0, RP()); \
		opc(0xf1, POP(PSW)); \
		opc(0xf2, JP()); \
		opc(0xf3, DI()); \
		opc(0xf4, CP()); \
		opc(0xf5, PUSH(PSW)); \
		opc(0xf6, ORI()); \
		opc(0xf7, RST(6)); \
		opc(0xf8, RM()); \
		opc(0xf9, SPHL()); \
		opc(0xfa, JM()); \
		opc(0xfb, EI()); \
		opc(0xfc, CM()); \
		opc(0xfd, CALL()); \
		opc(0xfe, CPI()); \
		opc(0xff, RST(7)); \
	}
