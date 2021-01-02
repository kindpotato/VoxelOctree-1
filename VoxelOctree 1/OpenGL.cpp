#include "OpenGL.h"
#include <iostream>
#include <fstream>

glm::vec2 windowSize(1000.f);
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void framebuffer_size_callback(GLFWwindow* window, int w, int h)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.

	windowSize.x = w;
	windowSize.y = h;
	glViewport(0, 0, w,h);
}

void window_focus_callback(GLFWwindow* window, int focused) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}
GLFWwindow* initGLFW_GLAD(const char* name, int width, int height) {
	windowSize.x = width;
	windowSize.y = height;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(width,height, name, NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetWindowFocusCallback(window, window_focus_callback);
	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		
	return window;
}

unsigned int makeShader(std::string fileName, GLenum type) {
	unsigned int shaderHandle = glCreateShader(type);
	std::string fullStr = "";
	std::string line;
	std::ifstream myfile(fileName);
	if (myfile.is_open()) {
		while (getline(myfile, line))
		{
			fullStr += line + '\n';
		}
		myfile.close();

	}
	else {
		std::cout << "Could not read shader files." << std::endl;
	}
	
	const char* cShaderStr = fullStr.c_str();

	//std::cout << cShaderStr << std::endl;
	glShaderSource(shaderHandle, 1, &cShaderStr, NULL);
	glCompileShader(shaderHandle);
	int  success;
	char infoLog[512];
	glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shaderHandle, 512, NULL, infoLog);
		std::string typeName;
		switch (type) {
		case GL_VERTEX_SHADER:
			typeName = "VERTEX";
			break;
		case GL_FRAGMENT_SHADER:
			typeName = "FRAGMENT";
			break;
		default:
			typeName = "COMPUTE";
			break;
		}
		std::cout << "ERROR::" << typeName << " compilation failed:\n" << infoLog << std::endl;
	}
	return shaderHandle;
}



unsigned int makeProgram(unsigned int vert, unsigned int frag) {
	unsigned int program = glCreateProgram();
	glAttachShader(program, vert);
	glAttachShader(program, frag);
	glLinkProgram(program);

	int  success;
	char infoLog[512];
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cout << "ERROR::Shader program linking failed:\n" << infoLog << std::endl;
	}

	glDeleteShader(vert);
	glDeleteShader(frag);
	return program;
}

unsigned int makeProgram(unsigned int shader) {
	unsigned int program = glCreateProgram();
	glAttachShader(program, shader);
	glLinkProgram(program);

	int  success;
	char infoLog[512];
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cout << "ERROR::Shader program linking failed:\n" << infoLog << std::endl;
	}

	glDeleteShader(shader);
	return program;
}