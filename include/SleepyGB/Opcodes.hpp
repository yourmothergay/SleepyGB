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

#ifndef __SLEEPYGB_OPCODES_HPP
#define __SLEEPYGB_OPCODES_HPP

// deleted original Opcodes.hpp

#define DEFINEOPCODE(name) \
void name(SleepyZ80 *z80, Registers *regs, Cycles *c, Memory *mem, u8 opcode)

DEFINEOPCODE(NOP);							// NOP
DEFINEOPCODE(LDADDRSP);						// LD (imm16), SP
DEFINEOPCODE(LDR1IMM16);					// LD R, imm16
DEFINEOPCODE(ADDHLR1);						// ADD HL, R
DEFINEOPCODE(LDBCDEA);						// LD (BC/DE), A or LD A, (BC/DE)
DEFINEOPCODE(INCR1);						// INC R
DEFINEOPCODE(DECR1);						// DEC R
DEFINEOPCODE(LDR1IMM);						// LD R, imm8
DEFINEOPCODE(RxCA);							// R(R/L)CA
DEFINEOPCODE(RxA);							// R(R/L)A
DEFINEOPCODE(STOP);							// STOP
DEFINEOPCODE(JUMPRELATIVE);					// JR imm8
DEFINEOPCODE(JUMPRELATIVEWITHCONDITION);	// JR condition, imm8
DEFINEOPCODE(LDINC);						// LDI A, (HL) or LDI (HL), A
DEFINEOPCODE(LDDEC);						// LDD A, (HL) or LDD (HL), A
DEFINEOPCODE(DAA);							// DAA
DEFINEOPCODE(CPL);							// CPL
DEFINEOPCODE(SCF);							// SCF
DEFINEOPCODE(CCF);							// CCF
DEFINEOPCODE(LDR1R2);						// LD R1, R2
DEFINEOPCODE(ALU);							// OP A, R1
DEFINEOPCODE(ALUIMM);						// OP A, imm8
DEFINEOPCODE(PUSHR1);						// PUSH R
DEFINEOPCODE(POPR1);						// POP R
DEFINEOPCODE(RST);							// RST addr
DEFINEOPCODE(RETWITHCONDITION);				// RET condition
DEFINEOPCODE(RET);							// RET
DEFINEOPCODE(RETI);							// RET, EI
DEFINEOPCODE(JMPWITHCONDITION);				// JMP condition, imm16
DEFINEOPCODE(JMP);							// JMP imm16
DEFINEOPCODE(CALLWITHCONDITION);			// CALL condition, imm16
DEFINEOPCODE(CALL);							// CALL imm16
DEFINEOPCODE(ADDSPIMM);						// ADD SP, imm8
DEFINEOPCODE(LDHLSPIMM);					// LD HL, SP+imm8
DEFINEOPCODE(LDAADDRIMM8);					// LD (0xFF00 + imm8), A or LD A, (0xFF00 + imm8)
DEFINEOPCODE(LDAADDRC);						// LD A, (C) or LD (C), A 
DEFINEOPCODE(LDAADDRIMM16);					// LD A, (imm16) or LD (imm16), A
DEFINEOPCODE(JMPHL);						// JMP HL
DEFINEOPCODE(LDSPHL);						// LD SP, HL
DEFINEOPCODE(DI);							// DI
DEFINEOPCODE(EI);							// EI
DEFINEOPCODE(PrefixCB);						// see implementation

#endif//__SLEEPYGB_OPCODES_HPP