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


//custom header
#include "ShaderProgram.h"  
#include "Freetype.h"
#include "VAO.h"
#include "VBO.h"

// Global Variables
const char* APP_TITLE = "Text rendering using Freetype ";
const int gWindowWidth = 800;
const int gWindowHeight = 600;
GLFWwindow* gWindow = NULL;
bool gWireframe = false;


// Function prototypes
void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode);
void glfw_onFramebufferSize(GLFWwindow* window, int width, int height);
bool initOpenGL();


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


	VBO triangleVBO;
	triangleVBO.bind();
	triangleVBO.BufferData(sizeof(vertices), vertices);

	VAO triangleVAO;
	triangleVAO.bind();
	triangleVAO.Attribpointer(0, 3, GL_FLOAT, 3 * sizeof(GLfloat), (GLvoid*)0);
	
	//call shaders
	ShaderProgram textShader, triangle;
	textShader.loadShaders("res/shaders/text.vert", "res/shaders/text.frag");
	triangle.loadShaders("res/shaders/triangle.vert", "res/shaders/triangle.frag");

	//====freetype===
	FreeTypeFont font;
	if (!font.initFreeType()) {
		// Handle initialization failure
		return -1;
	}

	font.loadCharacters("res/fonts/SNAP____.TTF");
	//===============

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
		triangleVAO.bind();
		glDrawArrays(GL_TRIANGLES, 0, 3);
		triangleVAO.unbind();

		//RENDERING THE TEXT
		glm::mat4 projection(1.0), model(1.0), view(1.0);
		textShader.use();
		projection = glm::ortho(0.0f, static_cast<float>(gWindowWidth), 0.0f, static_cast<float>(gWindowHeight));
		textShader.setUniform("projection", projection);
		font.renderText(textShader, "0.0", 160.0f, 300.0f, 0.7f, glm::vec3(0.0, 0.0f, 0.0f));
		font.renderText(textShader, "u", 180.0f, 280.0f, 0.7f, glm::vec3(0.0, 0.0f, 0.0f));
		font.renderText(textShader, "this is my cutie triangle!", 160.0f, 200.0f, 0.7f, glm::vec3(0.0, 0.0f, 0.0f));

		// Swap front and back buffers
		glfwSwapBuffers(gWindow);

		lastTime = cuurentTime;
	}

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
