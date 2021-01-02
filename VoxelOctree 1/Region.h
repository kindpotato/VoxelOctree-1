#pragma once
#include "Chunk.h"
#include "Player.h"
#include <string>
class Region {
public:
	const static unsigned int width = 8, depth = 3;
	const static unsigned int size = width * width * width;
private:
	Chunk space[size];
	
	bool outside(glm::vec3 pos);
public:
	static unsigned int vtoidx(glm::uvec3 v);
	static glm::uvec3 toChunkPos(glm::uvec3 pos);
	Chunk* getChunk(glm::uvec3 pos);
	unsigned char* serializeChunks();
	void addBlock(glm::uvec3 pos, unsigned int type);
	Region();
	glm::uvec3 checkPlayer(Player& player);
	unsigned char getByte(glm::uvec3 pos,unsigned int);
	//bool save(std::string fileName);
};

void printVec(glm::uvec3 v);