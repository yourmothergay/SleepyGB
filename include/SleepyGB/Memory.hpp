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

#ifndef __SLEEPYGB_MEMORY_HPP
#define __SLEEPYGB_MEMORY_HPP

class Memory {
	Cartridge cart;
	SleepyGB *gb;
	u8 *memory;
	u8 **RAMBanks;
	u16 currentROMBank, currentRAMBank;
	bool enableRAM, ROMBanking;
	MBCType mbcType;
	
	void HandleROMWrite(u16 address, u8 data);
	
public:
	Memory(SleepyGB *gb, Cartridge cart);
	~Memory();
	
	void RawWrite8(u16 address, u8 data);
	u8 RawRead8(u16 address);
	
	void Write8(u16 address, u8 data);
	void Write16(u16 address, u16 data);
	
	u8 Read8(u16 address);
	u16 Read16(u16 address);
	
	void Push8(Registers *regs, u8 data);
	void Push16(Registers *regs, u8 data);
	
	u8 Pop8(Registers *regs);
	u16 Pop16(Registers *regs);
	
	u8 *Read8Address(u16 address);
};

#endif//__SLEEPYGB_MEMORY_HPP
