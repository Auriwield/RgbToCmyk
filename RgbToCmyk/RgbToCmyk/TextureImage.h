#ifndef TEXTURE_IMAGE_H
#define TEXTURE_IMAGE_H

#include <string>
#include <map>
#include <SOIL.h>
#include <math.h>  

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

	unsigned char diff(unsigned char a, unsigned char b) {
		return (a > b) ? a - b : b - a;
	}

	TextureImage * count_diff(TextureImage * right) {
		int w = std::min(this->width, right->width);
		int h = std::min(this->height, right->height);

		int length = w*h*SOIL_LOAD_RGB;
		unsigned char* diffData = new unsigned char[length];

		for (int i = 0; i < length; i++)
		{
			float a = diff(this->data[i], right->data[i]) * 255.0f;
			if (a > 255.f) a = 255.0f;
			diffData[i] = (unsigned char)a;
		}

		return new TextureImage(diffData, w, h, SOIL_LOAD_RGB);
	}

	float mse(TextureImage * right)
	{
		int w = std::min(this->width, right->width);
		int h = std::min(this->height, right->height);

		float sum = 0.0f;

		for (int y = 0; y < h; y++)
		{
			for (int x = 0; x < w; x++)
			{
				int index = (y * w + x) * SOIL_LOAD_RGB;

				float l = (data[index] + data[index + 1] + data[index + 2]) / 3;
				float r = (right->data[index] + right->data[index + 1] + right->data[index + 2]) / 3;
				float ms = l - r;
				sum += ms * ms;
			}
		}
		return sum;
	}

	float cuberp(float a, float b, float c, float d, float m) 
	{
		float a0, a1, a2, a3, mu2;

		mu2 = m * m;
		a0 = d - c - a + b;
		a1 = a - b - a0;
		a2 = c - a;
		a3 = b;

		return (a0 * m * mu2 + a1 * mu2 + a2 * m + a3);
	}

	TextureImage * histogram()
	{
		int w = this->width;
		int h = this->height;

		int length = w*h*SOIL_LOAD_RGB;
		unsigned char* histData = new unsigned char[length];

		std::map<int, int> mapR;
		std::map<int, int> mapG;
		std::map<int, int> mapB;

		for (int i = 0; i <= 255; i++)
		{
			mapR[i] = 0;
			mapG[i] = 0;
			mapB[i] = 0;
		}

		for (int i = 0; i < length; i += SOIL_LOAD_RGB)
		{
			mapR[data[i]]++;
			mapB[data[i + 1]]++;
			mapG[data[i + 2]]++;
		}

		int maxR = 0;
		int maxG = 0;
		int maxB = 0;

		for (int i = 0; i < 255; i++)
		{
			if (mapR[i] > maxR) maxR = mapR[i];
		}

		for (int i = 0; i < 255; i++)
		{
			if (mapG[i] > maxG) maxG = mapG[i];
		}

		for (int i = 0; i < 255; i++)
		{
			if (mapB[i] > maxB) maxB = mapB[i];
		}

		int w2 = w, h2 = h;

		float dw = w2 / 256.0f;
		
		int max = std::max(maxR, std::max(maxG, maxB));

		mapR[-1] = mapR[0];
		mapR[256] = mapR[255];

		mapG[-1] = mapG[0];
		mapG[256] = mapG[255];

		mapB[-1] = mapB[0];
		mapB[256] = mapB[255];

		for (int i = 0; i < h2; i++) 
		{
			for (int j = 0; j < w2; j++) 
			{
				int destIndex = (i * w2 + j) * SOIL_LOAD_RGB;

				float j0 = j / dw;
				int srcIndex = std::floor(j0);
				int srcIndex2 = srcIndex < 255 ? srcIndex + 1 : 255;
				float t = j0 - srcIndex;
				int nh2 = h2 - i;

				float valR = cuberp(mapR[srcIndex - 1], mapR[srcIndex], mapR[srcIndex2], mapR[srcIndex2 + 1], t);
				float valG = cuberp(mapG[srcIndex - 1], mapG[srcIndex], mapG[srcIndex2], mapG[srcIndex2 + 1], t);
				float valB = cuberp(mapB[srcIndex - 1], mapB[srcIndex], mapB[srcIndex2], mapB[srcIndex2 + 1], t);

				if (nh2 <= valR / max * h2) histData[destIndex] = 255;

				if (nh2 <= valG / max * h2) histData[destIndex + 1] = 255;

				if (nh2 <= valB / max * h2) histData[destIndex + 2] = 255;
			}
		}

		return new TextureImage(histData, w, h, SOIL_LOAD_RGB);
	}

	~TextureImage() 
	{
		SOIL_free_image_data(data);
	}
};

#endif