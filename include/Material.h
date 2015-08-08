#pragma once

#include "GL_Includes.h"
#include <vec4.hpp>
#include <string>

// Stores colors and phong factor for a geometry object
// knows its own handles, which are static, since we only need
// one at a time

class Material{
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
    GLint getShinyHandle();
	GLint GetReflectHandle();
    GLint getDiffHandle();
    GLint getSpecHandle();
    static GLint GetTexMapHandle();
    static GLint GetNrmMapHandle();

    // Handle access
    void setShinyHandle(GLint handle);
	void SetReflectHandle(GLint handle);
    void setDiffHandle(GLint handle);
    void setSpecHandle(GLint handle);

	// These are global for all materials
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
    
	// Per instance shader handles
	GLint m_ShinyHandle;
	GLint m_ReflectHandle;
	GLint m_DiffHandle;
	GLint m_SpecHandle;

    // Static shader handles
    static GLint s_TexMapHandle;
    static GLint s_NrmMapHandle;
};