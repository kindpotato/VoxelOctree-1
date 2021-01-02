#include "Chunk.h"
#include <iostream>

void printByte(unsigned char byte) {
	for (int i = 7; i >= 0; --i) {
		std::cout << ((byte >> i) & 1);
	}
	std::cout << std::endl;
}

unsigned int Chunk::vtoidx(glm::uvec3 pos) {
	return (pos.x << (Chunk::depth << 1)) + (pos.y << Chunk::depth) + pos.z;
}
void Chunk::addBlock(glm::uvec3 pos, unsigned char type) {
	if ((pos.x >= width) || (pos.y >= width) || (pos.z >= width)) {
		std::cout << "out of bounds" << std::endl;
		return;
	}

	if (space == nullptr) {
		space = new unsigned char[size];
		memset(space, 0, size);
	}
	type = type & 7; //clears 5 of the most significant bits.
	unsigned int idx = vtoidx(pos);
	unsigned char byte = space[idx];
	
	byte &= (~7); //clears 3 least sig figs
	byte |= type; //writes 3 least sig figs
	space[idx] = byte;
	for (int i = 0; i < depth; ++i) {
		pos.x &= ~(1 << i);// knocks off increasingly more significant bits
		pos.y &= ~(1 << i);// with each iteration
		pos.z &= ~(1 << i);
		//std::cout << x << ' ' << y << ' ' << z << std::endl;
		idx = vtoidx(pos);
		byte = space[idx];
		unsigned char mask = (1 << (i + 3));
		//get rid of the "1&"
		bool flag = 1&(mask & byte); //if the bit was already set stop
		byte |= mask;
		space[idx] = byte;
		/*if (flag)
			break;*/

	}
	
}

Chunk::Chunk() {
	space = nullptr;
}
unsigned char Chunk::getByte(glm::uvec3 pos, unsigned int level) {
	if (space == nullptr) {
		return 0;
	}
	unsigned int mask = ~((1 << level) - 1); //round off level bits
	pos.x &= mask;
	pos.y &= mask;
	pos.z &= mask;
	unsigned int idx = vtoidx(glm::uvec3(pos));
	unsigned char block = space[idx];
	if (level == 0)
		return block & 7;
	return 1 & (block >> (2 + level));
}