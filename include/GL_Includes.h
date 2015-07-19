#ifndef GL_INCLUDES
#define GL_INCLUDES

#include <GL/glew.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
//#include <GL/freeglut.h>
#include <SDL.h>
#include <SDL_opengl.h>
#endif

#include <fwd.hpp>
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::fquat;
using glm::mat4;
using glm::mat3;

#endif //GL_INCLUDES
