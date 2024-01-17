#include <iostream>
#include <map>
#include <string>
#include <sstream>

#define GLEW_STATIC
#include "GL/glew.h"	// Important - this header must come before glfw3 header
#include "GLFW/glfw3.h"

#include "GLM/glm.hpp"
#include "GLM/gtc/matrix_transform.hpp"
#include "GLM/gtc/type_ptr.hpp"
#include "ft2build.h" 
#include FT_FREETYPE_H 

//custom header
#include "ShaderProgram.h"  

// Global Variables
const char* APP_TITLE = "Text rendering using Freetype ";
const int gWindowWidth = 800;
const int gWindowHeight = 600;
GLFWwindow* gWindow = NULL;
bool gWireframe = false;



// Function prototypes
void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode);
void glfw_onFramebufferSize(GLFWwindow* window, int width, int height);
//void showFPS(GLFWwindow* window); //
bool initOpenGL();
void RenderText(ShaderProgram& shader, std::string text, float x, float y, float scale, glm::vec3 color);

//TYPE WRITTER
struct Character {
	unsigned int TextureID; // ID handle of the glyph texture
	glm::ivec2   Size;      // Size of glyph
	glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
	unsigned int Advance;   // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character> Characters;
unsigned int VAO, VBO;


//-----------------------------------------------------------------------------
// Main Application Entry Point
//-----------------------------------------------------------------------------
int main()
{
	if (!initOpenGL())
	{
		// An error occured
		std::cerr << "GLFW initialization failed" << std::endl;
		return -1;
	}


	GLfloat vertices[] = {
		0.0f, 0.5f, 0.0f, //for the top point
		0.5f, -0.5f,0.0f, //for the right point
		-0.5f,-0.5f,0.0f // for the lift point
	};

	//2. set up buffers on the GPU
	GLuint vbo, vao;

	glGenBuffers(1, &vbo); //create buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo); //bind the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//set the vao
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);

	glEnableVertexAttribArray(0);



	//call shaders
	ShaderProgram textShader, triangle;
	textShader.loadShaders("res/shaders/text.vert", "res/shaders/text.frag");
	triangle.loadShaders("res/shaders/triangle.vert", "res/shaders/triangle.frag");
	FT_Library ft;
	// All functions return a value different than 0 whenever an error occurred
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		return -1;
	}

	// find path to font
	std::string font_name = "res/fonts/SNAP____.TTF";
	if (font_name.empty())
	{
		std::cout << "ERROR::FREETYPE: Failed to load font_name" << std::endl;
		return -1;
	}
	// load font as face
	FT_Face face;
	if (FT_New_Face(ft, font_name.c_str(), 0, &face)) {
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		return -1;
	}
	else {
		// set size to load glyphs as
		FT_Set_Pixel_Sizes(face, 0, 48);

		// disable byte-alignment restriction
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// load first 128 characters of ASCII set
		for (unsigned char c = 0; c < 128; c++)
		{
			// Load character glyph 
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				continue;
			}
			// generate texture
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);
			// set texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// now store character for later use
			Character character = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				static_cast<unsigned int>(face->glyph->advance.x)
			};
			Characters.insert(std::pair<char, Character>(c, character));
		}
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	// destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);


	// configure VAO/VBO for texture quads
	// -----------------------------------
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	double lastTime = glfwGetTime();
	float cubeAngle = 0.0f;

	// Rendering loop
	while (!glfwWindowShouldClose(gWindow))
	{
		//showFPS(gWindow);
		double cuurentTime = glfwGetTime();
		double deltaTime = cuurentTime - lastTime;

		// Poll for and process events
		glfwPollEvents();

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//=====RENDERING THE TRIANGLE
		triangle.use();
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0); //unbind

		//RENDERING THE TEXT
		glm::mat4 projection(1.0), model(1.0), view(1.0);
		textShader.use();
		projection = glm::ortho(0.0f, static_cast<float>(gWindowWidth), 0.0f, static_cast<float>(gWindowHeight));
		glUniformMatrix4fv(glGetUniformLocation(textShader.getProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		RenderText(textShader, "0.0", 160.0f, 300.0f, 0.7f, glm::vec3(0.0, 0.0f, 0.0f));
		RenderText(textShader, "u", 180.0f, 280.0f, 0.7f, glm::vec3(0.0, 0.0f, 0.0f));
		RenderText(textShader, "this is my cutie triangle!", 160.0f, 200.0f, 0.7f, glm::vec3(0.0, 0.0f, 0.0f));


		// Swap front and back buffers
		glfwSwapBuffers(gWindow);

		lastTime = cuurentTime;
	}

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glfwTerminate();

	return 0;
}

//-----------------------------------------------------------------------------
// Initialize GLFW and OpenGL
//-----------------------------------------------------------------------------
bool initOpenGL()
{
	// Intialize GLFW 
	// GLFW is configured.  Must be called before calling any GLFW functions
	if (!glfwInit())
	{
		// An error occured
		std::cerr << "GLFW initialization failed" << std::endl;
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);	// forward compatible with newer versions of OpenGL as they become available but not backward compatible (it will not run on devices that do not support OpenGL 3.3

	// Create an OpenGL 3.3 core, forward compatible context window
	gWindow = glfwCreateWindow(gWindowWidth, gWindowHeight, APP_TITLE, NULL, NULL);
	if (gWindow == NULL)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}

	// Make the window's context the current one
	glfwMakeContextCurrent(gWindow);

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return false;
	}

	// Set the required callback functions
	glfwSetKeyCallback(gWindow, glfw_onKey);
	glfwSetFramebufferSizeCallback(gWindow, glfw_onFramebufferSize);

	glClearColor(1.0f, 0.5f, 0.7f, 1.0f);


	// Define the viewport dimensions
	glViewport(0, 0, gWindowWidth, gWindowHeight);

	//enable depth testing
	glEnable(GL_DEPTH_TEST);


	return true;
}

//-----------------------------------------------------------------------------
// Is called whenever a key is pressed/released via GLFW
//-----------------------------------------------------------------------------
void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_W && action == GLFW_PRESS)
	{
		gWireframe = !gWireframe;
		if (gWireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

//-----------------------------------------------------------------------------
// Is called when the window is resized
//-----------------------------------------------------------------------------
void glfw_onFramebufferSize(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}


void RenderText(ShaderProgram& shader, std::string text, float x, float y, float scale, glm::vec3 color)
{

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Disable the depth test since the text is being rendered in 2D
	glDisable(GL_DEPTH_TEST);
	// activate corresponding render state	
	shader.use();
	glUniform3f(glGetUniformLocation(shader.getProgram(), "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	// iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;
		// update VBO for each character
		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};
		// render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glEnable(GL_DEPTH_TEST);

	/**
	* Optionally disable the blending
	* Can be removed since the 3D world might have some parts that needs to be rendered (semi-)transparent
	*/
	glDisable(GL_BLEND);
}

//i found the solution for rendering the geometry and text at the same time here: https://gamedev.stackexchange.com/questions/179775/text-rendering-using-freetype-library-not-working-correctly
//i modified the RenderText... 