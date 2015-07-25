#pragma once

#include "GL_Includes.h"
#include <vec3.hpp>

class Light{
    friend class Scene;
public:
	enum class Type : int{
		DIRECTIONAL,
		POINT,
		AMBIENT,
		NIL
	};
    // Constructors
	Light();
	Light(Type, glm::vec3, glm::vec3, glm::vec3);
    
    // Public accessors
	Type getType();
	vec3 getPos();
	vec3 getDir();
	vec3 getIntensity();
    
    // These are static for lights because of how
    // they're represented in the shader
    GLint GetTypeHandle();
    GLint GetPosOrHalfHandle();
    GLint GetDirOrAttenHandle();
    GLint GetIntensityHandle();
    
private:
    // Depending on type, these vectors can mean different things
	Type m_Type;
	vec3 m_v3PosOrHalf;
	vec3 m_v3DirOrAtten;
	vec3 m_Intensity;
    
    // Static shader handles
    GLint m_TypeHandle;
    GLint m_PosOrHalfHandle;
    GLint m_DirOrAttenHandle;
    GLint m_IntensityHandle;
    
protected:
    void SetTypeHandle(GLint h);
    void SetPosOrHalfHandle(GLint h);
    void SetDirOrAttenHandle(GLint h);
    void SetIntensityHandle(GLint h);
};