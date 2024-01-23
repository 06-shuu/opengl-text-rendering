// freetype.h

#ifndef FREETYPE_H
#define FREETYPE_H

#include <map>
#include <string>
#include "ShaderProgram.h"
#include "GLM/glm.hpp"

struct Character {
    unsigned int TextureID;
    glm::ivec2   Size;
    glm::ivec2   Bearing;
    unsigned int Advance;
};

class FreeTypeFont {
public:
    FreeTypeFont();
    ~FreeTypeFont();

    bool initFreeType();
    void loadCharacters(const std::string& fontPath);
    void renderText(ShaderProgram& shader, const std::string& text, float x, float y, float scale, const glm::vec3& color);

private:
    std::map<GLchar, Character> Characters;
    unsigned int VAO, VBO;
};

#endif // FREETYPE_H
