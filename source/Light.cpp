#include "Light.h"
#include <glm.hpp>

using glm::vec3;
using glm::normalize;

Light::Light()
	:m_Type(Type::NIL)
{}

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

GLint Light::GetTypeHandle(){
    return m_TypeHandle;
}

GLint Light::GetPosOrHalfHandle(){
    return m_PosOrHalfHandle;
}

GLint Light::GetDirOrAttenHandle(){
    return m_DirOrAttenHandle;
}

GLint Light::GetIntensityHandle(){
    return m_IntensityHandle;
}

Geometry Light::GetGeometry(){
	return m_Geometry;
}

void Light::SetTypeHandle(GLint h){
    m_TypeHandle = h;
}

void Light::SetPosOrHalfHandle(GLint h){
    m_PosOrHalfHandle = h;
}

void Light::SetDirOrAttenHandle(GLint h){
    m_DirOrAttenHandle = h;
}

void Light::SetIntensityHandle(GLint h){
    m_IntensityHandle = h;
}

void Light::SetGeometry(const Geometry& g){
	m_Geometry = Geometry(g);
}