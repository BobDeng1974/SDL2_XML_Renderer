#pragma once

#include "GL_Includes.h"
#include "Material.h"

#include <vec4.hpp>
#include <mat4x4.hpp>

class Geometry
{
	friend class Scene;
public:
	Geometry();
	~Geometry();
	Geometry(GLuint tex, GLuint VAO, GLuint nIdx, glm::vec4& C, glm::mat4& MV);
	void setVAO(GLuint VAO);
	void setNumIndices(GLuint nIndices);
	void setTex(GLuint tex);
	GLuint getVAO();
	GLuint getNumIdx();
	GLuint getTex();
	vec4 getColor();
	mat4 getMV();
	Material getMaterial();

	const GLfloat * getColorPtr();
	const GLfloat * getMVPtr();

	void identity();
	void leftMultMV(glm::mat4& lhs);
	void setColor(glm::vec4& C);
	void setMaterial(const Material& M);
	
	static GLint getMVHandle();
	static GLint getNormalHandle();
	static GLint getTexHandle();
private:
	GLuint m_Tex;
	GLuint m_uVAO;
	GLuint m_nIdx;
	vec4 m_v4Color;
	mat4 m_m4MV;
	Material m_Material;
protected:
	static GLint s_MVHandle;
	static GLint s_NrmHandle;
	static GLint s_TexHandle;
	static void setTexHandle(GLint texh);
	static void setMVHandle(GLint mvh);
	static void setNormalHandle(GLint nh);
};