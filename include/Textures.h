#pragma once

#include "GL_Includes.h"
#include <string>
#include <SDL.h>

// Namespace for texture creation functions
// relatively self explanatory
// TODO handle different image formats, normals, better

namespace Textures
{
	GLuint InitTexture(void * PXA, int w, int h, int h_Format, int d_Format);

	GLuint FromSDLSurface(SDL_Surface * s);

	GLuint ColorTexture(std::string fileName);

	GLuint FromSolidColor(vec4& C);

	GLuint OutlineTexture(bool invert);

	//GLuint TextureMap(std::string fileName); you know what to do

	GLuint NormalTexture(std::string fileName);

	GLuint CubeMap(std::string faces[6]);
}