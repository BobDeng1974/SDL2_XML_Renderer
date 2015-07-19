#include "Geometry.h"

using glm::mat4;
using glm::vec4;

using namespace std;

/*static*/ GLint Geometry::s_MVHandle(-1);
/*static*/ GLint Geometry::s_NrmHandle(-1);
/*static*/ GLint Geometry::s_TexHandle(-1);

Geometry::Geometry() :
m_Tex(0),
m_uVAO(0),
m_nIdx(0),
m_v4Color(1),
m_m4MV(1)
{}

Geometry::~Geometry(){}

Geometry::Geometry(GLuint tex, GLuint VAO, GLuint nIdx, glm::vec4& C, glm::mat4& MV) :
m_Tex(tex),
m_uVAO(VAO),
m_nIdx(nIdx),
m_v4Color(C),
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

GLuint Geometry::getVAO(){
	return m_uVAO;
}

GLuint Geometry::getNumIdx(){
	return m_nIdx;
}

GLuint Geometry::getTex(){
	return m_Tex;
}

vec4 Geometry::getColor(){
	return m_v4Color;
}

mat4 Geometry::getMV(){
	return m_m4MV;
}

Material Geometry::getMaterial(){
	return m_Material;
}

const GLfloat * Geometry::getColorPtr(){
	return (const GLfloat *)&m_v4Color;
}

const GLfloat * Geometry::getMVPtr(){
	return (const GLfloat *)&m_m4MV;
}

void Geometry::identity(){
	m_m4MV = mat4(1);
}

void Geometry::leftMultMV(mat4& lhs){
	m_m4MV = lhs * m_m4MV;
}

void Geometry::setColor(vec4& C){
	m_v4Color = C;
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