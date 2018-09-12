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

#ifndef __SLEEPYGB_PPU_HPP
#define __SLEEPYGB_PPU_HPP

class PPU {
	u8 *pixels;
	SleepyGB *gb;
	bool isEnabled;
	s32 scanlineCounter;
	Memory *mem;

	void WritePixel(u32 x, u32 y, u32 rgba);
	
	void DrawScanline(u32 scanline);
	
	void UpdateStatus(u32 scanline);
	
	void RenderTiles(u32 scanline);
	void RenderSprites(u32 scanline);
	
public:
	PPU(SleepyGB *gb);
	
	void Update(u32 cycles);
	
	inline u8 *GetScreenPixels() { return pixels; }
};

#endif//__SLEEPYGB_PPU_HPP
