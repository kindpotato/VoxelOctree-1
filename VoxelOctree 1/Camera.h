#pragma once
#ifndef GLM

#define GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#endif
#ifndef GLFW
#define GLFW
#include <GLFW/glfw3.h>
#endif
class Camera {
public:
	Camera();
	glm::vec3 pos;
	glm::vec2 angle; //yaw, pitch
	glm::vec2 lastMousePos;
	glm::mat3 getMatrix();
	glm::mat4 getMatrix4();
	void handleInputs(GLFWwindow*);
	glm::vec3 facingRay();

};