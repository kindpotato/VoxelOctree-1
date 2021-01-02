#include "Player.h"
#include "Region.h"
Player::Player() {
	position = glm::vec3(0);
	velocity = glm::vec3(0);
	acceleration = glm::vec3(0);
	placeBlock = false; 
	leftMousePressed = false;
	blockType = 1;
}

void Player::update() {
	const float forceFactor = 0.1f;
    velocity += acceleration*forceFactor;
    position += velocity;
	velocity *= 0.9;
	acceleration = glm::vec3(0);
	position = glm::clamp(position, glm::vec3(0), glm::vec3(Region::width * Chunk::width));
    
}

void Player::handleInputs(GLFWwindow* window) {
	
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if (state == GLFW_PRESS && !leftMousePressed ) {
		placeBlock = true;
		leftMousePressed = true;
	}
	else if (state == GLFW_RELEASE) {
		leftMousePressed = false;
	}
	camera.pos = position + glm::vec3(0, 1, 0);
	camera.handleInputs(window);
	const int keyCount = 6;
	//w, a, s, d, space, shift
	bool keys[keyCount] = { 0 };

	short keyCodes[keyCount] = { GLFW_KEY_W,GLFW_KEY_A,GLFW_KEY_S,GLFW_KEY_D,GLFW_KEY_LEFT_SHIFT,GLFW_KEY_SPACE };
	for (int i = 0; i < keyCount; ++i) {
		if (glfwGetKey(window, keyCodes[i]) == GLFW_PRESS)
			keys[i] = true;
	}
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) blockType = 1;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) blockType = 2;
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) blockType = 3;
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) blockType = 4;
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) blockType = 5;

	glm::mat3 basis = camera.getMatrix();
	glm::vec3 forward(sin(camera.angle.x), 0, cos(camera.angle.x));
	acceleration = glm::vec3(0);
	
	if (keys[0])//W
		acceleration += forward;
	if (keys[1])//A
		acceleration -= basis[0];
	if (keys[2])//S
		acceleration -= forward;
	if (keys[3])//D
		acceleration += basis[0];
	if (keys[4])//space
		acceleration += glm::vec3(0,1,0);
	if (keys[5])//shift
		acceleration -= glm::vec3(0,1,0);
}
