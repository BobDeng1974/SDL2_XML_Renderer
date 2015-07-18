#pragma once

#include "GL_Includes.h"
#include <string>

GLuint InitTexture(void * PXA, int w, int h);

GLuint FromSDLSurface(SDL_Surface * s);

GLuint FromImage(std::string fileName);

GLuint OutlineTexture(bool invert);