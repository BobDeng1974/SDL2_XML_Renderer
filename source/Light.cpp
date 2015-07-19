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
	m_v3PosOrHalf(pos),
	m_v3DirOrAtten(normalize(dir)),
	m_Intensity(intensity)
{}

Light::Type Light::getType(){
	return m_Type;
}

vec3 Light::getPos(){
	return m_v3PosOrHalf;
}

vec3 Light::getDir(){
	return m_v3DirOrAtten;
}

vec3 Light::getIntensity(){
	return m_Intensity;
}