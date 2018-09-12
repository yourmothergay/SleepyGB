/*
 * Copyright 2018 duck
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <SleepyGB.hpp>
#include <stdexcept>
#include "ALU.cpp"
#include "PrefixCB.cpp"

#define DEFINEOPCODE(name) \
void name(SleepyZ80 *z80, Registers *regs, Cycles *c, Memory *mem, u8 opcode)

DEFINEOPCODE(NOP) {}

DEFINEOPCODE(LDADDRSP) {
	mem->Write16(mem->Read16(regs->pc), regs->sp);
	regs->pc += 2;
	*c += 16;
}

DEFINEOPCODE(LDR1IMM16) {
	u16 *dest[] = { &regs->bc, &regs->de, &regs->hl, &regs->sp };
	*(dest[opcode >> 4]) = mem->Read16(regs->pc++);
	*c += 8;
}

DEFINEOPCODE(ADDHLR1) {
	u16 *dest[] = { &regs->bc, &regs->de, &regs->hl, &regs->sp };
	
	u32 hl = regs->hl;
	u32 d = *(dest[opcode >> 4]);
	
	regs->f &= 128;
	
	if((hl & 0xF) + (d & 0xF) > 0x10) SETBIT(regs->f, Flags::HalfCarry);
	if(hl + d > 0xFF) SETBIT(regs->f, Flags::Carry);
	RESETBIT(regs->f, Flags::Subtract);
	
	regs->hl += d;
	*c += 4;
}

DEFINEOPCODE(LDBCDEA) {
	u16 *dest[] = { &regs->bc, &regs->de };
	switch(GETBIT(opcode, 3)) {
	case 0:
		mem->Write8(*(dest[GETBIT(opcode, 4)]), regs->a);
		break;
	case 1:
		regs->a = mem->Read8(*(dest[GETBIT(opcode, 4)]));
		break;
	}
	*c += 4;
}

DEFINEOPCODE(INCR1) {
	u8 *dest[] = { 	&regs->b, &regs->c, &regs->d, &regs->e, &regs->h, &regs->l,
					nullptr, &regs->a };
	
	u8 *d = dest[opcode >> 3];
	
	if(d == nullptr) d = mem->Read8Address(regs->hl), *c += 8;
	
	++*d;
	
	if(!*d) SETBIT(regs->f, Flags::Zero);
	RESETBIT(regs->f, Flags::Subtract);
	if(((*d & 0xF) + 1) > 0xF) SETBIT(regs->f, Flags::HalfCarry);
}

DEFINEOPCODE(DECR1) {
	u8 *dest[] = { 	&regs->b, &regs->c, &regs->d, &regs->e, &regs->h, &regs->l,
					nullptr, &regs->a };
	
	u8 *d = dest[opcode >> 3];
	
	if(d == nullptr) d = mem->Read8Address(regs->hl), *c += 8;
	
	--*d;
	
	regs->f &= 32;
	
	if(!*d) SETBIT(regs->f, Flags::Zero);
	SETBIT(regs->f, Flags::Subtract);
	if((*d & 0xF) < 1) SETBIT(regs->f, Flags::HalfCarry);
}

DEFINEOPCODE(LDR1IMM) {
	u8 *dest[] = { 	&regs->b, &regs->c, &regs->d, &regs->e, &regs->h, &regs->l,
					nullptr, &regs->a };
	
	u8 *d = dest[opcode >> 3];
	
	if(d == nullptr) d = mem->Read8Address(regs->hl), *c += 4;
	
	*d = mem->Read8(regs->pc++);
	
	*c += 4;
}

DEFINEOPCODE(RxCA) {
	regs->f = 0;
	
	switch(GETBIT(opcode, 3)) {
	case 0:
		if(TESTBIT(regs->a, 7)) SETBIT(regs->f, Flags::Carry);
		regs->a = (regs->a << 1);
		break;
	case 1:
		if(TESTBIT(regs->a, 0)) SETBIT(regs->f, Flags::Carry);
		regs->a = (regs->a >> 1);
		break;
	}
	
	if(!regs->a) SETBIT(regs->f, Flags::Zero);
}

DEFINEOPCODE(RxA) {
	regs->f = 0;
	
	u8 oldcarry = GETBIT(regs->f, Flags::Carry);
	
	switch(GETBIT(opcode, 3)) {
	case 0:
		if(TESTBIT(regs->a, 7)) SETBIT(regs->f, Flags::Carry);
		regs->a = (regs->a << 1) | oldcarry;
		break;
	case 1:
		if(TESTBIT(regs->a, 0)) SETBIT(regs->f, Flags::Carry);
		regs->a = (regs->a >> 1) | (oldcarry << 7);
		break;
	}
	
	if(!regs->a) SETBIT(regs->f, Flags::Zero);
}

DEFINEOPCODE(STOP) {
	z80->Stop();
}

DEFINEOPCODE(JUMPRELATIVE) {
	regs->pc += (s8)mem->Read8(regs->pc++);
	*c += 4;
}

DEFINEOPCODE(JUMPRELATIVEWITHCONDITION) {
	regs->pc += (s8)mem->Read8(regs->pc++);

	switch((opcode >> 3) & 0b11) {
	case 0:
		if(!TESTBIT(regs->a, Flags::Zero))
			regs->pc += (s8)mem->Read8(regs->pc++);
		break;
	case 1:
		if( TESTBIT(regs->a, Flags::Zero))
			regs->pc += (s8)mem->Read8(regs->pc++);
		break;
	case 2:
		if(!TESTBIT(regs->a, Flags::Carry))
			regs->pc += (s8)mem->Read8(regs->pc++);
		break;
	case 3:
		if( TESTBIT(regs->a, Flags::Carry))
			regs->pc += (s8)mem->Read8(regs->pc++);
		break;
	default:
		throw std::runtime_error("impossible condition");
	}

	*c += 4;
}

DEFINEOPCODE(LDINC) {
	if(TESTBIT(opcode, 3)) {
		regs->a = mem->Read8(regs->hl++);
	} else {
		mem->Write8(regs->hl++, regs->a);
	}

	*c += 4;
}

DEFINEOPCODE(LDDEC) {
	if(TESTBIT(opcode, 3)) {
		regs->a = mem->Read8(regs->hl--);
	} else {
		mem->Write8(regs->hl--, regs->a);
	}

	*c += 4;
}

DEFINEOPCODE(DAA) {
	RESETBIT(regs->f, Flags::Subtract);

	u16 tmp = regs->a;

	if((regs->a & 0xF > 0x9) || TESTBIT(regs->f, Flags::HalfCarry))
		tmp += 0x6;

	if((regs->a & 0xF0 > 0x90) || TESTBIT(regs->f, Flags::Carry))
		tmp += 0x60;

	if(tmp > 0xFF)
		SETBIT(regs->f, Flags::Carry);
	else
		RESETBIT(regs->f, Flags::Carry);

	regs->a = tmp & 0xFF;

	if(tmp > 0xFF)
		SETBIT(regs->f, Flags::Zero);
	else
		RESETBIT(regs->f, Flags::Zero);
}

DEFINEOPCODE(CPL) {
	SETBIT(regs->f, Flags::HalfCarry);
	SETBIT(regs->f, Flags::Subtract);

	regs->a = ~regs->a;
}

DEFINEOPCODE(SCF) {
	regs->f = (regs->f & 0b10000000) | 0b10000;
}

DEFINEOPCODE(CCF) {
	regs->f &= 0b10010000;
	regs->f ^= 0b00010000;
}

DEFINEOPCODE(LDR1R2) {
	u8 *r[] = { 	&regs->b, &regs->c, &regs->d, &regs->e, &regs->h, &regs->l,
					nullptr, &regs->a };
	
	u8 *s = r[(opcode >> 3) & 0b111];
	u8 *d = r[opcode & 0b111];

	if(d == nullptr) {
		if(s == nullptr) {
			z80->Halt();
			return;
		}
		d = mem->Read8Address(regs->hl), *c += 4;
	} else if(s == nullptr) {
		s = mem->Read8Address(regs->hl), *c += 4;
	}

	*d = *s;
}

typedef void(*ALUCallbacks)(Registers*, u8);

DEFINEOPCODE(ALU) {
	ALUCallbacks callbacks[] = { ADD, ADC, SUB, SBC, AND, XOR, OR, CP };
	u8 *r[] = { 	&regs->b, &regs->c, &regs->d, &regs->e, &regs->h, &regs->l,
					nullptr, &regs->a };

	u8 *s = r[opcode & 0b111];
	if(s == nullptr) s = mem->Read8Address(regs->hl), *c += 4;

	callbacks[(opcode >> 3) & 0b111](regs, *s);
}

DEFINEOPCODE(ALUIMM) {
	ALUCallbacks callbacks[] = { ADD, ADC, SUB, SBC, AND, XOR, OR, CP };
	callbacks[(opcode >> 3) & 0b111](regs, mem->Read8(regs->pc++));
	*c += 4;
}

DEFINEOPCODE(POPR1) {
	u16 *dest[] = { &regs->bc, &regs->de, &regs->hl, &regs->af };
	*dest[(opcode >> 4) & 0b11] = mem->Pop16(regs);
	*c += 8;
}

DEFINEOPCODE(PUSHR1) {
	u16 *dest[] = { &regs->bc, &regs->de, &regs->hl, &regs->af };
	mem->Push16(regs, *dest[(opcode >> 4) & 0b11]);
	*c += 12;
}

DEFINEOPCODE(RST) {
	mem->Push16(regs, regs->pc);
	regs->pc = opcode & 0b111000;
	*c += 28;
}

DEFINEOPCODE(RETWITHCONDITION) {
	switch((opcode >> 3) & 0b11) {
	case 0:
		if(!TESTBIT(regs->a, Flags::Zero))
			regs->pc = mem->Pop16(regs);
		break;
	case 1:
		if( TESTBIT(regs->a, Flags::Zero))
			regs->pc = mem->Pop16(regs);
		break;
	case 2:
		if(!TESTBIT(regs->a, Flags::Carry))
			regs->pc = mem->Pop16(regs);
		break;
	case 3:
		if( TESTBIT(regs->a, Flags::Carry))
			regs->pc = mem->Pop16(regs);
		break;
	default:
		throw std::runtime_error("impossible condition");
	}

	*c += 4;
}

DEFINEOPCODE(RET) {
	regs->pc = mem->Pop16(regs);
	*c += 4;
}

DEFINEOPCODE(RETI) {
	regs->pc = mem->Pop16(regs);
	z80->EnableInterrupt();
	*c += 4;
}

DEFINEOPCODE(JMPWITHCONDITION) {
	switch((opcode >> 3) & 0b11) {
	case 0:
		if(!TESTBIT(regs->a, Flags::Zero))
			regs->pc = mem->Read16(regs->pc);
		break;
	case 1:
		if( TESTBIT(regs->a, Flags::Zero))
			regs->pc = mem->Read16(regs->pc);
		break;
	case 2:
		if(!TESTBIT(regs->a, Flags::Carry))
			regs->pc = mem->Read16(regs->pc);
		break;
	case 3:
		if( TESTBIT(regs->a, Flags::Carry))
			regs->pc = mem->Read16(regs->pc);
		break;
	default:
		throw std::runtime_error("impossible condition");
	}
	*c += 8;
}

DEFINEOPCODE(JMP) {
	regs->pc = mem->Read16(regs->pc);
	*c += 8;
}

DEFINEOPCODE(CALLWITHCONDITION) {
	mem->Push16(regs, regs->pc + 2);
	switch((opcode >> 3) & 0b11) {
	case 0:
		if(!TESTBIT(regs->a, Flags::Zero))
			regs->pc = mem->Read16(regs->pc);
		break;
	case 1:
		if( TESTBIT(regs->a, Flags::Zero))
			regs->pc = mem->Read16(regs->pc);
		break;
	case 2:
		if(!TESTBIT(regs->a, Flags::Carry))
			regs->pc = mem->Read16(regs->pc);
		break;
	case 3:
		if( TESTBIT(regs->a, Flags::Carry))
			regs->pc = mem->Read16(regs->pc);
		break;
	default:
		throw std::runtime_error("impossible condition");
	}
	*c += 8;
}

DEFINEOPCODE(CALL) {
	mem->Push16(regs, regs->pc + 2);
	regs->pc = mem->Read16(regs->pc);
	*c += 8;
}

DEFINEOPCODE(ADDSPIMM) {
	regs->f ^= regs->f;
	regs->sp += mem->Read8(regs->pc++);
	*c += 12;
}

DEFINEOPCODE(LDHLSPIMM) {
	regs->hl = regs->sp + mem->Read8(regs->pc++);
	*c += 8;
}

DEFINEOPCODE(LDAADDRIMM8) {
	u8 imm = mem->Read8(regs->pc++);

	if(TESTBIT(opcode, 3)) {
		regs->a = mem->Read8(0xFF00 + imm);
	} else {
		mem->Write8(0xFF00 + imm, regs->a);
	}

	*c += 8;
}

DEFINEOPCODE(LDAADDRC) {
	if(TESTBIT(opcode, 3)) {
		regs->a = mem->Read8(0xFF00 + regs->c);
	} else {
		mem->Write8(0xFF00 + regs->c, regs->a);
	}

	*c += 4;
}

DEFINEOPCODE(LDAADDRIMM16) {
	u16 addr = mem->Read16(regs->pc);
	regs->pc += 2;
	if(TESTBIT(opcode, 3)) {
		regs->a = mem->Read8(addr);
	} else {
		mem->Write8(addr, regs->a);
	}

	*c += 12;
}

DEFINEOPCODE(JMPHL) {
	regs->pc = regs->hl;
	*c += 12;
}

DEFINEOPCODE(LDSPHL) {
	regs->sp = regs->hl;
	*c += 4;
}

DEFINEOPCODE(DI) {
	z80->DisableInterrupt();
}

DEFINEOPCODE(EI) {
	z80->EnableInterrupt();
}

typedef void(*CBCallbacks)(Registers*, Cycles*, Memory*, u8);

DEFINEOPCODE(PrefixCB) {
	CBCallbacks callbacks[] = {
		RxC,		// R(R/L)C R
		Rx,			// R(R/L) R
		SxA,		// S(R/L)A
		SWAP,		// SWAP R
		SRL,		// SRL R
		BIT,		// BIT bit, R
		RES,		// RES bit, R
		SET			// SET bit, R
	};

	u8 op = mem->Read8(regs->pc++);
	u8 lop = (op >> 4) & 0b11, hop = (op >> 6) & 0b11;
	if(lop == 0b11) {
		callbacks[3 + GETBIT(op, 3)](regs, c, mem, op);
	} else if(hop != 0b00) {
		callbacks[4 + hop](regs, c, mem, op);
	} else {
		callbacks[lop](regs, c, mem, op);
	}

	*c += c->cycles;
}

#undef DEFINEOPCODE
