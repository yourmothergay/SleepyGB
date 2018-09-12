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

u8 res;

sf::Keyboard::Key layout[] = {
	// right, left, up, down
	sf::Keyboard::Right, sf::Keyboard::Left, sf::Keyboard::Up, sf::Keyboard::Down,
	// a, b
	sf::Keyboard::A, sf::Keyboard::S,
	// select, start
	sf::Keyboard::RControl, sf::Keyboard::Return
};

Joypad::Joypad(SleepyGB *gb) : gb(gb) { 
	mem = gb->GetMemory();
	std::cout << "[Joypad] Initialised" << std::endl;
}

u8 Joypad::GetState() {
	return *GetStatePtr();
}

u8 *Joypad::GetStatePtr() {
	res = ~mem->RawRead8(0xFF00);
	if(!TESTBIT(res, 4)) {
		u8 top = (state >> 4) | 0xF0;
		res &= top;
	} else if(!TESTBIT(res, 5)) {
		u8 bot = (state & 0xF) | 0xF0;
		res &= bot;
	}
	
	return &res;
}

void Joypad::Update() {
	for(u32 key = 0; key < 8; ++key) {
		sf::Keyboard::Key k = layout[key];
		if(!sf::Keyboard::isKeyPressed(k)) {
			SETBIT(state, key);
		} else if(sf::Keyboard::isKeyPressed(k) && !TESTBIT(state, key)) {
			bool unset = TESTBIT(state, key);
			RESETBIT(state, key);
			bool button = key > 3;
			u8 key = mem->RawRead8(0xFF00);
			bool irq = (button && !TESTBIT(key, 5)) || (!button && !TESTBIT(key, 4));
			if(irq && !unset) gb->GetZ80()->DoIRQ(4);
			gb->GetZ80()->Unstop();
		}
	}
}
