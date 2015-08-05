#ifndef GL_INCLUDES
#define GL_INCLUDES

// Adds OpenGL, SDL2, and the glm forwards

#include <GL/glew.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include <fwd.hpp>
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::fquat;
using glm::mat4;
using glm::mat3;

#include <iostream>

// Unfortunately these are implementeed in main.cpp
std::ostream& operator<<(std::ostream& os, const vec2& vec);
std::ostream& operator<<(std::ostream& os, const vec3& vec);
std::ostream& operator<<(std::ostream& os, const vec4& vec);
std::ostream& operator<<(std::ostream& os, const mat4& mat);
std::ostream& operator<<(std::ostream& os, const fquat& quat);

#define COLOR_TEX_UNIT 0
#define NORMAL_TEX_UNIT 1
#define CUBE_TEX_UNIT 2

#endif //GL_INCLUDES
