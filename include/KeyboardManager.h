#pragma once

#include <vector>

#include <fwd.hpp>

// Singleton, manages current key state, pretty basic
class KeyboardManager{
public:
	static void Init();
	static std::string InputKeys();
    static bool HandleKey(unsigned char key);
	static const KeyboardManager * Instance();
    static bool GetKeyState(unsigned char key);
    
private:
	KeyboardManager();
	static KeyboardManager s_Inst;
	std::vector<bool> vKeyState;
};