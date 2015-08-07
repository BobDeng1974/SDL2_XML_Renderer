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

#include <SDL.h>
#include <SDL_image.h>

Scene g_Scene;
Camera g_Camera;
Shader g_Shader;
SDL_GLContext g_Context;
SDL_Window * g_Window = nullptr;

// TODO
// add some keyboard input to allow for
// changing things around without restarting
// it would be nice to move the camera into different
// objects (even lights) and move them to see the effect

const int glMajor(3), glMinor(0);

// Initialize SDL2, OpenGL, and GLEW
bool initGL(){
	//Init SDL Video
	if (SDL_Init(SDL_INIT_VIDEO) < 0){
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	//Init SDL image
	int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
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
		WIDTH, HEIGHT,
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
#include <sstream>
// Event Handling switch
bool HandleEvent(SDL_Event& e){
	auto keyCode = [&e](){
		return (int)e.key.keysym.sym;
	};

	while (SDL_PollEvent(&e))
		switch (e.type)
	{
		// 'r' and Escape are special
		case SDL_KEYUP:
		case SDL_KEYDOWN:
			if (keyCode() == SDLK_ESCAPE)
				return true; // Quit
			else if (keyCode() == SDLK_r)
				g_Camera.Reset(); // Reset camera
			// Use R shift for user input, I'd like to use it to modify uniforms
			else if (keyCode() == SDLK_RSHIFT && !e.key.repeat){
				string input = KeyboardManager::InputKeys();
				const string d1 = " = ";
				size_t pos = input.find(d1), pos2(0);
				if (pos != string::npos){
					string U = input.substr(0, pos - 1);
					string V = input.substr(U.size() + d1.size() + 1, input.length());
					vector<float> inFloats;
					const string d2 = ",";
					pos = 0;
					for (pos = 0; pos != string::npos; V = V.substr(0, pos2-d2.size())){
						size_t pos2 = V.find(d2);
							stringstream sstr;
						float val(0);
						sstr.str(V.substr(pos, pos2 == string::npos ? V.length() : pos2));
						if (sstr >> val)
							inFloats.push_back(val);
						;
					}

					auto sBind = g_Shader.ScopeBind();
					GLint handle = g_Shader[U];
					//if (handle >= 0)
					{
						const size_t size = inFloats.size();
						if (size == 1)
							glUniform1fv(handle, size, inFloats.data());
						if (size == 2)
							glUniform2fv(handle, size, inFloats.data());
						if (size == 3)
							glUniform3fv(handle, size, inFloats.data());
						if (size == 4)
							glUniform4fv(handle, size, inFloats.data());
					}
				}
				return false;
			}
			else if (!e.key.repeat)
				KeyboardManager::HandleKey(keyCode());
			break;
			// What do buttons do?
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

			// Press alt to actually move the mouse (does this work?)
			bool noMove = KeyboardManager::GetKeyState(SDLK_LALT);
			if (noMove)
				break;

			// Move the mouse to the center of the screen if it gets too close to the borders
			bool tX = (e.motion.x < thr) || (WIDTH - e.motion.x < thr);
			bool tY = (e.motion.y < thr) || (HEIGHT - e.motion.y < thr);
			if ((tX || tY)){
				MouseManager::Reset(hW, hH);
				SDL_WarpMouseInWindow(g_Window, hW, hH);
			}
			else // Mouse rotates camera
				g_Camera.Rotate(MouseManager::HandleMouseMove(e.motion.x, e.motion.y));
		}
		default:
			break;
	}

	// Compute translation given current keyboard state
	vec3 v(0);
	float T = KeyboardManager::GetKeyState(SDLK_LSHIFT) ? 1.f : .5f;
	if (KeyboardManager::GetKeyState('w'))
		v.z -= T;
	if (KeyboardManager::GetKeyState('a'))
		v.x -= T;
	if (KeyboardManager::GetKeyState('s'))
		v.z += T;
	if (KeyboardManager::GetKeyState('d'))
		v.x += T;
	g_Camera.Translate(v);

	return false;
}

// Clean up your act
void TearDown(){
	SDL_DestroyWindow(g_Window);
	g_Window = nullptr;
	IMG_Quit();
	SDL_Quit();
}

// Draw
void Render(){
	auto sBind = g_Shader.ScopeBind();

	// Get projection and eye space transform, upload, draw
	mat4 PV = g_Camera.GetMat();
	vec3 camPos = g_Camera.GetPos();
	//mat4 proj = g_Camera.GetProj();
	//mat4 C = g_Camera.GetTransform();
	glUniformMatrix4fv(Camera::GetProjHandle(), 1, GL_FALSE, (const GLfloat *)&PV);
	glUniform3f(Camera::GetPosHandle(), camPos[0], camPos[1], camPos[2]);
	//	glUniformMatrix4fv(Camera::GetCHandle(), 1, GL_FALSE, (const GLfloat *)&C);
	g_Scene.Draw();
}

// Main
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

ostream& operator<<(ostream& os, const vec2& vec){
	os << "{" << vec.x << ", " << vec.y << "}";
	return os;
}

ostream& operator<<(ostream& os, const vec3& vec){
	os << "{" << vec.x << ", " << vec.y << ", " << vec.z << "}";
	return os;
}

ostream& operator<<(ostream& os, const vec4& vec){
	os << "{" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << "}";
	return os;
}

ostream& operator<<(ostream& os, const mat4& mat){
	mat4 transMat = glm::transpose(mat);
	os << "{\n" << transMat[0] << ",\n" << transMat[1] << ",\n" << transMat[2] << ",\n" << transMat[3] << ",\n}";
	return os;
}

ostream& operator<<(ostream& os, const fquat& quat){
	os << "{" << quat.w << ", " << quat.x << ", " << quat.y << ", " << quat.z << "}";
	return os;
}