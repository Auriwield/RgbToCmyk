#ifndef SPRITE_H
#define SPRITE_H

// GLEW	
#define GLEW_STATIC
#include <GL/glew.h>
#include "TextureImage.h"

class Sprite 
{
private:
	GLfloat *vertices;
	bool is_vertices_2d;
	GLfloat *texture_coords;
	GLfloat *indices;
	TextureImage texture;
public:

	Sprite(GLfloat *_vertices, GLfloat *_texture_coords, GLfloat *_indices, 
		TextureImage texture, bool _is_vertices_2d = false) : texture (texture)
	{
		vertices = _vertices;
		texture_coords = _texture_coords;
		indices = _indices;
		is_vertices_2d = _is_vertices_2d;
	}

	void set_vertices(GLfloat * _vertices, bool _is_vertices_2d = false) 
	{
		vertices = _vertices;
		is_vertices_2d = _is_vertices_2d;
	}

	void set_texture_coords(GLfloat *_texture_coords)
	{
		texture_coords = _texture_coords;
	}

	void set_indices(GLfloat *_indices)
	{
		indices = _indices;
	}


};

#endif