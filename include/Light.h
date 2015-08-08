#pragma once

#include "Geometry.h"
#include <vec3.hpp>

class Light{
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
	Type getType() const;
	vec3 getPos() const;
	vec3 getDir() const;
	vec3 getIntensity() const;
    
    // These aren't static for lights because of how
    // they're represented in the shader
    GLint GetTypeHandle() const;
	GLint GetPosOrHalfHandle() const;
	GLint GetDirOrAttenHandle() const;
	GLint GetIntensityHandle() const;
	Geometry GetGeometry() const;

	void SetTypeHandle(GLint h);
	void SetPosOrHalfHandle(GLint h);
	void SetDirOrAttenHandle(GLint h);
	void SetIntensityHandle(GLint h);
	void SetGeometry(const Geometry& g);
    
private:
    // Depending on type, these vectors can mean different things
	Type m_Type;
	vec3 m_v3PosOrHalf;
	vec3 m_v3DirOrAtten;
	vec3 m_Intensity;
	Geometry m_Geometry;
    
    // Static shader handles
    GLint m_TypeHandle;
    GLint m_PosOrHalfHandle;
    GLint m_DirOrAttenHandle;
    GLint m_IntensityHandle;
};