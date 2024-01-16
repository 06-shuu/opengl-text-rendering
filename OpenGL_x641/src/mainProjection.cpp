//-----------------------------------------------------------------------------
// mainProjection.cpp 
//
// - create basic projection for 3d
// - modify vertex shader for projection
//-----------------------------------------------------------------------------
#include <iostream>
#include <sstream>
#define GLEW_STATIC
#include "GL/glew.h"	// Important - this header must come before glfw3 header
#include "GLFW/glfw3.h"

#include "GLM/glm.hpp"
#include "GLM/gtc/matrix_transform.hpp"
#include "GLM/gtc/type_ptr.hpp"

//my custom headers
#include "ShaderProgram.h"  
#include "texture.h"


// Global Variables
const char* APP_TITLE = "Computer Graphics - Projection Demo";
const int gWindowWidth = 800;
const int gWindowHeight = 600;
GLFWwindow* gWindow = NULL;
bool gWireframe = false;
const std::string texture1FileName = "res/texture/wall.jpg";
const std::string texture2FileName = "res/texture/mario.png";


//experimenting with transformation:

//1. translate
bool transDirection = true;
float offset = 0.0f;
float increment = 0.001f;
float maxOffset = 0.9f;

//2.rotate
float curAngle = 0.0f;

//2.scale
bool sizeDirection = true;
float maxSize = 0.8f;
float minSize = 0.1f;
float curSize = 0.3f;


// Shaders

// Function prototypes
void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode);
void glfw_onFramebufferSize(GLFWwindow* window, int width, int height);
void showFPS(GLFWwindow* window); //
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

	// Set up our quad

	// 1. Set up an array of vertices for a quad (2 triangls) with an index buffer data
	//   (What is a vertex?)
	GLfloat vertices[] = {
		// position		 // tex coords

	   // front face
	   -1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f, 1.0f,
	   -1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
	   -1.0f, -1.0f,  1.0f, 0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 1.0f, 0.0f,

		// back face
		-1.0f,  1.0f, -1.0f, 0.0f, 1.0f,
		 1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, -1.0f, 1.0f, 1.0f,
		-1.0f,  1.0f, -1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, -1.0f, 1.0f, 0.0f,

		 // left face
		 -1.0f,  1.0f, -1.0f, 0.0f, 1.0f,
		 -1.0f, -1.0f,  1.0f, 1.0f, 0.0f,
		 -1.0f,  1.0f,  1.0f, 1.0f, 1.0f,
		 -1.0f,  1.0f, -1.0f, 0.0f, 1.0f,
		 -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
		 -1.0f, -1.0f,  1.0f, 1.0f, 0.0f,

		 // right face
		  1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
		  1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
		  1.0f,  1.0f, -1.0f, 1.0f, 1.0f,
		  1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
		  1.0f, -1.0f,  1.0f, 0.0f, 0.0f,
		  1.0f, -1.0f, -1.0f, 1.0f, 0.0f,

		  // top face
		 -1.0f,  1.0f, -1.0f, 0.0f, 1.0f,
		  1.0f,  1.0f,  1.0f, 1.0f, 0.0f,
		  1.0f,  1.0f, -1.0f, 1.0f, 1.0f,
		 -1.0f,  1.0f, -1.0f, 0.0f, 1.0f,
		 -1.0f,  1.0f,  1.0f, 0.0f, 0.0f,
		  1.0f,  1.0f,  1.0f, 1.0f, 0.0f,

		  // bottom face
		 -1.0f, -1.0f,  1.0f, 0.0f, 1.0f,
		  1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
		  1.0f, -1.0f,  1.0f, 1.0f, 1.0f,
		 -1.0f, -1.0f,  1.0f, 0.0f, 1.0f,
		 -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
		  1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
	};

	//cube position
	glm::vec3 CubePos(0.0f, 0.0f, -5.5f); 


	// 2. Set up buffers on the GPU
	GLuint vbo, vao;

	glGenBuffers(1, &vbo);					// Generate an empty vertex buffer on the GPU
	glBindBuffer(GL_ARRAY_BUFFER, vbo);		// "bind" or set as the current buffer we are working with
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);	// copy the data from CPU to GPU

	glGenVertexArrays(1, &vao);				// Tell OpenGL to create new Vertex Array Object
	glBindVertexArray(vao);					// Make it the current one
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);	// Define a layout for the first vertex buffer "0"
	glEnableVertexAttribArray(0);			// Enable the first attribute or attribute "0"

	//for texture

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);			// Enable the first attribute or attribute "0"

	//call shaders
	ShaderProgram shaderProgram;
	shaderProgram.loadShaders("res/shaders/transform.vert", "res/shaders/transform.frag");

	////**********************
	//// TEXTURE
	//// *********************
	Texture texture1;
	texture1.loadTexture(texture1FileName, true);

	Texture texture2;
	texture2.loadTexture(texture2FileName, true);


	double lastTime = glfwGetTime();
	float cubeAngle = 0.0f;

	// Rendering loop
	while (!glfwWindowShouldClose(gWindow))
	{
		showFPS(gWindow);
		double cuurentTime = glfwGetTime();
		double deltaTime = cuurentTime - lastTime;

		// Poll for and process events
		glfwPollEvents();

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render the quad (two triangles)
		shaderProgram.use();

		//write code for texture here (to bind it)
		texture1.bind(0);
		texture2.bind(1);
		glUniform1i(glGetUniformLocation(shaderProgram.getProgram(), "texSampler1"), 0); //in one line
		glUniform1i(glGetUniformLocation(shaderProgram.getProgram(), "texSampler2"), 1); //in one line


		//Animation
		if (transDirection)
			offset += increment;
		else
			offset -= increment;
		if (abs(offset) >= maxOffset)
			transDirection = !transDirection;

		//rotation
		curAngle += 0.05f;
		if (curAngle >= 360)
			curAngle -= 360;

		//scalling 
		if (sizeDirection)
			curSize += 0.001f;
		else
			curSize -= 0.001f;
		if (curSize >= maxSize || curSize <= minSize)
			sizeDirection = !sizeDirection;

		//use deltaTime
		cubeAngle += (float)deltaTime * 50;
		if (cubeAngle >= 360)
			cubeAngle = 0.0f;

		//=========PRojection Demo
		//glm::mat4 model = glm::mat4(1.0); 
		//glm::mat4 view = glm::mat4(1.0);
		//glm::mat4 projection = glm::mat4(1.0);
		//or define it in one command:

		//============code for view
		glm::mat4 model(1.0), view(1.0), projection(1.0);

		//set the model
		//model = glm::scale(model, glm::vec3(curSize, curSize, curSize));
		//model = glm::translate(model, glm::vec3(0.0f, 0.0f, -10.5f));
		//model = glm::rotate(model, glm::radians(cubeAngle), glm::vec3(0.0f, 1.0f, 1.0f));
		              //translation                    //all this for rotation --------------------------until here  
		model = glm::translate(model, CubePos) * glm::rotate(model, glm::radians(cubeAngle), glm::vec3(0.0f, 1.0f, 0.0f));

		//set the view and lookat
		glm::vec3 camPos(0.0f, 0.0f, 0.0f);
		glm::vec3 targetPos(0.0f, 0.0f, -1.0f); //-1 because the camera is facing the opposite direction from the z
		glm::vec3 up(0.0f, 1.0f, 0.0f); //we set y=1 because it is the up 

		//create the view matrix
		view = glm::lookAt(camPos, targetPos, up);

		//creat the projection matrix
		projection = glm::perspective(glm::radians(45.0f), (float) gWindowWidth/gWindowHeight, 0.1f, 100.0f);
		                        //2 for x   //2 for y   //2 for z
		//projection = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 5.0f);


		//pass the uniforms to the shaders
		shaderProgram.setUniform("model", model);
		shaderProgram.setUniform("view", view);
		shaderProgram.setUniform("projection", projection);


		//set uniform for color with some fun
		GLfloat time = (GLfloat)glfwGetTime();
		GLfloat greenColor = (sin(time) / 2) + 0.5;
		shaderProgram.setUniform("vertColor", glm::vec4(1.0f, greenColor, 0.0f, 1.0f));

		//GLint vertexColorLocation = glGetUniformLocation(shaderProgram.getProgram(), "vertColor");
		//glUniform4f(vertexColorLocation, 0.0f, greenColor, 0.0f, 0.0f);

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		// Swap front and back buffers
		glfwSwapBuffers(gWindow);

		lastTime = cuurentTime;
	}

	// Clean up

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

	glClearColor(0.23f, 0.38f, 0.47f, 1.0f);

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

//-----------------------------------------------------------------------------
// Code computes the average frames per second, and also the average time it takes
// to render one frame.  These stats are appended to the window caption bar.
//-----------------------------------------------------------------------------
void showFPS(GLFWwindow* window)
{
	static double previousSeconds = 0.0;
	static int frameCount = 0;
	double elapsedSeconds;
	double currentSeconds = glfwGetTime(); // returns number of seconds since GLFW started, as double float

	elapsedSeconds = currentSeconds - previousSeconds;

	// Limit text updates to 4 times per second
	if (elapsedSeconds > 0.25)
	{
		previousSeconds = currentSeconds;
		double fps = (double)frameCount / elapsedSeconds;
		double msPerFrame = 1000.0 / fps;

		// The C++ way of setting the window title
		std::ostringstream outs;
		outs.precision(3);	// decimal places
		outs << std::fixed
			<< APP_TITLE << "    "
			<< "FPS: " << fps << "    "
			<< "Frame Time: " << msPerFrame << " (ms)";
		glfwSetWindowTitle(window, outs.str().c_str());

		// Reset for next average.
		frameCount = 0;
	}

	frameCount++;
}