#pragma once

#include "GL_Includes.h"

#include <fwd.hpp>
#include <mat4x4.hpp>
#include <vec3.hpp>
#include <gtx/quaternion.hpp>

class Camera
{
	friend class Scene;
public:
	enum Type {
		ORTHO,
		PERSP,
		NIL
	};
    // Constructors (default, ortho, persp)
	Camera();
	Camera(vec2 X, vec2 Y, vec2 Z);
	Camera(float fovy, float aspect, vec2 nf);
    
    // Functions to modify projection transform
	void ResetRot();
	void ResetPos();
	void Reset();
    void Translate(vec3 T);
    void Rotate(fquat Q);
    
    // Functions to access camera info
    vec3 getView();
    vec3 GetPos();
    fquat GetRot();
    mat4 GetMat();
	mat4 GetTransform();
    mat4 GetProj();
    
    // Access to static shader handles
    static GLint GetProjHandle();//{ return s_ProjHandle; }
    static GLint GetMVHandle();//{ return s_MVHandle; }
private:
    // Camera Type, position, rotation, projection
	Type m_Type;
    vec3 m_v3Pos;
    fquat m_qRot;
	mat4 m_m4Proj;
    
    // Static shader handles
	static GLint s_ProjHandle;
	static GLint s_MVHandle;
protected:
    // Only the Scene Contructor can set these
    static void SetProjHandle(GLint p);
    static void SetMVHandle(GLint mv);
};