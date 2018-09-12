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

#define INRANGE(x, min, max) ((min) <= (x) && (x) < (max))

Memory::Memory(SleepyGB *gb, Cartridge cart) 
: gb(gb)
{
	if(!cart.IsLoaded()) {
		throw std::runtime_error("Cartridge is not loaded");
	}
	
	this->cart = cart;
	
	memory = new u8[0x10000];
	
	RAMBanks = new u8*[4];
	for(u16 i = 0; i < 4; RAMBanks[i++] = new u8[0x2000]) {}
	
	currentRAMBank = (currentROMBank = 1) ^ 1;
	
	mbcType = cart.GetMBCType();
	
	ROMBanking = false;

	std::cout << "[Memory] Initialised" << std::endl;
}

Memory::~Memory() {
}

// NOTE: Use Write8 unless necessary
void Memory::RawWrite8(u16 address, u8 data) {
	memory[address] = data;
}

void Memory::Write8(u16 address, u8 data) {
	if(address < 0x8000) {
		HandleROMWrite(address, data);
		return;
	} else if(INRANGE(address, 0xFEA0, 0xFEFF)) {
		// not usable
		return;
	} else if(INRANGE(address, 0xA000, 0xC000)) {
		if(enableRAM) {
			u16 a = address - 0xA000;
			RAMBanks[currentRAMBank][a] = data;
			return;
		}
	}
	
	if(INRANGE(address, 0xE000, 0xFE00)) {
		address -= 0x2000;
	}
	
	if(address == 0xFF04 || address == 0xFF44) {
		memory[address] = 0;
		return;
	}
	
	if(address == 0xFF46) {
		u16 a = data;
		a <<= 8;
		for(u32 i = 0; i < 0xA0; ++i) {
			Write8(0xFE00 + i, Read8(a + i));
		}
	}
	
	if(address == 0xFF07) {
		u8 cf = memory[0xFF07];
		if(cf != data) {
			memory[0xFF07] = data;
			gb->GetZ80()->UpdateClockFreq();
		}
	}
	
	memory[address] = data;
}

void Memory::Write16(u16 address, u16 data) {
	Write8(address + 1, data >> 8);
	Write8(address + 0, data & 0xFF);
}

u8 *Memory::Read8Address(u16 address) {
	if(address < 0x4000) {
		return &cart.GetROMBank(0)[address];
	}

	if(INRANGE(address, 0x4000, 0x8000)) {
		u16 a = address - 0x4000;
		return &cart.GetROMBank(currentROMBank)[a];
	}
	
	if(INRANGE(address, 0xA000, 0xC000)) {
		u16 a = address - 0xA000;
		return &RAMBanks[currentRAMBank][a];
	}
	
	if(address == 0xFF00) {
		return gb->GetJoypad()->GetStatePtr();
	}
	
	return &memory[address];
}

u8 Memory::Read8(u16 address) {
	auto *ptr = Read8Address(address);

	return *ptr;
}

u16 Memory::Read16(u16 address) {
	u16 r = Read8(address + 1);
	r = (r << 8) | Read8(address);
	return r;
}

void Memory::HandleROMWrite(u16 address, u8 data) {
	if(address < 0x2000 && mbcType >= MBC1) {
		if(mbcType == MBC2 && TESTBIT(address, 4)) {
			u8 t = data & 0xF;
			if(t == 0xA) {
				enableRAM = true;
			} else if(t == 0x0) {
				enableRAM = false;
			}
		}
	} else if(INRANGE(address, 0x2000, 0x4000) && mbcType >= MBC1) {
		if(mbcType == MBC2) {
			currentROMBank = (data & 0xF) || 1;
		} else {
			currentROMBank = ((currentROMBank & 224) | (data & 31)) || 1;
		}
	} else if(INRANGE(address, 0x4000, 0x6000) && mbcType == MBC1) {
		if(ROMBanking) {
			currentROMBank = ((currentROMBank & 224) | (data & 31)) || 1;
		} else {
			currentRAMBank = data & 3;
		}
	} else if(INRANGE(address, 0x6000, 0x8000) && mbcType == MBC1) {
		currentRAMBank = (ROMBanking = !(data & 1)) ? 0 : currentRAMBank;
	}
}

u8 Memory::RawRead8(u16 address) {
	return memory[address];
}

void Memory::Push8(Registers *regs, u8 data) {
	Write8(regs->sp--, data);
}

void Memory::Push16(Registers *regs, u8 data) {
	Write16(regs->sp, data);
	regs->sp -= 2;
}

u8 Memory::Pop8(Registers *regs) {
	return Read8(++regs->sp);
}

u16 Memory::Pop16(Registers *regs) {
	return Read16(regs->sp += 2);
}

