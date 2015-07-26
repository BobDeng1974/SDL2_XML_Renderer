#include "Geometry.h"

using namespace std;

/*static*/ GLint Geometry::s_MVHandle(-1);
/*static*/ GLint Geometry::s_NrmHandle(-1);
/*static*/ GLint Geometry::s_TexHandle(-1);

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

void Geometry::setTex(GLuint tex){
	m_Tex = tex;
}

void Geometry::setNrm(GLuint nrm){
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

/*static*/ void Geometry::setNormalHandle(GLint nh){
	s_NrmHandle = nh;
}

/*static*/ void Geometry::setTexHandle(GLint texh){
	s_TexHandle = texh;
}

/*static*/ GLint Geometry::getMVHandle(){
	return s_MVHandle;
}

/*static*/ GLint Geometry::getNormalHandle(){
	return s_NrmHandle;
}

/*static*/ GLint Geometry::getTexHandle(){
	return s_TexHandle;
}