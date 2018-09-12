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

#ifndef __SLEEPYGB_SLEEPYZ80_HPP
#define __SLEEPYGB_SLEEPYZ80_HPP

#include <vector>
#include <string>

struct Registers {
	union {
		struct {
			u16 af, bc, de, hl;
		};
		struct {
			u8 l, h, e, d, c, b, f, a;
		};
	};
	u16 pc, divCounter, sp;
};

struct Cycles {
	u32 cycles, opcycle;
	
	u32 operator=(u32 v) {
		return opcycle = cycles = v;
	}
	
	u32 operator+=(u32 v) {
		opcycle += v;
		cycles += v;
		return cycles;
	}
};

enum Flags {
	Zero = 7,
	Subtract = 6,
	HalfCarry = 5,
	Carry = 4
};

struct Opcode {
	u8 bitmask, value;
	std::string name;
	void(*callback)(SleepyZ80*, Registers*, Cycles*, Memory*, u8);
};

class SleepyZ80 {
	SleepyGB *gb;
	Memory *mem;
	Registers regs;
	Cycles c;
	s32 timerCounter;
	bool isIntEnabled, stopped, isCurrentOpcodeStop, prevOpcodeStop;
	bool isCurrentOpcodeEI, prevOpcodeEI, isCurrentOpcodeDI, prevOpcodeDI;
	bool isCurrentOpcodeHalt, prevOpcodeHalt, halted;
	std::vector<Opcode> opcodeTable;
	
	void UpdateTimers();
	void DoInterrupts();
	void UpdateDivReg();
	
	inline Opcode GetOpcode(u8 o) {
		for(Opcode op : opcodeTable) {
			if((o & op.bitmask) == op.value) {
				return op;
			}
		}
		
		Opcode op;
		throw std::runtime_error(std::string("not a valid opcode ") + std::to_string((u32)o));
		return op;
	}
	
	void _AddOpcode(const char *s, const std::string &name, void(*callback)(SleepyZ80*, Registers*, Cycles*, Memory*, u8));
	
public:
	SleepyZ80(SleepyGB *gb);
	
	void DoIRQ(u16 irq);
	void EmulateCycle();
	void UpdateClockFreq();
	void Stop();
	void Unstop();
	void Halt();
	void Unhalt();
	void EnableInterrupt();
	void DisableInterrupt();
};

#endif//__SLEEPYGB_SLEEPYZ80_HPP
