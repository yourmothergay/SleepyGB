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
#include <cstring>

void PPU::WritePixel(u32 x, u32 y, u32 rgba) {
	u32 position = (x + y * 160) * 4;
	pixels[position + 0] = x >> 24;
	pixels[position + 1] = (x >> 16) & 0xFF; 
	pixels[position + 2] = (x >> 8) & 0xFF; 
	pixels[position + 3] = x & 0xFF;

	std::cout << "[PPU] WritePixel called, x: " << x << " y: " << y;
	std::cout << "colour: 0x" << std::hex << rgba << std::dec << std::endl;
}

PPU::PPU(SleepyGB *gb) : gb(gb) {
	pixels = new u8[/*160 * 144 * 4*/ 92160];
	memset(pixels, '\xFF', 92160);
	scanlineCounter = 456;
	mem = gb->GetMemory();
	std::cout << "[PPU] Initialised" << std::endl;
}

void PPU::Update(u32 cycles) {
	//std::cout << "[PPU] Update called, cycles: " << cycles << std::endl;
	u32 scanline = mem->Read8(0xFF44);
	UpdateStatus(scanline);
	if(!isEnabled) return;
	
	scanlineCounter -= cycles;
	if(scanlineCounter <= 0) {
		scanline++;
		mem->RawWrite8(0xFF44, scanline);
		
		//std::cout << "[PPU] scanlineCounter is 0" << std::endl;

		scanlineCounter = 456;
		
		if(scanline == 144)
			gb->GetZ80()->DoIRQ(0);
		else if(scanline > 153)
			mem->RawWrite8(0xFF44, 0);
		else
			DrawScanline(scanline);
	}
}

void PPU::DrawScanline(u32 scanline) {
	//std::cout << "[PPU] Drawing scanline " << scanline << std::endl;

	u8 ctl = mem->Read8(0xFF40);
	if(TESTBIT(ctl, 0))
		RenderTiles(scanline);
	
	if(TESTBIT(ctl, 1))
		RenderSprites(scanline);
}

void PPU::UpdateStatus(u32 scanline) {
	isEnabled = TESTBIT(mem->Read8(0xFF40), 7);

	u8 status = mem->Read8(0xFF41);
	if(!isEnabled) {
		scanlineCounter = 456;
		mem->RawWrite8(0xFF44, 0);
		status = (status & 252) | 1;
		mem->Write8(0xFF41, status);
		return;
	}
	
	u8 currentMode = status & 3;
	u8 mode = 0;
	bool irq = false;
	
	if(scanline >= 144) {
		mode = 1;
		status = BITRESET(BITSET(status, 0), 1);
		irq = TESTBIT(status, 4);
	} else {
		if(scanlineCounter >= 396) {
			mode = 2;
			status = BITRESET(BITSET(status, 1), 0);
			irq = TESTBIT(status, 5);
		} else if(scanlineCounter >= 224) {
			mode = 3;
			status = BITSET(BITSET(status, 1), 0);
			irq = TESTBIT(status, 5);
		} else {
			mode = 0;
			status = BITRESET(BITRESET(status, 1), 0);
			irq = TESTBIT(status, 3);
		}
	}
	
	if(irq && mode != currentMode) gb->GetZ80()->DoIRQ(1);
	
	if(scanline == mem->Read8(0xFF45)) {
		SETBIT(status, 2);
		if(TESTBIT(status, 6))
			gb->GetZ80()->DoIRQ(1);
	} else {
		RESETBIT(status, 2);
	}
	
	mem->Write8(0xFF41, status);
}

void PPU::RenderTiles(u32 scanline) {
	u16 tileData = 0x8000;
	u16 backgroundMem = 0x9800, tileRow;
	bool unsig = true;
	u8 ctl = mem->Read8(0xFF40), y;
	
	u8 scrollY = mem->Read8(0xFF42);
	u8 scrollX = mem->Read8(0xFF43);
	u8 windowY = mem->Read8(0xFF4A);
	u8 windowX = mem->Read8(0xFF4B) - 7;
	
	bool usingWindow = false;
	
	if(TESTBIT(ctl, 5) && windowY <= scanline) usingWindow = true;
	
	if(!TESTBIT(ctl, 4)) {
		tileData |= 0x800;
		unsig = false;
	}
	
	if((!usingWindow && TESTBIT(ctl, 3)) || (usingWindow && TESTBIT(ctl, 6))) {
		backgroundMem |= 0x400;
	}
	
	y = scanline + (usingWindow ? -1 : 1) * windowY;
	tileRow = (y & 248) << 5;
	
	for(u32 p = 0; p < 160; ++p) {
		u8 x = (usingWindow && p >= windowX) ? p - windowX : p + scrollX;
		u16 tileColumn = x >> 3;
		u16 addr = backgroundMem + tileRow + tileColumn;
		s16 tileNo = unsig ? mem->Read8(addr) : (s8)mem->Read8(addr);
		u16 tileLoc = tileData + ((tileNo + (unsig ? 0 : 128)) << 4);
		u8 line = (y & 7) << 1;
		u8 d1 = mem->Read8(tileLoc + line);
		u8 d2 = mem->Read8(tileLoc + line + 1);
		u32 colourBit = 7 - (x & 7);
		u32 colour = (GETBIT(d2, colourBit) << 1) | GETBIT(d1, colourBit);
		
		u8 palette = mem->Read8(0xFF47);
		u32 z = 0, w = 0;
		u32 rgba = 0x00000000;
		switch(colour) {
			case 0: z = 1; break;
			case 1: z = 3; w = 2; break;
			case 2: z = 5; w = 4; break;
			case 3: z = 7; w = 6; break;
			default: break;
		}
		
		switch((GETBIT(palette, z) << 1) | GETBIT(palette, w)) {
			case 0: rgba = 0xFFFFFFFF; break;
			case 1: rgba = 0xAAAAAAFF; break;
			case 2: rgba = 0x555555FF; break;
			default: break;
		}
		
		if(scanline < 0 || scanline > 143 || p < 0 || p > 159) {
			continue;
		}
		
		WritePixel(p, scanline, rgba);
	}
}

#define INRANGE(x, min, max) ((min) <= (x) && (x) < (max))

void PPU::RenderSprites(u32 scanline) {
	u8 ctl = mem->Read8(0xFF40);
	bool use8x16 = TESTBIT(ctl, 2);
	
	for(u32 s = 0; s < 40; ++s) {
		u8 i = s << 2;
		u8 y = mem->Read8(0xFE00 + i) - 16;
		u8 x = mem->Read8(0xFE01 + i) - 8;
		u8 tileLoc = mem->Read8(0xFE02 + i);
		u8 attr = mem->Read8(0xFE03 + i);
		bool flipX = TESTBIT(attr, 6);
		bool flipY = TESTBIT(attr, 5);
		
		u32 sizeY = 8 << (u32)use8x16; // genius
		
		if(INRANGE(scanline, y, y + sizeY)) {
			u32 line = scanline - y;
			if(flipY) line = sizeY - line;
			line <<= 1;
			
			u16 dataAddr = 0x8000 + line + tileLoc << 4;
			u8 d1 = mem->Read8(dataAddr);
			u8 d2 = mem->Read8(dataAddr + 1);
			
			for(u32 pixel = 7; pixel >= 0; --pixel) {
				u32 colourBit = pixel;
				if(flipX) colourBit = 7 - colourBit;
				
				u8 colourNo = (GETBIT(d2, colourBit) << 1) | GETBIT(d1, colourBit);
				u16 colourAddr = 0xFF48 + GETBIT(attr, 4);
				
				u32 z = 0, w = 0;
				u8 palette = mem->Read8(0xFF47);
				bool transparent = false;
				u32 rgba = 0x00000000;
				switch(colourNo) {
					case 0: z = 1; break;
					case 1: z = 3; w = 2; break;
					case 2: z = 5; w = 4; break;
					case 3: z = 7; w = 6; break;
					default: break;
				}
				
				switch((GETBIT(palette, z) << 1) | GETBIT(palette, w)) {
					case 0: transparent = true; break;
					case 1: rgba = 0xAAAAAAFF;  break;
					case 2: rgba = 0x555555FF;  break;
					default: break;
				}
				
				if(transparent) continue;
				
				u32 p = x + (7 - p);
				
				if(scanline < 0 || scanline > 143 || p < 0 || p > 159) {
					continue;
				}
				
				WritePixel(p, scanline, rgba);
			}
		}
	}
}

