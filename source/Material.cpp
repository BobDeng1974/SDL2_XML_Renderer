#include "Material.h"

#include <vec4.hpp>

/*static*/ GLint Material::s_ShinyHandle;
/*static*/ GLint Material::s_ReflectHandle;
/*static*/ GLint Material::s_DiffHandle;
/*static*/ GLint Material::s_SpecHandle;
/*static*/ GLint Material::s_TexMapHandle;
/*static*/ GLint Material::s_NrmMapHandle;

Material::Material(float sh, float r, const vec4& d, const vec4& sp)	:
	m_fShininess(sh),
    m_fReflectivity(r),
	m_v4Diff(d),
	m_v4Spec(sp)
{}

float Material::getShininess(){
	return m_fShininess;
}

float Material::GetReflectivity(){
    return m_fReflectivity;
}

vec4 Material::getDiff(){
	return m_v4Diff;
}

vec4 Material::getSpec(){
	return m_v4Spec;
}

std::string Material::GetNrmMapFile(){
    return m_NrmMapSrc;
}

std::string Material::GetTexMapFile(){
    return m_TexMapSrc;
}

GLuint Material::GetNrmMap(){
	return m_NrmMap;
}

GLuint Material::GetTexMap(){
	return m_TexMap;
}

void Material::SetNrmMap(GLuint n){
    m_NrmMap = n;
}

void Material::SetTexMap(GLuint t){
    m_TexMap = t;
}

void Material::SetTexMapSrc(std::string src){
    m_TexMapSrc = src;
}

void Material::SetNrmMapSrc(std::string src){
    m_NrmMapSrc = src;
}

/*static*/ void Material::setShinyHandle(GLint handle){
	s_ShinyHandle = handle;
}

/*static*/ void Material::SetReflectHandle(GLint handle){
	s_ReflectHandle = handle;
}

/*static*/ void Material::setDiffHandle(GLint handle){
	s_DiffHandle = handle;
}

/*static*/ void Material::setSpecHandle(GLint handle){
	s_SpecHandle = handle;
}

/*static*/ void Material::SetTexMapHandle(GLint handle){
    s_TexMapHandle = handle;
}

/*static*/ void Material::SetNrmMapHandle(GLint handle){
    s_NrmMapHandle = handle;
}

/*static*/ GLint Material::getShinyHandle(){
	return s_ShinyHandle;
}

/*static*/ GLint Material::GetReflectHandle(){
	return s_ReflectHandle;
}

/*static*/ GLint Material::getDiffHandle(){
	return s_DiffHandle;
}

/*static*/ GLint Material::getSpecHandle(){
	return s_SpecHandle;
}

/*static*/ GLint Material::GetNrmMapHandle(){
    return s_NrmMapHandle;
}

/*static*/ GLint Material::GetTexMapHandle(){
    return s_TexMapHandle;
}