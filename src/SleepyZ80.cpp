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
#include <iomanip>
#include <SleepyGB/Opcodes.hpp>

#define AddOpcode(e, c) _AddOpcode(e, #c, c)

SleepyZ80::SleepyZ80(SleepyGB *gb) : gb(gb) {
	mem = gb->GetMemory();
	
	regs.pc = 0x0100;
	regs.af = 0x01B0;
	regs.bc = 0x0013;
	regs.de = 0x00D8;
	regs.hl = 0x014D;
	regs.sp = 0xFFFE;
	regs.divCounter = 0x0000;
	
	isIntEnabled = true;

	isIntEnabled = true;

	stopped = isCurrentOpcodeStop = prevOpcodeStop = \
	isCurrentOpcodeEI = prevOpcodeEI = isCurrentOpcodeDI = prevOpcodeDI = \
	isCurrentOpcodeHalt = prevOpcodeHalt = halted = false;
	
	mem->Write8(0xFF05, 0x00);
	mem->Write8(0xFF06, 0x00);
	mem->Write8(0xFF07, 0x00);
	mem->Write8(0xFF10, 0x80);
	mem->Write8(0xFF11, 0xBF);
	mem->Write8(0xFF12, 0xF3);
	mem->Write8(0xFF14, 0xBF);
	mem->Write8(0xFF16, 0x3F);
	mem->Write8(0xFF17, 0x00);
	mem->Write8(0xFF19, 0xBF);
	mem->Write8(0xFF1A, 0x7F);
	mem->Write8(0xFF1B, 0xFF);
	mem->Write8(0xFF1C, 0x9F);
	mem->Write8(0xFF1E, 0xBF);
	mem->Write8(0xFF20, 0xFF);
	mem->Write8(0xFF21, 0x00);
	mem->Write8(0xFF22, 0x00);
	mem->Write8(0xFF23, 0xBF);
	mem->Write8(0xFF24, 0x77);
	mem->Write8(0xFF25, 0xF3);
	mem->Write8(0xFF26, 0xF1);
	mem->Write8(0xFF40, 0x91);
	mem->Write8(0xFF42, 0x00);
	mem->Write8(0xFF43, 0x00);
	mem->Write8(0xFF45, 0x00);
	mem->Write8(0xFF47, 0xFC);
	mem->Write8(0xFF48, 0xFF);
	mem->Write8(0xFF49, 0xFF);
	mem->Write8(0xFF4A, 0x00);
	mem->Write8(0xFF4B, 0x00);
	mem->Write8(0xFFFF, 0x00);
	
	AddOpcode("00000000", NOP);							// NOP
	AddOpcode("00001000", LDADDRSP);					// LD (imm16), SP
	AddOpcode("00xx0001", LDR1IMM16);					// LD R, imm16
	AddOpcode("00xx1001", ADDHLR1);						// ADD HL, R
	AddOpcode("000xx010", LDBCDEA);						// LD (BC/DE), A or LD A, (BC/DE)
	AddOpcode("00xxx100", INCR1);						// INC R
	AddOpcode("00xxx101", DECR1);						// DEC R
	AddOpcode("00xxx110", LDR1IMM);						// LD R, imm8
	AddOpcode("0000x111", RxCA);						// R(R/L)CA
	AddOpcode("0001x111", RxA);							// R(R/L)A
	AddOpcode("00010000", STOP);						// STOP
	AddOpcode("00011000", JUMPRELATIVE);				// JR imm8
	AddOpcode("001xx000", JUMPRELATIVEWITHCONDITION);	// JR condition, imm8
	AddOpcode("0010x010", LDINC);						// LDI A, (HL) or LDI (HL), A
	AddOpcode("0011x010", LDDEC);						// LDD A, (HL) or LDD (HL), A
	AddOpcode("00100111", DAA);							// DAA
	AddOpcode("00101111", CPL);							// CPL
	AddOpcode("00110111", SCF);							// SCF
	AddOpcode("00111111", CCF);							// CCF
	AddOpcode("01xxxxxx", LDR1R2);						// LD R1, R2
	AddOpcode("10xxxxxx", ALU);							// OP A, R1
	AddOpcode("11xxx110", ALUIMM);						// OP A, imm8
	AddOpcode("11xx0101", PUSHR1);						// PUSH R
	AddOpcode("11xx0001", POPR1);						// POP R
	AddOpcode("11xx1111", RST);							// RST addr
	AddOpcode("110xx000", RETWITHCONDITION);			// RET condition
	AddOpcode("11001001", RET);							// RET
	AddOpcode("11011001", RETI);						// RET, EI
	AddOpcode("110xx010", JMPWITHCONDITION);			// JMP condition, imm16
	AddOpcode("11000011", JMP);							// JMP imm16
	AddOpcode("110xx100", CALLWITHCONDITION);			// CALL condition, imm16
	AddOpcode("11001101", CALL);						// CALL imm16
	AddOpcode("11101000", ADDSPIMM);					// ADD SP, imm8
	AddOpcode("11111000", LDHLSPIMM);					// LD HL, SP+imm8
	AddOpcode("111x0000", LDAADDRIMM8);					// LD (0xFF00 + imm8), A or LD A, (0xFF00 + imm8)
	AddOpcode("111x0010", LDAADDRC);					// LD A, (C) or LD (C), A 
	AddOpcode("111x1010", LDAADDRIMM16);				// LD A, (imm16) or LD (imm16), A
	AddOpcode("11101001", JMPHL);						// JMP HL
	AddOpcode("11111001", LDSPHL);						// LD SP, HL
	AddOpcode("11110011", DI);							// DI
	AddOpcode("11111011", EI);							// EI
	AddOpcode("11001011", PrefixCB);					// see implementation

	std::cout << "[CPU] Initialised" << std::endl;
}

void SleepyZ80::_AddOpcode(const char *s, const std::string &name, void(*callback)(SleepyZ80*, Registers*, Cycles*, Memory*, u8)) {
	u8 bitmask, value;
	char *o = new char[8];
	for(u32 i = 0; i < 8; ++i) {
		switch(s[i]) {
		case '0':
			bitmask |= 1;
		case 'x':
			o[i] = '0';
			break;
		case '1':
			o[i] = '1';
			bitmask |= 1;
			break;
		default:
			throw std::runtime_error("no.");
		}

		if(i < 7) {
			bitmask <<= 1;
		}
	}

	value = static_cast<char>(std::stoi(o, 0, 2));
	
	Opcode op;
	op.bitmask = bitmask;
	op.value = value;
	op.callback = callback;
	op.name = name;

	//std::cout << "opcode " << name << " bitmask " << (u32)bitmask << " value " << (u32)value << std::endl; 
	
	opcodeTable.push_back(op);
}

#define TIMA 0xFF05
#define TMA 0xFF06
#define TMC 0xFF07 

void SleepyZ80::EmulateCycle() {
	if(c.cycles) {
		c.cycles--;
		if(!c.cycles) {
			//std::cout << "[CPU] End of opcode" << std::endl;

			{
				UpdateDivReg();
	
				if(TESTBIT(mem->Read8(TMC), 2)) {
					timerCounter -= c.opcycle;
					
					if(timerCounter <= 0) {
						UpdateClockFreq();
						
						if(mem->Read8(TIMA) == 255) {
							mem->Write8(TIMA, mem->Read8(TMA));
							DoIRQ(2);
						} else {
							mem->Write8(TIMA, mem->Read8(TIMA) + 1);
						}
					}
				}
			}

			gb->GetPPU()->Update(c.opcycle);
			
			{
				if(!isIntEnabled) return;
				
				u8 req = mem->Read8(0xFF0F);
				u8 enabled = mem->Read8(0xFFFF);
				
				if(!req) return;
				
				for(u32 i = 0; i < 5; ++i) {
					if(TESTBIT(req, i) && TESTBIT(enabled, i)) {
						isIntEnabled = false;
						RESETBIT(req, i);
						mem->Write8(0xFF0F, req);
						
						mem->Push16(&regs, regs.pc);
						
						halted = false;

						regs.pc = 0x40 | (i << 3);
					}
				}
			}

			gb->GetJoypad()->Update();

			if(isCurrentOpcodeStop) {
				isCurrentOpcodeStop = false;
				stopped = true;
				prevOpcodeStop = true;
			}

			if(isCurrentOpcodeHalt) {
				isCurrentOpcodeHalt = true;
				prevOpcodeHalt = true;
				halted = true;
			}
		}
		return;
	}
	
	if(!prevOpcodeHalt && halted) {
		c = 3;
		return;
	}

	if(!prevOpcodeStop && stopped) {
		gb->GetJoypad()->Update();
		return;
	}

	u8 o = mem->Read8(regs.pc);

	if(!prevOpcodeHalt) regs.pc++;

	Opcode op = GetOpcode(o);
	c = 3;
	//std::cout << "[CPU] opcode " << op.name << std::endl;
	op.callback(this, &regs, &c, mem, o);
}

void SleepyZ80::UpdateTimers() {
	UpdateDivReg();
	
	if(TESTBIT(mem->Read8(TMC), 2)) {
		timerCounter -= c.opcycle;
		
		if(timerCounter <= 0) {
			UpdateClockFreq();
			
			if(mem->Read8(TIMA) == 255) {
				mem->Write8(TIMA, mem->Read8(TMA));
				DoIRQ(2);
			} else {
				mem->Write8(TIMA, mem->Read8(TIMA) + 1);
			}
		}
	}
}

void SleepyZ80::DoInterrupts() {
	if(!isIntEnabled) return;
	
	u8 req = mem->Read8(0xFF0F);
	u8 enabled = mem->Read8(0xFFFF);
	
	if(!req) return;
	
	for(u32 i = 0; i < 5; ++i) {
		if(TESTBIT(req, i) && TESTBIT(enabled, i)) {
			isIntEnabled = false;
			RESETBIT(req, i);
			mem->Write8(0xFF0F, req);
			
			mem->Push16(&regs, regs.pc);
			
			halted = false;

			regs.pc = 0x40 | (i << 3);
		}
	}
}

u16 TimerCounterTable[] = { 1024, 16, 64, 256 };

void SleepyZ80::UpdateClockFreq() {
	timerCounter = TimerCounterTable[mem->Read8(TMC) & 3];
}

void SleepyZ80::UpdateDivReg() {
	regs.divCounter += c.opcycle;
	if(regs.divCounter >= 255) {
		regs.divCounter = 0;
		mem->RawWrite8(0xFF04, mem->Read8(0xFF04) + 1);
	}
}

void SleepyZ80::DoIRQ(u16 irq) {
	//std::cout << "[CPU] Interrupt Request " << irq << std::endl;
	mem->Write8(0xFF0F, BITSET(mem->Read8(0xFF0F), irq));
	mem->Write8(0xFFFF, BITSET(mem->Read8(0xFFFF), irq));
}

void SleepyZ80::Stop() {
	isCurrentOpcodeStop = true;
}

void SleepyZ80::Unstop() {
	stopped = false;
}

void SleepyZ80::EnableInterrupt() {
	isCurrentOpcodeEI = true;
}

void SleepyZ80::DisableInterrupt() {
	isCurrentOpcodeDI = true;
}

void SleepyZ80::Halt() {
	isCurrentOpcodeHalt = true;
}

void SleepyZ80::Unhalt() {
	halted = false;
}