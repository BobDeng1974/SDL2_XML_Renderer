#include "Geometry.h"
#include <gtx/transform.hpp>

using namespace std;

/*static*/ GLint Geometry::s_MVHandle(-1);
/*static*/ GLint Geometry::s_NHandle(-1);
/*static*/ GLint Geometry::s_TexMapHandle(-1);
/*static*/ GLint Geometry::s_NrmMapHandle(-1);

Geometry::Geometry() :
m_Tex(0),
m_Nrm(0),
m_uVAO(0),
m_nIdx(0),
m_m4MV(1)
{}

Geometry::Geometry(GLuint tex, GLuint nrm, GLuint VAO, GLuint nIdx, mat4& MV) :
m_Tex(tex),
m_Nrm(nrm),
m_uVAO(VAO),
m_nIdx(nIdx),
m_m4MV(MV)
{}

void Geometry::setVAO(GLuint VAO){
	m_uVAO = VAO;
}

void Geometry::setNumIndices(GLuint nIndices){
	m_nIdx = nIndices;
}

void Geometry::setTexMap(GLuint tex){
	m_Tex = tex;
}

void Geometry::setNrmMap(GLuint nrm){
	m_Nrm = nrm;
}

GLuint Geometry::getVAO(){
	return m_uVAO;
}

GLuint Geometry::getNumIdx(){
	return m_nIdx;
}

GLuint Geometry::GetTexMap(){
	return m_Tex;
}

GLuint Geometry::GetNrmMap(){
	return m_Nrm;
}

mat4 Geometry::getMV(){
	return m_m4MV;
}

Material Geometry::getMaterial(){
	return m_Material;
}

void Geometry::identity(){
	m_m4MV = mat4(1);
}

void Geometry::leftMultMV(mat4& lhs){
	m_m4MV = lhs * m_m4MV;
}

void Geometry::setMaterial(const Material& M){
	m_Material = M;
}

/*static*/ void Geometry::setMVHandle(GLint mvh){
	s_MVHandle = mvh;
}

/*static*/ void Geometry::setNHandle(GLint nh){
	s_NHandle = nh;
}

/*static*/ void Geometry::setNrmMapHandle(GLint nh){
	s_NrmMapHandle = nh;
}

/*static*/ void Geometry::setTexMapHandle(GLint texh){
	s_TexMapHandle = texh;
}

/*static*/ GLint Geometry::getMVHandle(){
	return s_MVHandle;
}

/*static*/ GLint Geometry::getNHandle(){
	return s_NHandle;
}


/*static*/ GLint Geometry::getNrmMapHandle(){
	return s_NrmMapHandle;
}

/*static*/ GLint Geometry::getTexMapHandle(){
	return s_TexMapHandle;
}

void Geometry::Draw(){
	// Lambda to bind a texture
	auto bindTex = [](GLint t, int glTexNum){
		if (t >= 0){
			glActiveTexture(glTexNum);
			glBindTexture(GL_TEXTURE_2D, t);
		}
	};

	// Upload world MV, N matrices
	mat3 N(glm::inverse(m_m4MV));
	glUniformMatrix4fv(Geometry::getMVHandle(), 1, GL_FALSE, (const GLfloat *)&m_m4MV);
	glUniformMatrix3fv(Geometry::getNHandle(), 1, GL_FALSE, (const GLfloat *)&N);

	// Gotta get geom's material properties and upload them as uniforms (every call?)
	vec4 diff = m_Material.getDiff();
	vec4 spec = m_Material.getSpec();
	float shininess = m_Material.getShininess();

	glUniform1f(Material::getShinyHandle(), shininess);
	glUniform4f(Material::getDiffHandle(), diff[0], diff[1], diff[2], diff[3]);
	glUniform4f(Material::getSpecHandle(), spec[0], spec[1], spec[2], spec[3]);

	// Bind texture and normal map, if they exist
	bindTex(m_Tex, GL_TEXTURE0);
	bindTex(m_Nrm, GL_TEXTURE1);

	// Bind VAO, draw
	glBindVertexArray(m_uVAO);
	glDrawElements(GL_TRIANGLES, m_nIdx, GL_UNSIGNED_INT, NULL);
}