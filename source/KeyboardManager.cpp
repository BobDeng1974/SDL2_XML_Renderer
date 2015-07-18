#include "KeyboardManager.h"

#include <vec3.hpp>
using glm::vec3;

KeyboardManager KeyboardManager::s_Inst;

KeyboardManager::KeyboardManager() :
vKeyState(512, false)
{}

/*static*/ void KeyboardManager::Init(){
	// Wat do?
}

#include <iostream>
using namespace std;
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