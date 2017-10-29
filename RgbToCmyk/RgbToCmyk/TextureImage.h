#ifndef TEXTURE_IMAGE_H
#define TEXTURE_IMAGE_H

#include <string>
//#include <iostream>
#include <SOIL.h>

class TextureImage
{
private:
	unsigned char* data;
	int width;
	int height;
	int channel_count;
	GLuint texture;
public:
	
	TextureImage(unsigned char* _data, int _width, int _height, int _channel_count)
	{
		data = _data;
		width = _width;
		height = _height;
		channel_count = _channel_count;
	}

	TextureImage(std::string filePath, int _channel_count = SOIL_LOAD_RGB) 
	{
		channel_count = _channel_count;
		data = SOIL_load_image(filePath.c_str(), &width, &height, 0, channel_count);
	}

	void init() 
	{
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void bind() 
	{
		glBindTexture(GL_TEXTURE_2D, texture);
	}

	~TextureImage() 
	{
		SOIL_free_image_data(data);
	}
};

#endif