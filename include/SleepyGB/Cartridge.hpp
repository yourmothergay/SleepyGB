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

#ifndef __SLEEPYGB_CARTRIDGE_HPP
#define __SLEEPYGB_CARTRIDGE_HPP

#define MAX_CART_SIZE 0x200000

enum MBCType {
	NONE,
	MBC1,
	MBC2
};

class Cartridge {
	u8 *cartMemory;
	bool isLoaded;
	
public:
	Cartridge();
	~Cartridge();
	
	void LoadFile(const char *filename);

	MBCType GetMBCType();

	inline u8 *GetROMBank(u16 number) { return &cartMemory[number * 0x4000]; }
	inline bool IsLoaded() { return isLoaded; }
};

#endif//__SLEEPYGB_CARTRIDGE_HPP
