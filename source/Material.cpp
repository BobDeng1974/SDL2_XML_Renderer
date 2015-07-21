#include "Material.h"

#include <vec4.hpp>

/*static*/ GLint Material::s_ShinyHandle;
/*static*/ GLint Material::s_DiffHandle;
/*static*/ GLint Material::s_SpecHandle;

Material::Material(float sh, const vec4& d, const vec4& sp)	:
	m_fShininess(sh),
	m_v4Diff(d),
	m_v4Spec(sp)
{}

float Material::getShininess(){
	return m_fShininess;
}

vec4 Material::getDiff(){
	return m_v4Diff;
}

vec4 Material::getSpec(){
	return m_v4Spec;
}

/*static*/ void Material::setShinyHandle(GLint handle){
	s_ShinyHandle = handle;
}

/*static*/ void Material::setDiffHandle(GLint handle){
	s_DiffHandle = handle;
}

/*static*/ void Material::setSpecHandle(GLint handle){
	s_SpecHandle = handle;
}

/*static*/ GLint Material::getShinyHandle(){
	return s_ShinyHandle;
}

/*static*/ GLint Material::getDiffHandle(){
	return s_DiffHandle;
}

/*static*/ GLint Material::getSpecHandle(){
	return s_SpecHandle;
}