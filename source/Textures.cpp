#include <iostream>
#include <vector>
using namespace std;

#include <SDL_image.h>

#include "GL_Includes.h"
#include <vec4.hpp>

namespace Textures{

	uint32_t flt_rgba32(vec4& C){
		uint32_t ret(0);
		for (int i = 0; i < 4; i++)
			((uint8_t *)&ret)[i] = C[i] * 0xFF;
		return ret;
	}

	uint32_t InitTexture(void * PXA, int w, int h){
		uint32_t tex;

		//Generate the device texture and bind it
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);

		//Upload host texture to device
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, PXA);

		//Does this really help?
		glGenerateMipmap(GL_TEXTURE_2D);

		// Set filtering   
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		//This necessary?
		float aniso = 0.0f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);

		//Unbind, Delete the host texture, return handle to device texture
		glBindTexture(GL_TEXTURE_2D, 0);

		return tex;
	}

	uint32_t FromSDLSurface(SDL_Surface * s){
		if (!s){
			cout << "Invalid SDL Surface" << endl;
			return 0;//This is bad
		}
		GLuint tex = InitTexture(s->pixels, s->w, s->h);

		//Unbind, Delete the host texture, return handle to device texture
		glBindTexture(GL_TEXTURE_2D, 0);

		return (uint32_t)tex;
	}

	uint32_t FromImage(string fileName){
		GLuint tex(0);
		SDL_Surface * s = IMG_Load(fileName.c_str());
		if (!s){
			cout << "Couldn't load image " << fileName.c_str() << endl;
			return 0;
		}
		// Make 32 bit RGBA if not
		if (s->format->format != SDL_PIXELFORMAT_RGBA8888){
			SDL_Surface * newS = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_RGBA8888, 0);
			SDL_FreeSurface(s);
			s = newS;
			for (int i = 0; i < s->w*s->h; i++)
				((uint32_t *)(s->pixels))[i] |= 0xFF000000;
		}
		tex = FromSDLSurface(s);
		if (!tex){
			cout << "Failed to load texture " << fileName.c_str() << endl;
			return 0;//This is bad
		}
		SDL_FreeSurface(s);
		
		return (uint32_t)tex;
	}

	uint32_t OutlineTexture(bool invert){
		const uint32_t white(0xFFFFFFFF), black(0xFF000000), DIM(100), th(8);
		vector<uint32_t> PXA(DIM*DIM);

		for (uint32_t y = 0; y < DIM; y++){
			for (uint32_t x = 0; x < DIM; x++){
				if (x<th || x>DIM - th || y<th || y>DIM - th)
					PXA[y*DIM + x] = (invert ? white : black);
				else
					PXA[y*DIM + x] = (invert ? black : white);
			}
		}

		return InitTexture(PXA.data(), DIM, DIM);
	}

	uint32_t FromSolidColor(vec4& C){
		GLuint tex(0);
		uint32_t color = flt_rgba32(C);
		const uint32_t DIM(100);
		vector<uint32_t> PXA(DIM*DIM, color);

		return InitTexture(PXA.data(), DIM, DIM);
	}
}