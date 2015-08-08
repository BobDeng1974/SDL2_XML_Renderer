#pragma once

#include "GL_Includes.h"
#include "Material.h"

#include <vec4.hpp>
#include <mat4x4.hpp>

// Stores what gets drawn, including MV transform
// and various handles. Also owns a material. 

class Geometry
{
	friend class Scene;
public:
    // Constructors
	Geometry(std::string srcFile = "");
	Geometry(GLuint VAO, GLuint nIdx, mat4& MV);
    
    // Public Modifiers
    void identity();
    void leftMultM(glm::mat4& lhs);
    void setMaterial(const Material& M);
    void setVAO(GLuint VAO);
    void setNumIndices(GLuint nIndices);

    // Public Accessors
	GLuint getVAO();
	GLuint getNumIdx();
	vec4 getColor();
	mat4 getMV();
	Material getMaterial();
	std::string GetSrcFile();
	
    // Static shader handle access
	static GLint getMHandle();
	static GLint getNHandle();
	static GLint getMatIdxHandle();

	inline bool operator==(const Geometry& other){
		return m_SrcFileName == other.m_SrcFileName;
	}
	inline bool operator<(const Geometry& other){
		return m_SrcFileName < other.m_SrcFileName;
	}

	// Handle modifications
	static void setMHandle(GLint mh);
	static void setNHandle(GLint nh);
	static void setMatIdxHandle(GLint mih);

	void Draw();
    
private:
    // Texture, MV locations, index count, MV, Material
	GLuint m_uVAO;
	GLuint m_nIdx;
	mat4 m_m4M;
	Material m_Material;
	std::string m_SrcFileName;
    
    // Static shader handles
	static GLint s_MHandle;
	static GLint s_NHandle;
	static GLint s_MatIdxHandle;
};