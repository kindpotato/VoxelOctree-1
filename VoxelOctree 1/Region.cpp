#include "Region.h"
#include <iostream>


bool Region::outside(glm::vec3 pos) {
	float width = (float)(Region::size * Chunk::size);
	return pos.x <= 1. || pos.y <= 1. || pos.z <= 1. || pos.x >= width - 1. || pos.y >= width - 1. || pos.z >= width - 1.;
}

unsigned int Region::vtoidx(glm::uvec3 v) {
	return (v.x << (depth << 1)) + (v.y << depth) + v.z;
}

Region::Region() {
	for (int i = 0; i < size; ++i) {
		space[i].space = nullptr;
	}
}

void printVec(glm::uvec3 v) {
	std::cout << v.x << " " << v.y << " " << v.z << std::endl;
}

Chunk* Region::getChunk(glm::uvec3 pos) {
	unsigned int idx = vtoidx(pos);
	if (idx < size)
		return &space[idx];
	//std::cout << "REGION: out of bounds" << std::endl;
	return &space[0];
}

void Region::addBlock(glm::uvec3 pos, unsigned int type) {
	glm::uvec3 whichChunk = toChunkPos(pos);
	unsigned int mask = Chunk::width - 1; //00011111
	pos.x &= mask;
	pos.y &= mask;
	pos.z &= mask;
	getChunk(whichChunk)->addBlock(pos, type);
}

unsigned char* Region::serializeChunks() {
	unsigned char* buffer = new unsigned char[Chunk::size * size];
	memset(buffer, 0, Chunk::size * size);
	int n = 0;
	for (int i = 0; i < size; ++i) {
		if (space[i].space != nullptr) {
			memcpy(buffer+n, space[i].space, Chunk::size);
			
		}
		else {
			memset(buffer+n, 0, Chunk::size);
		}
		n += Chunk::size;
	}
	
	return buffer;
}

glm::uvec3 Region::toChunkPos(glm::uvec3 pos) {
	glm::uvec3 whichChunk;
	whichChunk.x = pos.x >> Chunk::depth;
	whichChunk.y = pos.y >> Chunk::depth;
	whichChunk.z = pos.z >> Chunk::depth;
	return whichChunk;
}

unsigned char Region::getByte(glm::uvec3 pos, unsigned int level) {
	glm::uvec3 whichChunk = toChunkPos(pos);
	unsigned int mask = (Chunk::width - 1);
	pos.x &= mask;
	pos.y &= mask;
	pos.z &= mask;
	return getChunk(whichChunk)->getByte(pos,level);

}

glm::uvec3 Region::checkPlayer(Player& player) {
	if (player.placeBlock) {
		player.placeBlock = false;
		glm::vec3 v = player.camera.facingRay();
		glm::vec3 pos = player.camera.pos, frac;
		float minimum, t, ray = 0.;
		glm::vec3 sideColor(0);
		unsigned char blockInfo;
		for (int i = 0; i < 150; ++i) {
			int level = 5;

			do {
				blockInfo = getByte(glm::uvec3(pos), level);
				--level;
			} while (level >= 0 && bool(blockInfo));

			if (blockInfo == 0) {

				++level;
				sideColor = glm::vec3(0);
				minimum = 100;
				float nodeSize = float(1 << level);
				frac = -glm::mod(pos, nodeSize);
				const float tolerance = 0.0;
				t = frac.x / v.x;
				if (t < minimum && t > tolerance) {
					minimum = t;
					sideColor = glm::vec3(-1, 0, 0);
				}

				t = (nodeSize + frac.x) / v.x;
				if (t < minimum && t > tolerance) {
					minimum = t;
					sideColor = glm::vec3(1, 0, 0);
				}

				t = frac.y / v.y;
				if (t < minimum && t > tolerance) {
					minimum = t;
					sideColor = glm::vec3(0, -1, 0);
				}

				t = (nodeSize + frac.y) / v.y;
				if (t < minimum && t > tolerance) {
					minimum = t;
					sideColor = glm::vec3(0, 1, 0);
				}

				t = frac.z / v.z;
				if (t < minimum && t > tolerance) {
					minimum = t;
					sideColor = glm::vec3(0, 0, -1);
				}

				t = (nodeSize + frac.z) / v.z;
				if (t < minimum && t > tolerance) {
					minimum = t;
					sideColor = glm::vec3(0, 0, 1);
				}
				ray += minimum + 0.001;
				pos = player.camera.pos + ray * v;
				if (outside(pos)) {
					break;
				}
			}
			else { //In this condition we have collided with a block
				break;
			}
		}
		glm::uvec3 upos(player.camera.pos + (ray - 0.002f) * v);
		addBlock(upos, player.blockType);
		player.placeBlock = false;
		return toChunkPos(upos);
	}
	else
		return glm::uvec3(1000,1000,1000);
}

