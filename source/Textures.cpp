#include <iostream>
#include <vector>
using namespace std;

#include <SDL_image.h>

#include "Textures.h"
#include "GL_Includes.h"
#include <vec4.hpp>
#include <vec3.hpp>

#include <algorithm>

// So what I want to do here is cache off textures by fileName and
// color format inside a map. Any queries for that specific BPP
// will return the cached GPU resource, otherwise a new one will
// be created. Furthermore, knowledge of which texture unit to bind will be managed
// here. If I can assume client binding of an appropriate shader, then this thin
// Solid color textures aren't handled, because that's stupid anyway

namespace Textures{

	// float4 color to rgba32
	uint32_t flt_rgba32(vec4& C){
		uint32_t ret(0);
		for (int i = 0; i < 4; i++)
			((uint8_t *)&ret)[i] = C[i] * 0xFF;
		return ret;
	}

	static SDL_Surface * getSurfaceFromImage(std::string fileName, uint32_t desiredFormat = SDL_PIXELFORMAT_RGBA8888){

		SDL_Surface * s = IMG_Load(fileName.c_str());
		if (!s){
			cout << "Couldn't load image " << fileName.c_str() << endl;
			return nullptr;
		}
		if (s->format->format != desiredFormat){
			SDL_Surface * newS = SDL_ConvertSurfaceFormat(s, desiredFormat, 0);
			if (!newS){
				cout << "Error: unablet to convert image to desired format " << fileName.c_str() << endl;
				SDL_FreeSurface(newS);
			}
			else{
				// Find a better way of doing this
				if (s->format->BytesPerPixel == 3 && newS->format->BytesPerPixel == 4)
					for (int i = 0; i < newS->w*newS->h; i++)
						((uint32_t *)(newS->pixels))[i] |= 0xFF000000;
			}
			SDL_FreeSurface(s);
			s = newS;
		}

		return s;
	}

	// Given data and dims, create a texture on device
	uint32_t InitTexture(void * PXA, int w, int h, int h_Format, int d_Format){
		uint32_t tex;

		//Generate the device texture and bind it
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);

		//Upload host texture to device
		glTexImage2D(GL_TEXTURE_2D, 0, h_Format, w, h, 0, d_Format, GL_UNSIGNED_BYTE, PXA);

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

	// Texture from an SDL surface
	uint32_t FromSDLSurface(SDL_Surface * s){
		if (!s){
			cout << "Invalid SDL Surface" << endl;
			return 0;//This is bad
		}

		const uint32_t glFormats[4][2] = {
			{ GL_R8, GL_RED },
			{ 0, 0 },
			{ GL_RGB8, GL_RGB },
			{ GL_RGBA8, GL_RGBA }
		};
		const uint32_t * fmts = glFormats[s->format->BytesPerPixel - 1];

		GLuint tex = InitTexture(s->pixels, s->w, s->h, fmts[0], fmts[1]);

		//Unbind, Delete the host texture, return handle to device texture
		glBindTexture(GL_TEXTURE_2D, 0);

		return (uint32_t)tex;
	}

	// The dim for these two seems arbitrary
	// Create a black/white texture with an outline (invert controls border color)
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

		// These could use less bytes, but whatever
		return InitTexture(PXA.data(), DIM, DIM, GL_RGBA8, GL_RGBA);
	}

	// Create a texture from a solid color
	uint32_t FromSolidColor(vec4& C){
		uint32_t color = flt_rgba32(C);
		const uint32_t DIM(100);
		vector<uint32_t> PXA(DIM*DIM, color);

		return InitTexture(PXA.data(), DIM, DIM, GL_RGBA8, GL_RGBA);
	}

	uint32_t FromImage(std::string fileName, uint32_t fmt){
		GLuint img(0);
		SDL_Surface * s = getSurfaceFromImage(fileName, fmt);

		if (!s){
			cout << "Couldn't load image texture " << fileName.c_str() << endl;
			return 0;
		}
		img = FromSDLSurface(s);
		if (!img){
			cout << "Failed to load texture " << fileName.c_str() << endl;
			return 0;//This is bad
		}
		SDL_FreeSurface(s);

		return (uint32_t)img;
	}

	// Texture from an image resource
	uint32_t ColorTexture(std::string fileName){
		const uint32_t texFormat = SDL_PIXELFORMAT_RGBA8888;

		return FromImage(fileName, texFormat);
	}

	GLuint NormalTexture(std::string fileName){
		const uint32_t normalFmt = SDL_PIXELFORMAT_RGB24;

		return FromImage(fileName, normalFmt);
	}

	GLuint HeightTexture(std::string fileName){
		const uint32_t normalFmt = SDL_PIXELFORMAT_INDEX8;

		return FromImage(fileName, normalFmt);
	}

	//GLuint HeightMap(std::string fileName){
	//	// must be greyscale
	//	const uint32_t heightFmt = SDL_PIXELFORMAT_
	//}

	GLuint CubeMap(std::string faces[6]){
		GLuint cubeTex(0);

		glGenTextures(1, &cubeTex);
		if (!cubeTex){
			cout << "Unable to create texture" << endl;
			return 0;
		}
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		for (int i = 0; i < 6; i++){
			SDL_Surface * s = getSurfaceFromImage(/*"../Resources/Environments/" + */string("spaceCat.jpg"), SDL_PIXELFORMAT_RGB24);
			if (!s)
				continue;

			uint32_t internalFormat = s->format->BytesPerPixel == 3 ? GL_RGB : GL_RGBA; // so sick of this
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, s->w, s->h, 0, internalFormat, GL_UNSIGNED_BYTE, s->pixels);
			SDL_FreeSurface(s);
		}

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		return cubeTex;
	}
}