#ifndef TETXTURE_H
#define TEXTURE_H

#include  "GL/glew.h"
#include <string>

using namespace std; // to avoide rewriting std::

class Texture {
public:
	Texture(); //constructor
	virtual ~Texture(); //to enable overload (rewrite)
	bool loadTexture(const string &filename, bool generateMipMap);
	void bind(GLuint texUnit = 0);
private: 
	GLuint mTexture; //to load multiple textures

};



#endif //