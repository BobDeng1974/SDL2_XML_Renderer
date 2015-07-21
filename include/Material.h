#pragma once

#include "GL_Includes.h"
#include <vec4.hpp>

class Material{
	float m_fShininess;
	vec4 m_v4Diff;
	vec4 m_v4Spec;

	static GLint s_ShinyHandle;
	static GLint s_DiffHandle;
	static GLint s_SpecHandle;
public:
	Material(){}
	Material(float sh, const vec4& d, const vec4& sp);
	float getShininess();
	vec4 getDiff();
	vec4 getSpec();

	static void setShinyHandle(GLint handle);
	static void setDiffHandle(GLint handle);
	static void setSpecHandle(GLint handle);

	static GLint getShinyHandle();
	static GLint getDiffHandle();
	static GLint getSpecHandle();
};