#pragma once

class Picture {
public:
	int width, height, nrChannels;
	unsigned char* data;
	Picture(const char* name);
	void generateTexture();
};