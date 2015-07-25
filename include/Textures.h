#pragma once

#include "GL_Includes.h"
#include <string>
#include <SDL.h>

// Namespace for texture creation functions
// relatively self explanatory

namespace Textures
{
	GLuint InitTexture(void * PXA, int w, int h);

	GLuint FromSDLSurface(SDL_Surface * s);

	GLuint FromImage(std::string fileName);

	GLuint FromSolidColor(vec4& C);

	GLuint OutlineTexture(bool invert);
}