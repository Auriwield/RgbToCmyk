#include <iostream>
#include <algorithm> 
#include <cstdlib>

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

// GLEW	
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include <SOIL.h>

// Other includes
#include "Shader.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#include "stb_image_write.h";

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
std::string show_open_file_dialog();
void prepare_file(std::string inPath, std::string outPath, float * aspect_ratio);
std::string get_exe_path();
GLfloat * get_rect_coords(GLfloat left, GLfloat top, float width, float aspect_ratio);

// Window dimensions
const GLuint WIDTH = 600, HEIGHT = 600;

float x = 0;
float y = 0;
float tx = 0;
float ty = 0;
bool is_space_pressed;
bool is_mouse_left_pressed;
float dx, dy;

// The MAIN function, from here we start the application and run the game loop
int main(int argc, char **argv)
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Rgb versus Cmyk", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, WIDTH, HEIGHT);

	// Build and compile our shader program
	Shader ourShader("vertexShader.sh", "fragmentShader.sh");

	float aspect_ratio;

	std::string filePath = show_open_file_dialog();
	if (filePath[0] == '\0') return 0;

	std::string tempFilePath = get_exe_path().append("\\temp.png");
	prepare_file(filePath, tempFilePath, &aspect_ratio);

	GLfloat* left = get_rect_coords(-0.95f, 0.5f, 0.9f, aspect_ratio);
	GLfloat* right = get_rect_coords(0.0f, 0.5f, 0.9f, aspect_ratio);

	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat verticesLeft[] = {
		// Positions          // Texture Coords
		left[2],  left[1], 0.0f,    1.0f, 1.0f,  // Top Right
		left[2],  left[3], 0.0f,    1.0f, 0.0f,  // Bottom Right
		left[0],  left[3], 0.0f,    0.0f, 0.0f,  // Bottom Left
		left[0],  left[1], 0.0f,    0.0f, 1.0f  // Top Left 
	};

	GLfloat verticesRight[] = {
		right[2],  right[1], 0.0f,    1.0f, 1.0f,  // Top Right
		right[2],  right[3], 0.0f,    1.0f, 0.0f,  // Bottom Right
		right[0],  right[3], 0.0f,    0.0f, 0.0f,  // Bottom Left
		right[0],  right[1], 0.0f,    0.0f, 1.0f   // Top Left 
	};

	GLuint indices[] = {
		0, 1, 3, // First Triangle
		1, 2, 3  // Second Triangle
	};

	GLuint VBOLeft, VAOLeft, EBOLeft;
	glGenVertexArrays(1, &VAOLeft);
	glGenBuffers(1, &VBOLeft);
	glGenBuffers(1, &EBOLeft);

	glBindVertexArray(VAOLeft);

	glBindBuffer(GL_ARRAY_BUFFER, VBOLeft);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesLeft), verticesLeft, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOLeft);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// TexCoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO

	GLuint VBORight, VAORight, EBORight;
	glGenVertexArrays(1, &VAORight);
	glGenBuffers(1, &VBORight);
	glGenBuffers(1, &EBORight);

	glBindVertexArray(VAORight);

	glBindBuffer(GL_ARRAY_BUFFER, VBORight);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesRight), verticesRight, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBORight);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// TexCoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO

						  // Load and create a texture 
	GLuint textureLeft;
	GLuint textureRight;
	glGenTextures(1, &textureLeft);
	glGenTextures(1, &textureRight);
	glBindTexture(GL_TEXTURE_2D, textureLeft);

	// Set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load image, create texture and generate mipmaps

	int width, height;

	unsigned char* image = SOIL_load_image(filePath.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	glBindTexture(GL_TEXTURE_2D, textureRight);

	image = SOIL_load_image(tempFilePath.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Render
		// Clear the colorbuffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Activate shader
		ourShader.Use();

		glm::mat4 transform;
		transform = glm::translate(transform, glm::vec3(tx, ty, 0.0f));
		//transform = glm::rotate(transform, (GLfloat)glfwGetTime() * 50.0f, glm::vec3(0.0f, 0.0f, 1.0f));

		// Get matrix's uniform location and set matrix
		GLint transformLoc = glGetUniformLocation(ourShader.Program, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

		// Draw container
		glBindTexture(GL_TEXTURE_2D, textureLeft);
		glBindVertexArray(VAOLeft);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindTexture(GL_TEXTURE_2D, textureRight);
		glBindVertexArray(VAORight);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}
	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &VAOLeft);
	glDeleteBuffers(1, &VBOLeft);
	glDeleteBuffers(1, &EBOLeft);
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();

	return 0;
}

void mapMouseCoords(float *x, float *y) {
	if (*x < 0) *x = -1.0f;
	else if (*x > WIDTH) *x = 1.0f;
	else *x = (*x / (float)WIDTH * 2) - 1.0f;

	if (*y < 0) *y = -1.0f;
	else if (*y > HEIGHT) *y = 1.0f;
	else *y = (*y / (float)HEIGHT * 2) - 1.0f;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_SPACE) {
		is_space_pressed = action == GLFW_PRESS;
	}

	if (action != GLFW_PRESS) return;

	if (key == GLFW_KEY_ESCAPE)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	float x1 = (float)xpos;
	float y1 = (float)ypos;

	mapMouseCoords(&x1, &y1);

	dx = x1 - x;
	dy = y1 - y;

	x = x1;
	y = y1;

	if (is_mouse_left_pressed) {
		tx += dx;
		ty -= dy;
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		is_mouse_left_pressed = action == GLFW_PRESS;
	}
}

void prepare_file(std::string inPath, std::string outPath, float * aspect_ratio)
{
	int width, height;
	unsigned char *data = SOIL_load_image(inPath.c_str(), &width, &height, 0, SOIL_LOAD_RGB);

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int index = (width * y + x) * SOIL_LOAD_RGB;
			float r = data[index + 0] / 255.0f;
			float b = data[index + 1] / 255.0f;
			float g = data[index + 2] / 255.0f;

			float black = std::max(std::min(r, g), b);
			float n_black = 1.0f - black;

			float cyan = (black - r) / black;
			float magenta = (black - g) / black;
			float yellow = (black - b) / black;

			data[index + 0] = (unsigned char)(255.0f * (1 - cyan) * black);
			data[index + 1] = (unsigned char)(255.0f * (1 - magenta) * black);
			data[index + 2] = (unsigned char)(255.0f * (1 - yellow) * black);
		}
	}

	SOIL_save_image(outPath.c_str(), SOIL_SAVE_TYPE_BMP, width, height, SOIL_LOAD_RGB, data);
	SOIL_free_image_data(data);
	*aspect_ratio = (width * 1.0f) / (height * 1.0f);
}

std::string show_open_file_dialog()
{
	// This is where the filename your user selects will go.
	// Make sure it is long enough. (Especially if you start playing with the OFN_ALLOWMULTISELECT flag.)
	std::string filename(MAX_PATH, '\0');

	// This structure tells Windows how you want the Open File Dialog to appear.
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);

	// We'll add filters for text files (*.txt) and any file (*.*).
	// If you don't want any filters, just comment out this line (== set it to NULL).
	ofn.lpstrFilter = "Image file\0*.*\0";

	// Where to put the filename the user selects
	ofn.lpstrFile = (char*)filename.c_str();
	ofn.nMaxFile = filename.size();

	// Title of the dialog
	ofn.lpstrTitle = "Choose image file";

	// There are many more options, but right now we just want the user to 
	// select a file that actually exists.
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;


	if (GetOpenFileName(&ofn))
	{
		filename.resize(filename.find('\0'));
	}
	return filename;
}

std::string get_exe_path()
{
	char result[MAX_PATH];

	std::string filename = std::string(result, GetModuleFileName(NULL, result, MAX_PATH));

	std::string directory;
	const size_t last_slash_idx = filename.rfind('\\');
	if (std::string::npos != last_slash_idx)
	{
		directory = filename.substr(0, last_slash_idx);
	}

	return directory;
}

//return rect coords left, top, right, bottom
GLfloat * get_rect_coords(GLfloat left, GLfloat top, float width, float aspect_ratio) {

	float height = width / aspect_ratio;
	float right = left + width;
	float bottom = top - height;

	GLfloat * vertices = new GLfloat[4];
	vertices[0] = left;
	vertices[1] = top;
	vertices[2] = right;
	vertices[3] = bottom;

	return vertices;
}