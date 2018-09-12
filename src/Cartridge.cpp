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
#include <stdio.h>

Cartridge::Cartridge() {
	cartMemory = new u8[MAX_CART_SIZE];
	isLoaded = false;
}

Cartridge::~Cartridge() {
}

void Cartridge::LoadFile(const char *filename) {
	FILE *fp = fopen(filename, "rb");
	if(!fp) {
		return;
	}
	
	fread(cartMemory, 1, MAX_CART_SIZE, fp);
	fclose(fp);
	
	isLoaded = true;
}

MBCType Cartridge::GetMBCType() {
	if(!isLoaded) return NONE;
	
	switch(cartMemory[0x147]) {
	case 0: case 1: case 2:
		return MBC1;
	case 4: case 5:
		return MBC2;
	default:
		return NONE;
	}
}

