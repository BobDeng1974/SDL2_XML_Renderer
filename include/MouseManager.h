#pragma once

#include <fwd.hpp>

class MouseManager{
public:
	static void Init();
	static void HandleMouseBtn(int button, int state, int x, int y);
	static void Reset(int newX, int newY);
	static glm::fquat HandleMouseMove(int x, int y);
	static const MouseManager * Instance();
private:
	MouseManager();
	static MouseManager s_Inst;
	int button;
	int lX;
	int lY;
};