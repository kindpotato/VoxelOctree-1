#include "Camera.h"
#include <iostream>
constexpr auto pi = 3.14159265;;

Camera::Camera() {
	lastMousePos = glm::vec3(0);
	pos = glm::vec3(2);
	angle = glm::vec2(0,0);

}

glm::vec3 Camera::facingRay() {
	return glm::vec3(cos(angle.y) * sin(angle.x), sin(angle.y),cos(angle.y) * cos(angle.x));
}
 
glm::mat3 Camera::getMatrix() {
	glm::vec3 Z = facingRay();
	glm::vec3 Y(0, 1, 0);
	glm::vec3 X = glm::normalize(glm::cross(Z, Y));
	Y = glm::cross(Z, X);
	return glm::mat3(X, Y, Z);
}

void print(glm::mat4 mat) {
	mat = glm::transpose(mat);
	for (int i = 0; i < 4; ++i) {
		std::cout << mat[i].x << ' ' << mat[i].y << ' ' << mat[i].z << ' ' << mat[i].w << '\n';
	}
}

glm::mat4 Camera::getMatrix4() {
	glm::mat4 trans(1);
	trans[3] = glm::vec4(-pos, 1);
	glm::mat3 camMat = getMatrix();
	glm::mat4 dir;
	dir[0] = glm::vec4(camMat[0], 0.f);
	dir[1] = glm::vec4(camMat[1], 0.f);
	dir[2] = glm::vec4(camMat[2], 0.f);
	dir[3] = glm::vec4(0, 0, 0, 1);
	return trans*dir;
}

void Camera::handleInputs(GLFWwindow* window) {
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	glm::vec2 mouse((float)xpos, (float)ypos);
	mouse -= lastMousePos;
	lastMousePos = glm::vec2((float)xpos, (float)ypos);
	const float mouseSensitivity = 0.001f;
	angle.x -= mouseSensitivity * mouse.x;
	angle.y += mouseSensitivity * mouse.y;

	if (angle.x < -pi)
		angle.x += 2.f * pi;
	else if (angle.x > pi)
		angle.x -= 2.f * pi;
	const float tolerance = 0.01;
	if (angle.y > pi / 2.f - tolerance)
		angle.y = pi / 2.f - tolerance;
	else if (angle.y < -pi / 2.f + tolerance)
		angle.y = -pi / 2.f + tolerance;
}