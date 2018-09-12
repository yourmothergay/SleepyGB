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

#define DEFINEOPCODE(name) \
inline void name(Registers *regs, Cycles *c, Memory *mem, u8 opcode)

#define SETVARS u8 *dest[] = { 	&regs->b, &regs->c, &regs->d, &regs->e, &regs->h, &regs->l, nullptr, &regs->a }; \
				u8 *d = dest[opcode & 0b111]; if(d == nullptr) d = mem->Read8Address(regs->hl), *c += 4

DEFINEOPCODE(RxC) {
	SETVARS;

	regs->f = 0;
	
	switch(GETBIT(opcode, 3)) {
	case 0:
		if(TESTBIT(*d, 7)) SETBIT(regs->f, Flags::Carry);
		*d = (*d << 1);
		break;
	case 1:
		if(TESTBIT(*d, 0)) SETBIT(regs->f, Flags::Carry);
		*d = (*d >> 1);
		break;
	}
	
	if(!*d) SETBIT(regs->f, Flags::Zero);
}

DEFINEOPCODE(Rx) {
	SETVARS;

	regs->f = 0;
	
	u8 oldcarry = GETBIT(regs->f, Flags::Carry);
	
	switch(GETBIT(opcode, 3)) {
	case 0:
		if(TESTBIT(*d, 7)) SETBIT(regs->f, Flags::Carry);
		*d = (*d << 1) | oldcarry;
		break;
	case 1:
		if(TESTBIT(*d, 0)) SETBIT(regs->f, Flags::Carry);
		*d = (*d >> 1) | (oldcarry << 7);
		break;
	}
	
	if(!*d) SETBIT(regs->f, Flags::Zero);
}

DEFINEOPCODE(SxA) {
	SETVARS;

	regs->f = 0;
	
	switch(GETBIT(opcode, 3)) {
	case 0:
		if(TESTBIT(*d, 7)) SETBIT(regs->f, Flags::Carry);
		*d <<= 1;
		break;
	case 1:
		if(TESTBIT(*d, 0)) SETBIT(regs->f, Flags::Carry);
		u8 msb = *d & 0b10000000;
		*d >>= 1;
		*d |= msb;
		break;
	}
	
	if(!*d) SETBIT(regs->f, Flags::Zero);
}

DEFINEOPCODE(SWAP) {
	SETVARS;

	*d = (*d >> 4) | ((*d & 0xF) << 4); 
}

DEFINEOPCODE(SRL) {
	SETVARS;
	regs->f = 0;
	if(TESTBIT(*d, 0)) SETBIT(regs->f, Flags::Carry);

	*d >>= 1;

	if(!*d) SETBIT(regs->f, Flags::Zero);
}

DEFINEOPCODE(BIT) {
	SETVARS;

	regs->f = (regs->f & 0b00010000) | 0b00100000;
	if(TESTBIT(*d, (opcode >> 3) & 0b111)) 
		SETBIT(regs->f, Flags::Zero);
}

DEFINEOPCODE(RES) {
	SETVARS;
	SETBIT(*d, (opcode >> 3) & 0b111);
}

DEFINEOPCODE(SET) {
	SETVARS;
	RESETBIT(*d, (opcode >> 3) & 0b111);
}

#undef DEFINEOPCODE
