#pragma once
#include <random>
#ifndef GLM
#define GLM
#include <glm/glm.hpp>
#endif
#include <vector>
#include <thread>
class Perlin {
	float lerp(float a, float b, float c) {
		return a* (1.f - c) + b * c;
	}

	unsigned int indexof(glm::vec3 pos) {
		glm::uvec3 upos = glm::uvec3(floor(pos));
		return dims.y* dims.z* upos.x + dims.z * upos.y + upos.z;
	}
public:
	std::vector<glm::vec3> space;
	glm::uvec3 dims;

	float sample(glm::vec3 pos) {
		pos = glm::clamp(pos, glm::vec3(0), glm::vec3(dims) - 0.1f);
		glm::vec3 frac = glm::fract(pos);
		float dots[8];
		glm::vec3 displacement;
		for (int i = 0; i < 8; ++i) {
			glm::vec3 offset(0);
			if (1 & i)
				offset.x += 1.f;
			if (2 & i)
				offset.y += 1.f;
			if (4 & i)
				offset.z += 1.f;
			displacement = frac - offset;
			displacement *= -1.f;
			try {
				dots[i] = glm::dot(space[indexof(pos + offset)], displacement);
			}
			catch (const std::exception& oor ) {
				std::cout << "Pos: " << pos.x << " " << pos.y << " " << pos.z << "\nidx: " << indexof(pos + offset) << std::endl;
				std::this_thread::sleep_for(std::chrono::seconds(60));
				exit(1);
			}
			
			
		}
		float c00 = lerp(dots[0], dots[4], frac.x);
		float c01 = lerp(dots[1], dots[5], frac.x);
		float c10 = lerp(dots[2], dots[6], frac.x);
		float c11 = lerp(dots[3], dots[7], frac.x);

		float c0 = lerp(c00, c10, frac.y);
		float c1 = lerp(c01, c11, frac.y);

		return lerp(c0, c1, frac.z);
		

	}
	Perlin(glm::uvec3 dimensions) {
		std::mt19937 generator;
		dims = dimensions;
		space.resize(dims.x * dims.y * dims.z);
		std::normal_distribution<float> dist{ 0,1 };
		for (glm::vec3& vec : space) {
			vec.x = dist(generator);
			vec.y = dist(generator);
			vec.z = dist(generator);
			vec = glm::normalize(vec);
		}
	}
};