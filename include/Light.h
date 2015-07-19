#pragma once

#include <vec3.hpp>
using glm::vec3;

class Light{
public:
	enum class Type : int{
		DIRECTIONAL,
		POINT,
		AMBIENT,
		NIL
	};
	Light();
	Light(Type, glm::vec3, glm::vec3, glm::vec3);
	~Light();
	Type getType();
	vec3 getPos();
	vec3 getDir();
	vec3 getIntensity();
private:
	Type m_Type;
	vec3 m_v3PosOrHalf;
	vec3 m_v3DirOrAtten;
	vec3 m_Intensity;
};