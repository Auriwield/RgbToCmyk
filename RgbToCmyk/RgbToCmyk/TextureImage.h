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

public:
	
	TextureImage(unsigned char* _data, int _width, int _height, int _channel_count)
	{
		data = _data;
		width = _width;
		height = _height;
		channel_count = _channel_count;
	}

	TextureImage(std::string filePath, int _channel_count = SOIL_LOAD_RGB) {
		channel_count = _channel_count;
		data = SOIL_load_image(filePath.c_str(), &width, &height, 0, channel_count);
	}

	~TextureImage() {
		SOIL_free_image_data(data);
	}
};

#endif