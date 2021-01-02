#pragma once
#ifndef OPENGL
#define OPENGL

#include <string>
#include <glad/glad.h>
#ifndef GLFW
#define GLFW
#include <GLFW/glfw3.h>
#endif


#ifndef GLM
#define GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>>
#endif

extern glm::vec2 windowSize;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void window_focus_callback(GLFWwindow* window, int focused);
void processInput(GLFWwindow* window);

GLFWwindow* initGLFW_GLAD(const char* name, int width, int height);
unsigned int makeShader(std::string fileName, GLenum);
unsigned int makeProgram(unsigned int vert, unsigned int frag);
unsigned int makeProgram(unsigned int shader);
#endif