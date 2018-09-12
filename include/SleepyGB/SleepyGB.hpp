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

#ifndef __SLEEPYGB_SLEEPYGB_HPP
#define __SLEEPYGB_SLEEPYGB_HPP

class SleepyGB {
	Memory *mem;
	PPU *ppu;
	SleepyZ80 *z80;
	Joypad *pad;
	
public:
	SleepyGB(Cartridge cart);
	
	void Run();
	
	inline Memory *GetMemory() { return mem; }
	inline PPU		 *GetPPU() { return ppu; }
	inline SleepyZ80 *GetZ80() { return z80; }
	inline Joypad *GetJoypad() { return pad; }
};

#endif//__SLEEPYGB_SLEEPYGB_HPP
