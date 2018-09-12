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

#ifndef __SLEEPYGB_HPP
#define __SLEEPYGB_HPP

#define TESTBIT(x, y) (((x) >> (y)) & 1 != 0)
#define SETBIT(x, y) ((x) |= (1 << (y)))
#define BITSET(x, y) ((x) | (1 << (y)))
#define RESETBIT(x, y) if((x) & (1 << (y))) (x) ^= (1 << (y))
#define BITRESET(x, y) ((x) & (1 << (y)) ? (x) ^ (1 << (y)) : (x))
#define GETBIT(x, y) (((x) >> (y)) & 1)

class Memory;
class PPU;
class SleepyZ80;
class Joypad;

#include <iostream>
#include "SleepyGB/Types.hpp"
#include "SleepyGB/Cartridge.hpp"
#include "SleepyGB/SleepyGB.hpp"
#include "SleepyGB/SleepyZ80.hpp"
#include "SleepyGB/Memory.hpp"
#include "SleepyGB/PPU.hpp"
#include "SleepyGB/Joypad.hpp"

#endif//__SLEEPYGB_HPP
