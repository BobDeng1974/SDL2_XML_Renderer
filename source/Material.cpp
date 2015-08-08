#include "Material.h"

#include <vec4.hpp>

/*static*/ GLint Material::s_TexMapHandle;
/*static*/ GLint Material::s_NrmMapHandle;

Material::Material(float sh, float r, const vec4& d, const vec4& sp)	:
	m_fShininess(sh),
    m_fReflectivity(r),
	m_v4Diff(d),
	m_v4Spec(sp),
	m_ShinyHandle(-1),
	m_ReflectHandle(-1),
	m_DiffHandle(-1),
	m_SpecHandle(-1)
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

void Material::setShinyHandle(GLint handle){
	m_ShinyHandle = handle;
}

void Material::SetReflectHandle(GLint handle){
	m_ReflectHandle = handle;
}

void Material::setDiffHandle(GLint handle){
	m_DiffHandle = handle;
}

void Material::setSpecHandle(GLint handle){
	m_SpecHandle = handle;
}

/*static*/ void Material::SetTexMapHandle(GLint handle){
    s_TexMapHandle = handle;
}

/*static*/ void Material::SetNrmMapHandle(GLint handle){
    s_NrmMapHandle = handle;
}

GLint Material::getShinyHandle(){
	return m_ShinyHandle;
}

GLint Material::GetReflectHandle(){
	return m_ReflectHandle;
}

GLint Material::getDiffHandle(){
	return m_DiffHandle;
}

GLint Material::getSpecHandle(){
	return m_SpecHandle;
}

/*static*/ GLint Material::GetNrmMapHandle(){
    return s_NrmMapHandle;
}

/*static*/ GLint Material::GetTexMapHandle(){
    return s_TexMapHandle;
}