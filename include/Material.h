#pragma once

#include "GL_Includes.h"
#include <vec4.hpp>
#include <string>

// Stores colors and phong factor for a geometry object
// knows its own handles, which are static, since we only need
// one at a time

class Material{
    friend class Scene;
public:
    // Constructors
	Material(){}
    Material(float sh, float r, const vec4& d, const vec4& sp);
    
    void SetTexMap(GLuint t);
    void SetNrmMap(GLuint n);
    void SetTexMapSrc(std::string src);
    void SetNrmMapSrc(std::string src);
    
    // Public accessors
	float getShininess();
    float GetReflectivity();
	vec4 getDiff();
	vec4 getSpec();
    std::string GetTexMapFile();
    std::string GetNrmMapFile();
    GLuint GetTexMap();
    GLuint GetNrmMap();
    
    // Public shader handle access
    static GLint getShinyHandle();
	static GLint GetReflectHandle();
    static GLint getDiffHandle();
    static GLint getSpecHandle();
    static GLint GetTexMapHandle();
    static GLint GetNrmMapHandle();

protected:
    // Proteced handle access
    static void setShinyHandle(GLint handle);
	static void SetReflectHandle(GLint handle);
    static void setDiffHandle(GLint handle);
    static void setSpecHandle(GLint handle);
    static void SetTexMapHandle(GLint handle);
    static void SetNrmMapHandle(GLint handle);

private:
    // power factor, diffuse color, spec color
    float m_fShininess;
    float m_fReflectivity;
    vec4 m_v4Diff;
    vec4 m_v4Spec;
    
    // Texture Handles
    GLuint m_TexMap;
    GLuint m_NrmMap;
    std::string m_TexMapSrc;
    std::string m_NrmMapSrc;
    
    // Static shader handles
    static GLint s_ShinyHandle;
	static GLint s_ReflectHandle;
    static GLint s_DiffHandle;
    static GLint s_SpecHandle;
    static GLint s_TexMapHandle;
    static GLint s_NrmMapHandle;
};