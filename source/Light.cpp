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

Light::Type Light::getType() const{
	return m_Type;
}

vec3 Light::getPos() const{
	return m_v3PosOrHalf;
}

vec3 Light::getDir() const{
	return m_v3DirOrAtten;
}

vec3 Light::getIntensity() const{
	return m_Intensity;
}

GLint Light::GetTypeHandle() const{
    return m_TypeHandle;
}

GLint Light::GetPosOrHalfHandle() const{
    return m_PosOrHalfHandle;
}

GLint Light::GetDirOrAttenHandle() const{
    return m_DirOrAttenHandle;
}

GLint Light::GetIntensityHandle() const{
    return m_IntensityHandle;
}

Geometry Light::GetGeometry() const{
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