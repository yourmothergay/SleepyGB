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

#ifndef __SLEEPYGB_JOYPAD_HPP
#define __SLEEPYGB_JOYPAD_HPP

#include <SFML/Graphics.hpp>

class Joypad {
	u8 state;
	SleepyGB *gb;
	sf::RenderWindow *w;
	Memory *mem;
	
public:
	Joypad(SleepyGB *gb);
	
	u8 GetState();

	u8 *GetStatePtr();
	
	void Update();

	inline void SetWindow(sf::RenderWindow *window) { w = window; }
};

#endif//__SLEEPYGB_JOYPAD_HPP
