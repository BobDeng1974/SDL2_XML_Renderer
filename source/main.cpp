#include "Scene.h"
#include <MouseManager.h>
#include "KeyboardManager.h"
#include "Textures.h"

#include <gtx/quaternion.hpp>
#include <gtx/transform.hpp>

#include <iostream>
using namespace std;

const int WIDTH = 600;
const int HEIGHT = 600;
const unsigned int FPS = 30;

#include <SDL.h>
#include <SDL_image.h>

Scene g_Scene;
Camera g_Camera;
Shader g_Shader;
SDL_GLContext g_Context;
SDL_Window * g_Window = nullptr;

const int glMajor(3), glMinor(0);

bool initGL(){
	//Init SDL Video
	if (SDL_Init(SDL_INIT_VIDEO) < 0){
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	//Init SDL image
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags)){
		printf("SDL_Image could not initialize! SDL_Image Error: %s\n", IMG_GetError());
		return false;
	}

	//Init SDL+OpenGL Context
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, glMajor);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, glMinor);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	//Create Window
	g_Window = SDL_CreateWindow("3D Test",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		WIDTH,HEIGHT, 
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (g_Window == NULL){
		printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	//Create Context from Window
	g_Context = SDL_GL_CreateContext(g_Window);
	if (g_Context == NULL){
		printf("OpenGL context was not created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	//Initialize GLEW
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK){
		printf("Error initializing GLEW! %s\n", glewGetErrorString(glewError));
		return false;
	}

	//Use Vsync
	if (SDL_GL_SetSwapInterval(1) < 0){
		printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
	}

	//OpenGL settings
	glClearColor(1, .4, .4, 1);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	glEnable(GL_MULTISAMPLE_ARB);

	//These are legacy calls valid in OpenGL 3.0 only
#ifndef __APPLE__
	glAlphaFunc(GL_GREATER, 0.9);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#endif

	//For debugging
	glLineWidth(8.f);

	return true;
}

bool HandleEvent(SDL_Event& e){
	auto keyCode = [&e](){
		return (int)e.key.keysym.sym;
	};

	while (SDL_PollEvent(&e))
		switch (e.type)
	{
		case SDL_KEYUP:
		case SDL_KEYDOWN:
			if (keyCode() == SDLK_ESCAPE)
				return true;
			else if (keyCode() == SDLK_r)
				g_Camera.Reset();
			else if (!e.key.repeat)
				KeyboardManager::HandleKey(keyCode());
			break;
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			switch (e.button.button){
			case SDL_BUTTON_LEFT:
				//eReg->toggleLMB();
				break;
			case SDL_BUTTON_RIGHT:
				//eReg->toggleRMB();
				break;
			}
			break;
		case SDL_MOUSEMOTION:{

			// half measures
			const int hW = WIDTH / 2, hH = HEIGHT / 2;
			const int thr = 100;

			// Press alt to actually move the mouse
			bool noMove = KeyboardManager::GetKeyState(SDLK_LALT);

			if (noMove)
				break;

			bool tX = (e.motion.x < thr) || (WIDTH - e.motion.x < thr);
			bool tY = (e.motion.y < thr) || (HEIGHT - e.motion.y < thr);
			if ((tX || tY)){
				MouseManager::Reset(hW, hH);
				SDL_WarpMouseInWindow(g_Window, hW, hH);
			}
			else
				g_Camera.rotate(MouseManager::HandleMouseMove(e.motion.x, e.motion.y));
		}
		default:
			break;
	}

	vec3 v(0);
	float T = KeyboardManager::GetKeyState(SDLK_LSHIFT) ? 3.f : 1.5f;
	if (KeyboardManager::GetKeyState('w'))
		v.z -= T;
	if (KeyboardManager::GetKeyState('a'))
		v.x -= T;
	if (KeyboardManager::GetKeyState('s'))
		v.z += T;
	if (KeyboardManager::GetKeyState('d'))
		v.x += T;
	g_Camera.translate(v);

	return false;
}

void TearDown(){
	SDL_DestroyWindow(g_Window);
	g_Window = nullptr;
	IMG_Quit();
	SDL_Quit();
}

void Render(){
	auto sBind = g_Shader.ScopeBind();
	
	mat4 proj = g_Camera.getProj();
	mat4 MV_e = g_Camera.getTransform();
	//vec3 eye = g_Camera.getPos();// proj * vec4(0, 0, 0, 1);// g_Camera.getView();
	glUniformMatrix4fv(g_Camera.getProjHandle(), 1, GL_FALSE, (const GLfloat *)&proj);
	glUniformMatrix4fv(g_Shader["MV_e"], 1, GL_FALSE, (const GLfloat *)&MV_e);
	//glUniform3f(g_Shader["u_Eye"], eye[0], eye[1], eye[2]);
	g_Scene.Draw();
}

int main(int argc, char ** argv){
	if (!initGL(/*argc, argv*/))
		return EXIT_FAILURE;
	// This inits shaders, loads geom, etc.
	g_Scene = Scene("../Resources/Scenes/TestScene.xml", g_Shader, g_Camera);

	bool quit(false);
	SDL_Event e;

	while (!quit){
		//Clear OpenGL Buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Render();

		//Swap window buffers
		SDL_GL_SwapWindow(g_Window);

		quit = HandleEvent(e);
	}

	TearDown();

	return EXIT_SUCCESS;
}