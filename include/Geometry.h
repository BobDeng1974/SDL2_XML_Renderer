#pragma once

#include "GL_Includes.h"

#include <vec4.hpp>
#include <mat4x4.hpp>

class Geometry
{
	friend class Scene;
public:
	Geometry();
	~Geometry();
	Geometry(GLuint VAO, GLuint nIdx, glm::vec4& C, glm::mat4& MV);
	inline GLuint getVAO(){ return m_uVAO; }
	inline GLuint getNumIdx(){ return m_nIdx; }
	static inline GLint getMVHandle(){ return s_MVHandle; }
	inline glm::vec4 getColor(){ return m_v4Color; }
	inline glm::vec4 * getColorPtr(){ return &m_v4Color; }
	inline glm::mat4 getMV(){ return m_m4MV; }
	inline glm::mat4 * getMVPtr(){ return &m_m4MV; }
	inline void identity(){ m_m4MV = glm::mat4(1); }
	inline void leftMultMV(glm::mat4& lhs){ m_m4MV = lhs * m_m4MV; }
	inline void setColor(glm::vec4& C){ m_v4Color = C; }
	void setVAO(GLuint VAO);
	inline void setNumIndices(GLuint nIndices) { m_nIdx = nIndices; }
private:
	GLuint m_uVAO;
	GLuint m_nIdx;
	glm::vec4 m_v4Color;
	glm::mat4 m_m4MV;
protected:
	static GLint s_MVHandle;
	static void setMVHandle(GLint mvh);
};