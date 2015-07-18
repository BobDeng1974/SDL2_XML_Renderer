#include "Geometry.h"

using glm::mat4;
using glm::vec4;

using namespace std;

GLint Geometry::s_MVHandle(-1);

Geometry::Geometry()
	: m_uVAO(0),
	m_nIdx(0),
	m_v4Color(1),
	m_m4MV(1)
{}

Geometry::~Geometry(){}

Geometry::Geometry(GLuint VAO, GLuint nIdx, glm::vec4& C, glm::mat4& MV)
	: m_uVAO(VAO),
	m_nIdx(nIdx),
	m_v4Color(C),
	m_m4MV(MV)
{}

/*static*/ void Geometry::setMVHandle(GLint mvh){
	s_MVHandle = mvh;
}

void Geometry::setVAO(GLuint VAO){
	m_uVAO = VAO;
}