#include <iostream>
#include <vector>
using namespace std;

#include <SDL_image.h>

#include "GL_Includes.h"


//#include <Textures.h>
//#include <Util.h>

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

	for (uint32_t y = 0; y<DIM; y++){
		for (uint32_t x = 0; x<DIM; x++){
			if (x<th || x>DIM - th || y<th || y>DIM - th)
				PXA[y*DIM + x] = (invert ? white : black);
			else
				PXA[y*DIM + x] = (invert ? black : white);
		}
	}

	return InitTexture(PXA.data(), DIM, DIM);
}