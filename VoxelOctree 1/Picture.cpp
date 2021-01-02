#include "Picture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "OpenGL.h"
Picture::Picture(const char* name) {
	data = stbi_load(name, &width, &height, &nrChannels, 0);
}

void Picture::generateTexture() {
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
}