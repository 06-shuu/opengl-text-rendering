#ifndef VBO_H
#define VBO_H

#include "GL/glew.h"	// Important - this header must come before glfw3 header
#include "GLFW/glfw3.h"

//using namespace std;

class VBO {
public:
	VBO();
	~VBO();
	void bind();
	void unbind();
	void BufferData(GLsizeiptr size, GLfloat* vertices);

private:
	GLuint ID;
};

#endif