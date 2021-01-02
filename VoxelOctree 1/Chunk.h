#pragma once
#include <vector>

#ifndef GLM
#define GLM
#include <glm/glm.hpp>
#endif
class Chunk {
	unsigned int vtoidx(glm::uvec3 v);
public:
	unsigned char* space;
	const static int depth = 5;
	const static int width = 1 << depth;
	const static int size = width * width * width;
	Chunk();
	void addBlock(glm::uvec3 pos, unsigned char type);
	unsigned char getByte(glm::uvec3 pos, unsigned int level);
	
};