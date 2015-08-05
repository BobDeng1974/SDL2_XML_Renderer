#include "Geometry.h"
#include <gtx/transform.hpp>

using namespace std;

/*static*/ GLint Geometry::s_MHandle(-1);
/*static*/ GLint Geometry::s_NHandle(-1);
///*static*/ GLint Geometry::s_TexMapHandle(-1);
///*static*/ GLint Geometry::s_NrmMapHandle(-1);

Geometry::Geometry() :
//m_Tex(0),
//m_Nrm(0),
m_uVAO(0),
m_nIdx(0),
m_m4M(1)
{}

Geometry::Geometry(GLuint VAO, GLuint nIdx, mat4& MV) :
//m_Tex(tex),
//m_Nrm(nrm),
m_uVAO(VAO),
m_nIdx(nIdx),
m_m4M(MV)
{}

void Geometry::setVAO(GLuint VAO){
	m_uVAO = VAO;
}

void Geometry::setNumIndices(GLuint nIndices){
	m_nIdx = nIndices;
}

//void Geometry::setTexMap(GLuint tex){
//	m_Tex = tex;
//}
//
//void Geometry::setNrmMap(GLuint nrm){
//	m_Nrm = nrm;
//}

GLuint Geometry::getVAO(){
	return m_uVAO;
}

GLuint Geometry::getNumIdx(){
	return m_nIdx;
}

//GLuint Geometry::GetTexMap(){
//	return m_Tex;
//}
//
//GLuint Geometry::GetNrmMap(){
//	return m_Nrm;
//}

mat4 Geometry::getMV(){
	return m_m4M;
}

Material Geometry::getMaterial(){
	return m_Material;
}

void Geometry::identity(){
	m_m4M = mat4(1);
}

void Geometry::leftMultM(mat4& lhs){
	m_m4M = lhs * m_m4M;
}

void Geometry::setMaterial(const Material& M){
	m_Material = Material(M);
}

/*static*/ void Geometry::setMHandle(GLint mvh){
	s_MHandle = mvh;
}

/*static*/ void Geometry::setNHandle(GLint nh){
	s_NHandle = nh;
}

///*static*/ void Geometry::setNrmMapHandle(GLint nh){
//	s_NrmMapHandle = nh;
//}
//
///*static*/ void Geometry::setTexMapHandle(GLint texh){
//	s_TexMapHandle = texh;
//}

/*static*/ GLint Geometry::getMHandle(){
	return s_MHandle;
}

/*static*/ GLint Geometry::getNHandle(){
	return s_NHandle;
}


///*static*/ GLint Geometry::getNrmMapHandle(){
//	return s_NrmMapHandle;
//}
//
///*static*/ GLint Geometry::getTexMapHandle(){
//	return s_TexMapHandle;
//}

void Geometry::Draw(){
	// Lambda to bind a texture
	auto bindTex = [](GLint t, int glTexNum){
		if (t >= 0){
			glActiveTexture(glTexNum);
			glBindTexture(GL_TEXTURE_2D, t);
		}
	};

	// Upload world MV, N matrices
	mat3 N(m_m4M);// glm::inverse(glm::transpose(m_m4M)));
	glUniformMatrix4fv(Geometry::getMHandle(), 1, GL_FALSE, (const GLfloat *)&m_m4M);
	glUniformMatrix3fv(Geometry::getNHandle(), 1, GL_FALSE, (const GLfloat *)&N);

	// Gotta get geom's material properties and upload them as uniforms (every call?)
	float shininess = m_Material.getShininess();
	float reflectivity = m_Material.GetReflectivity();
	vec4 diff = m_Material.getDiff();
	vec4 spec = m_Material.getSpec();

	glUniform1f(Material::getShinyHandle(), shininess);
	glUniform1f(Material::GetReflectHandle(), reflectivity);
	glUniform4f(Material::getDiffHandle(), diff[0], diff[1], diff[2], diff[3]);
	glUniform4f(Material::getSpecHandle(), spec[0], spec[1], spec[2], spec[3]);

	// Bind texture and normal map, if they exist
	bindTex(m_Material.GetTexMap(), GL_TEXTURE0); // This assumes the uniforms are set,
	bindTex(m_Material.GetNrmMap(), GL_TEXTURE1); // which is bad

	// Bind VAO, draw
	glBindVertexArray(m_uVAO);
	glDrawElements(GL_TRIANGLES, m_nIdx, GL_UNSIGNED_INT, NULL);
}