#ifndef SPRITE_H
#define SPRITE_H

// GLEW	
#include <GL/glew.h>
#include "TextureImage.h"

static const size_t vx_size = 20;
static const size_t ix_size = 6;
static const size_t tx_size = 8;

class Sprite 
{
private:
	GLfloat *vertices;
	GLfloat *texture_coords;
	GLfloat *indices;
	TextureImage* texture;
	GLuint VBO, VAO, EBO;
	GLfloat * buildVxArray() {
		GLfloat* vx = new GLfloat[vx_size] {
			// Positions					 // Texture Coords
			vertices[2],  vertices[1], 0.0f, texture_coords[0], texture_coords[1],  // Top Right
			vertices[2],  vertices[3], 0.0f, texture_coords[2], texture_coords[3],  // Bottom Right
			vertices[0],  vertices[3], 0.0f, texture_coords[4], texture_coords[5],  // Bottom Left
			vertices[0],  vertices[1], 0.0f, texture_coords[6], texture_coords[7]   // Top Left 
		};

		return vx;
	}
public:

	// Sprite is a 2d image that is integrated into scene
	// vertices - coords of rectangle left/
	Sprite(GLfloat *_vertices, TextureImage* _texture) /*: texture (texture)*/
	{
		texture = _texture;
		vertices = _vertices;
		indices = new GLfloat[ix_size]{ 0, 1, 3, 1, 2, 3 };
		texture_coords = new GLfloat[tx_size]{ 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
	}

	void set_vertices(GLfloat * _vertices) 
	{
		vertices = _vertices;
	}

	void set_texture_coords(GLfloat *_texture_coords)
	{
		texture_coords = _texture_coords;
	}

	void set_indices(GLfloat *_indices)
	{
		indices = _indices;
	}

	void init() {
		GLfloat* vx = buildVxArray();

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vx[0]) * vx_size, vx, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices) * ix_size, indices, GL_STATIC_DRAW);

		// Position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		// TexCoord attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0); // Unbind VAO

		texture->init();
	}

	void draw() {
		texture->bind();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	~Sprite() {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
		delete texture;
	}
};

#endif