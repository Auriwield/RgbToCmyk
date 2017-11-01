#include <iostream>
#include <algorithm> 
#include <cstdlib>
#include <math.h>
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
#include "Sprite.h"
#include <vector>;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
std::string show_open_file_dialog();
void prepare_file(std::string inPath, std::string outPath, float * aspect_ratio);
std::string get_exe_path();
GLfloat * get_rect_coords(GLfloat left, GLfloat top, float width, float aspect_ratio);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// Window dimensions
const GLuint WIDTH = 600, HEIGHT = 600;

float x = 0;
float y = 0;
float tx = 0;
float ty = 0;
bool is_space_pressed;
bool is_mouse_left_pressed;
float dx, dy;
float scale = 1.0f;

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
	glfwSetScrollCallback(window, scroll_callback);
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

	float h = 0.9f / aspect_ratio;

	//GLfloat* leftCoords = get_rect_coords(-0.5, 0.5f, 1.0f, aspect_ratio);
	GLfloat* leftCoords = get_rect_coords(-0.95f, 0.5f, 0.9f, aspect_ratio);
	GLfloat* rightCoords = get_rect_coords(0.0f, 0.5f, 0.9f, aspect_ratio);

	GLfloat* histLeftCoords = get_rect_coords(-0.95f, 0.45f - h , 0.9f, aspect_ratio);
	GLfloat* histRightCoords = get_rect_coords(0.0f, 0.45f - h, 0.9f, aspect_ratio);

	GLfloat* diffCoords = get_rect_coords(-0.45f, 0.4f - (h*2), 0.9f, aspect_ratio);

	TextureImage* leftText = new TextureImage(filePath);
	TextureImage* rightText = new TextureImage(tempFilePath);

	TextureImage* leftHistText = leftText->histogram();
	TextureImage* rightHistText = rightText->histogram();

	TextureImage* diff = leftText->count_diff(rightText);

	float mse = leftText->mse(rightText);
	printf("MSE = %.4f\n", mse);

	int spritesCount = 5;
	Sprite ** sprites = new Sprite*[spritesCount];

	sprites[0] = new Sprite(leftCoords, leftText);;
	sprites[1] = new Sprite(rightCoords, rightText);
	sprites[2] = new Sprite(histLeftCoords, leftHistText);;
	sprites[3] = new Sprite(histRightCoords, rightHistText);
	sprites[4] = new Sprite(diffCoords, diff);

	// Set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	for (int i = 0; i < spritesCount; i++)
	{
		sprites[i]->init();
	}

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
		transform = glm::scale(transform, glm::vec3(scale, scale, 0.0f));
		transform = glm::translate(transform, glm::vec3(tx, ty, 0.0f));

		// Get matrix's uniform location and set matrix
		GLint transformLoc = glGetUniformLocation(ourShader.Program, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

		/*for (auto sprite : sprites) // access by reference to avoid copying
		{
			sprite->draw();
		}*/

		for (int i = 0; i < spritesCount; i++)
		{
			sprites[i]->draw();
		}

		glBindVertexArray(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

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
		tx += dx * 1.0f / scale;
		ty -= dy * 1.0f / scale;
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		is_mouse_left_pressed = action == GLFW_PRESS;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	float factor = 1.0f + yoffset / 20;

	scale *= factor;
	//tx *= factor;
	//ty *= factor;

	//tx -= (x+tx) * factor - (x+tx);
	//ty += (y-ty) * factor - (y-ty);
}

void rgb_to_cmyk(int r, int g, int b, int* c, int* m, int* y, int* k, int scale = 100) 
{
	float cf, mf, yf, kf;
	cf = 1 - r / 255.0f;
	mf = 1 - g / 255.0f;
	yf = 1 - b / 255.0f;
	kf = std::min(std::min(cf, mf), yf);

	cf = (cf - kf) / (1 - kf);
	mf = (mf - kf) / (1 - kf);
	yf = (yf - kf) / (1 - kf);

	*c = cf * scale;
	*m = mf * scale;
	*y = yf * scale;
	*k = kf * scale;
}		

void cmyk_to_rgb(int c, int m, int y, int k, int* r, int* g, int* b, int scale = 100)
{
	float cf = c * 1.0f / scale;
	float mf = m * 1.0f / scale;
	float yf = y * 1.0f / scale;
	float kf = k * 1.0f / scale;

	cf = cf * (1 - kf) + kf;
	mf = mf * (1 - kf) + kf;
	yf = yf * (1 - kf) + kf;

	*r = (1 - cf) * 255.0f;
	*g = (1 - mf) * 255.0f;
	*b = (1 - yf) * 255.0f;
}

void prepare_file(std::string inPath, std::string outPath, float * aspect_ratio)
{
	int width, height;
	unsigned char *data = SOIL_load_image(inPath.c_str(), &width, &height, 0, SOIL_LOAD_RGB);

	for (int y1 = 0; y1 < height; y1++)
	{
		for (int x = 0; x < width; x++)
		{
			int r, g, b, c, m, y, k;
			int index = (width * y1 + x) * SOIL_LOAD_RGB;

			r = data[index + 0];
			g = data[index + 1];
			b = data[index + 2];

			rgb_to_cmyk(r, g, b, &c, &m, &y, &k);
			cmyk_to_rgb(c, m, y, k, &r, &g, &b);

			data[index + 0] = (unsigned char) r;
			data[index + 1] = (unsigned char) g;
			data[index + 2] = (unsigned char) b;
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