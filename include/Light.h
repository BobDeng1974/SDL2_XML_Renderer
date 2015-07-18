#pragma once

#include <vec3.hpp>

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
//private:
	Type m_Type;
	glm::vec3 m_Pos;
	glm::vec3 m_Dir;
	glm::vec3 m_Intensity;
};