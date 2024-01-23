#include "VBO.h"

VBO::VBO() {
	glGenBuffers(1, &ID);
}

VBO::~VBO() {
	glDeleteBuffers(1, &ID);
}

void VBO::bind() {
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}

void VBO::unbind() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::BufferData(GLsizeiptr size, GLfloat* vertices) {
	//bind();
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}