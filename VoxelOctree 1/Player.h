#pragma once
#ifndef GLM
#define GLM
#include <glm/glm.hpp>
#endif

#include "Camera.h"
//#include "Chunk.h"
#ifndef GLFW
#define GLFW
#include <GLFW/glfw3.h>
#endif
class Player {
	bool leftMousePressed;
public:
	unsigned char blockType;
	bool placeBlock;
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	Camera camera;
	void handleInputs(GLFWwindow* window);
	void update();
	Player();
};