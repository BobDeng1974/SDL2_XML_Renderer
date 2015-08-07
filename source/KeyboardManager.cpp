#include "KeyboardManager.h"

#include <vec3.hpp>

#include <SDL.h>

KeyboardManager KeyboardManager::s_Inst;

KeyboardManager::KeyboardManager() :
vKeyState(512, false)
{}

/*static*/ void KeyboardManager::Init(){
	// Wat do?
}

#include <iostream>
#include <string>
#include <sstream>
using namespace std;

// Modal state where text input is handled
string KeyboardManager::InputKeys(){
	SDL_Event e;
	bool quit = false;
	string ret;
	stringstream sstr;
	SDL_StartTextInput();
	while (!quit){
		while (SDL_PollEvent(&e)){
			if (e.type == SDL_TEXTINPUT){
				cout << e.text.text;
				sstr << string(e.text.text);
			}
			else if (e.type == SDL_KEYDOWN){
				unsigned char key(e.key.keysym.sym);
				switch (key){
				case SDLK_RETURN:
					ret = sstr.str();
					cout << endl;
					quit = true;
					break;
				case SDLK_BACKSPACE:
					ret = sstr.str();
					if (ret.size()){
						ret.resize(ret.size() - 1);
						sstr.str(ret);
						cout << "\b";
					}
					break;
				case SDLK_SPACE:
					cout << " ";
					sstr << " ";
				}
			} 
		}
	}
	SDL_StopTextInput();
	return ret;
}

//public: 
// This should return something useful
/*static*/ bool KeyboardManager::HandleKey(unsigned char key){
	//   const float T(5.f);
	//   vec3 v(0);
	//bool verbose(true);

	//if (verbose){
	//	if (s_Inst.vKeyState[key])
	//		cout << "Clearing keystate for " << key << endl;
	//	else
	//		cout << "Setting keystate for " << key << endl;
	//}

	bool n = !s_Inst.vKeyState[key];;
	s_Inst.vKeyState[key] = n;
	return n;


	//   if (GetKeyState('w'))
	//       v.z -= T;
	//   if (GetKeyState('a'))
	//       v.x -= T;
	//   if (GetKeyState('s'))
	//       v.z += T;
	//   if (GetKeyState('d'))
	//       v.x += T;
	//   
	//return v;
}

/*static*/ const KeyboardManager * KeyboardManager::Instance(){
	return (const KeyboardManager *)&s_Inst;
}

/*static*/ bool KeyboardManager::GetKeyState(unsigned char key){
	return s_Inst.vKeyState[key];
}