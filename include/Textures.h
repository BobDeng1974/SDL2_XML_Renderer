#pragma once

#include "GL_Includes.h"
#include <string>

namespace Textures
{
	GLuint InitTexture(void * PXA, int w, int h);

	GLuint FromSDLSurface(SDL_Surface * s);

	GLuint FromImage(std::string fileName);

	GLuint FromSolidColor(vec4& C);

	GLuint OutlineTexture(bool invert);
}