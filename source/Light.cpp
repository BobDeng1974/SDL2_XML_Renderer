#include "Light.h"
#include <glm.hpp>

using glm::vec3;
using glm::normalize;

Light::Light()
	:m_Type(Type::NIL)
{}

Light::~Light(){}

Light::Light(Type t, vec3 pos, vec3 dir, vec3 intensity)
	: m_Type(t),
	m_Pos(pos),
	m_Dir(normalize(dir)),
	m_Intensity(intensity)
{}