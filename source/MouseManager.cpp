#include "MouseManager.h"

#include <iostream>
using namespace std;

#include <math.h>
#include <glm.hpp>
#include <gtc/quaternion.hpp>

using glm::fquat;

MouseManager MouseManager::s_Inst;

MouseManager::MouseManager() :
lX(-1), // a call to glutWarpPointer ensures the moues starts at (-1,-1)
lY(-1)  // so that the initial dX = dY = 0
{}

/*static*/ void MouseManager::Init(){
	// Wat do?
}

//public: 
//// This should return something useful
///*static*/ void MouseManager::HandleMouseBtn(int button, int state, int x, int y){
//	// What do?
//}
///*static*/ fquat MouseManager::HandleMouseMove_B(int x, int y){
//	return HandleMouseMove(x, y, true);
//}
//
///*static*/ fquat MouseManager::HandleMouseMove_P(int x, int y){
//	return HandleMouseMove(x, y, false);
//}

//private:
/*static*/ fquat MouseManager::HandleMouseMove(int x, int y){
    auto fn = [](float x){
        return 0.5 * x;
    };
    
	// What do?
    float dX(x - s_Inst.lX);
	float dY(y - s_Inst.lY);
    
    // The problem with this is that it lets the
    // camera twist in z, which becomes disorienting
    // how do I deal with it?
	fquat rX(cos(fn(dY/180.f)), sin(fn(dY/180.f)), 0, 0);
	fquat rY(cos(fn(dX/180.f)), 0, sin(fn(dX/180.f)), 0);
    fquat R = glm::normalize(rX*rY);
    
	s_Inst.lX = x;
	s_Inst.lY = y;
    
    return R;
}

/*static*/ const MouseManager * MouseManager::Instance() {
	return (const MouseManager *)&s_Inst;
}

/*static*/ void MouseManager::Reset(int newx, int newY){
	s_Inst.lX = newx;
	s_Inst.lY = newY;
}