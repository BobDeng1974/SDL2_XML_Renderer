#pragma once

#include "GL_Includes.h"
#include <vec4.hpp>

class Material{
    friend class Scene;
public:
    // Constructors
	Material(){}
	Material(float sh, const vec4& d, const vec4& sp);
    
    // Public accessors
	float getShininess();
	vec4 getDiff();
	vec4 getSpec();
    
    // Public shader handle access
    static GLint getShinyHandle();
    static GLint getDiffHandle();
    static GLint getSpecHandle();

protected:
    // Proteced handle access
    static void setShinyHandle(GLint handle);
    static void setDiffHandle(GLint handle);
    static void setSpecHandle(GLint handle);

private:
    // power factor, diffuse color, spec color
    float m_fShininess;
    vec4 m_v4Diff;
    vec4 m_v4Spec;
    
    // Static shader handles
    static GLint s_ShinyHandle;
    static GLint s_DiffHandle;
    static GLint s_SpecHandle;
};