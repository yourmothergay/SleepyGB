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

#define DEFINEALUOP(name) inline void name(Registers *regs, u8 data)

DEFINEALUOP(ADD) {
	regs->f ^= regs->f;

	u16 tmp = regs->a & 0xF;
	tmp += data & 0xF;

	if(tmp > 0xF) SETBIT(regs->f, Flags::HalfCarry);

	tmp += regs->a & 0xF0;
	tmp += data & 0xF0;

	if(tmp > 0xFF) SETBIT(regs->f, Flags::Carry);

	regs->a = tmp & 0xFF;

	if(!regs->a) SETBIT(regs->f, Flags::Zero);
}

DEFINEALUOP(ADC) {
	u16 tmp = (regs->a & 0xF) + GETBIT(regs->f, Flags::Carry);
	tmp += data & 0xF;

	regs->f ^= regs->f;

	if(tmp > 0xF) SETBIT(regs->f, Flags::HalfCarry);

	tmp += regs->a & 0xF0;
	tmp += data & 0xF0;

	if(tmp > 0xFF) SETBIT(regs->f, Flags::Carry);

	regs->a = tmp & 0xFF;

	if(!regs->a) SETBIT(regs->f, Flags::Zero);
}

DEFINEALUOP(SUB) {
	regs->f = 0b01000000;
	if((regs->a & 0xF) < (data & 0xF)) SETBIT(regs->f, Flags::HalfCarry);
	if(regs->a < data) SETBIT(regs->f, Flags::Carry);

	regs->a -= data;

	if(!regs->a) SETBIT(regs->f, Flags::Zero);
}

DEFINEALUOP(SBC) {
	u8 carry = GETBIT(regs->f, Flags::Carry);
	u8 s = data + carry;
	if((regs->a & 0xF) < (s & 0xF)) SETBIT(regs->f, Flags::HalfCarry);
	if(regs->a < s) SETBIT(regs->f, Flags::Carry);

	regs->a -= s;

	if(!regs->a) SETBIT(regs->f, Flags::Zero);
}

DEFINEALUOP(AND) {
	regs->f = 0b00100000;
	regs->a &= data;
	if(!regs->a) SETBIT(regs->f, Flags::Zero);
}

DEFINEALUOP(XOR) {
	regs->a ^= data;
	if(!regs->a) SETBIT(regs->f, Flags::Zero);
}

DEFINEALUOP(OR) {
	regs->a |= data;
	if(!regs->a) SETBIT(regs->f, Flags::Zero);
}

DEFINEALUOP(CP) {
	regs->f = 0b01000000;
	if((regs->a & 0xF) < (data & 0xF)) SETBIT(regs->f, Flags::HalfCarry);
	if(regs->a < data) SETBIT(regs->f, Flags::Carry);
	if(regs->a == data) SETBIT(regs->f, Flags::Zero);
}

#undef DEFINEALUOP
