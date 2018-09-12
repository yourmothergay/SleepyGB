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
#include <SFML/Graphics.hpp>
#include <chrono>
#include <thread>
#include <cassert>
#include <string>
#include <iostream>
#include <cstdio>
#include <cstring>

SleepyGB::SleepyGB(Cartridge cart) {
	mem = new Memory(this, cart);
	ppu = new PPU(this);
	pad = new Joypad(this);
	z80 = new SleepyZ80(this);
	std::cout << "[GB] Created Memory, PPU, Joypad and CPU objects" << std::endl;
}

void SleepyGB::Run() {
	sf::RenderWindow window(sf::VideoMode(160, 144), "SleepyGB");

	window.setFramerateLimit(60);

	pad->SetWindow(&window);

	sf::Texture tex;
	tex.create(160, 144);

	u32 f = 0;

	sf::Clock d;
	sf::Sprite spr(tex);

	while(window.isOpen()) {
		sf::Event event;
		while(window.pollEvent(event)) {
			if(event.type == sf::Event::Closed)
				window.close();
		}
		
		for(u32 i = 0; i < 69905; ++i)
			z80->EmulateCycle();
		
		tex.update(ppu->GetScreenPixels(), 160, 144, 0, 0);
		tex.setSmooth(false);
		spr.setTexture(tex);
		window.clear(sf::Color::White);
		window.draw(spr);
		window.display();
		
		f = (f + 1) % 60;

		//std::cout << "[GB] " << d.restart().asSeconds() << " time delta" << std::endl;
	}
}
