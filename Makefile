#
# Copyright 2018 duck
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

CC=g++
CCFLAGS=-I./include -lsfml-graphics -lsfml-window -lsfml-system -O3

OBJECTS=build/Cartridge.o	\
		build/Joypad.o		\
		build/main.o		\
		build/Memory.o		\
		build/PPU.o			\
		build/SleepyGB.o	\
		build/SleepyZ80.o	\
		build/Opcodes.o

%.o:
	$(CC) $(CCFLAGS) -c $(patsubst %.o,%.cpp,$(patsubst build%,src%,$@)) -o $@

all: build
	exit

build: $(OBJECTS)
	$(CC) $(CCFLAGS) $(OBJECTS) -o build/sleepygb

clean:
	rm build/$(OBJECTS)
