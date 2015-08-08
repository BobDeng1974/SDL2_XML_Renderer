#include <fstream>
#include <string>
#include <iostream>
using namespace std;

#include "Shader.h"

// TODO
// Set up constructors so things don't get messed up if someone
// decides to copy this around or move it

// Basics
Shader::Shader()
	: m_bIsBound(false),
	m_Program(0),
	m_hVertShader(0),
	m_hFragShader(0)
{
}

Shader::~Shader()
{
	Unbind();
}

/*static*/ ShaderPtr Shader::FromSource(string v, string f){
	ShaderPtr ret(new Shader);

	ret->m_VertShaderSrc = v;
	ret->m_FragShaderSrc = f;
	int err = ret->CompileAndLink();

	// Exit on this, just because
	if (err)
		exit(err);


	// Don't bother with this yet, but it could work

	// Get all variables from shader now
	//auto bind = ret->ScopeBind();
	//GLuint prog = ret->m_Program;

	//HandleMap& hMap = ret->m_Handles;
	//auto getHandles = [&hMap, prog](GLint type){
	//	int N(0);
	//	GLenum props[] = { GL_NAME_LENGTH, GL_BLOCK_INDEX };

	//	glGetProgramInterfaceiv(prog, type, GL_ACTIVE_RESOURCES, &N);
	//	for (int i = 0; i < N; i++){
	//		GLint handle[] = { -1, -1 };
	//		char buf[256];
	//		glGetProgramResourceiv(prog, type, i, 2, props,2, NULL, handle);
	//		glGetProgramResourceName(prog, type, i, 256, NULL, buf);
	//		hMap[std::string(buf)] = handle[0];
	//	}
	//};

	//getHandles(GL_PROGRAM_INPUT);
	//getHandles(GL_UNIFORM);

	//for (int i = 0; i < numActiveAttribs; i++){
	//	GLint handle(-1);
	//	char buf[256];
	//	glGetProgramResourceiv(prog, GL_PROGRAM_INPUT, i, 1, &props, 1, NULL, &handle);
	//	glGetProgramResourceName(prog, GL_PROGRAM_INPUT, i, 256, NULL, buf);
	//	ret->m_Handles[std::string(buf)] = handle;
	//}

	//for (int i = 0; i < numActiveAttribs; i++){
	//	GLint handle(-1);
	//	char buf[256];
	//	glGetProgramResourceiv(prog, GL_UNIFORM, i, 1, &props, 1, NULL, &handle);
	//	glGetProgramResourceName(prog, GL_UNIFORM, i, 256, NULL, buf);
	//	ret->m_Handles[std::string(buf)] = handle;
	//}

	return std::move(ret);
}

/*static*/ ShaderPtr Shader::FromFile(string v, string f){
	ifstream vIn(v), fIn(f);
	string vSrc((istreambuf_iterator<char>(vIn)), istreambuf_iterator<char>());
	string fSrc((istreambuf_iterator<char>(fIn)), istreambuf_iterator<char>());

	return FromSource(vSrc, fSrc);
}

//// Source constructor
//Shader::Shader(string vs, string fs)
//	: Shader()
//{
//	vs = "../Resources/Shaders/" + vs;
//	fs = "../Resources/Shaders/" + fs;
//	ifstream vIn(vs), fIn(fs);
//	string v((istreambuf_iterator<char>(vIn)), istreambuf_iterator<char>());
//	string f((istreambuf_iterator<char>(fIn)), istreambuf_iterator<char>());
//	m_VertShaderSrc = v;
//	m_FragShaderSrc = f;
//	int err = CompileAndLink();
//
//	// Exit on this, just because
//	if (err)
//		exit(err);
//}

// Managing bound state
bool Shader::Bind(){
	if (!m_bIsBound){
		glUseProgram(m_Program);
		m_bIsBound = true;
	}
	return m_bIsBound == true;
}

bool Shader::Unbind(){
	if (m_bIsBound){
		glUseProgram(0);
		m_bIsBound = false;
	}
	return m_bIsBound == false;
}

bool Shader::IsBound() const{
	return m_bIsBound;
}

int Shader::CompileAndLink(){
	// Check if the shader op went ok
	auto check = [](GLuint id, GLuint type){
		GLint status(GL_FALSE);
		if (type == GL_COMPILE_STATUS)
			glGetShaderiv(id, type, &status);
		if (type == GL_LINK_STATUS)
			glGetProgramiv(id, type, &status);
		return status == GL_TRUE;
	};

	// Error codes to return
	const int ERR_N(0), ERR_V(1), ERR_F(2), ERR_L(4);

	// Weird thing that helps with glShaderSource
	const GLchar * shaderSrc[] = { m_VertShaderSrc.c_str(), m_FragShaderSrc.c_str() };

	// Compile Vertex Shader
	m_hVertShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(m_hVertShader, 1, &(shaderSrc[0]), 0);
	glCompileShader(m_hVertShader);
	if (!check(m_hVertShader, GL_COMPILE_STATUS)){
		cout << "Unable to compile vertex shader." << endl;
		PrintLog_V();
		return ERR_V;
	}

	// Compile Frag Shader
	m_hFragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(m_hFragShader, 1, &(shaderSrc[1]), 0);
	glCompileShader(m_hFragShader);
	if (!check(m_hFragShader, GL_COMPILE_STATUS)){
		cout << "Unable to compile fragment shader." << endl;
		PrintLog_F();
		return ERR_F;
	}

	// Create and Link Program
	m_Program = glCreateProgram();
	glAttachShader(m_Program, m_hVertShader);
	glAttachShader(m_Program, m_hFragShader);
	glLinkProgram(m_Program);
	if (!check(m_Program, GL_LINK_STATUS)){
		cout << "Unable to link shader program." << endl;
		return ERR_L;
	}

	return ERR_N;
}

// Accessor for shader handles
GLint Shader::getHandle (const string idx){
	// If we have the handle, return it
	if (m_Handles.find(idx) != m_Handles.end())
		return m_Handles.find(idx)->second;

	// Otherwise we have to find it, so client must bind
	if (!m_bIsBound){
		cout << "Error: shader queried for untested variable " << idx
			<< " while shader was not bound" << endl;
		return -1;
	}

	// Try and get handle, first as attr then as uniform
	GLint handle = glGetAttribLocation(m_Program, idx.c_str());
	if (handle < 0)
		handle = glGetUniformLocation(m_Program, idx.c_str());

	// valid handles begin at 0
	if (handle >= 0)
		m_Handles[idx] = handle;
	else{
		// We queried something bad, print the log
		cout << "Invalid variable " << idx << " queried in shader" << endl;
		PrintLog_V();
		PrintLog_F();
	}

	return handle;
}

// Print Logs
int Shader::PrintLog_V() const{
	const int max(1024);
	int len(0);
	char log[max];
	glGetShaderInfoLog(m_hVertShader, max, &len, log);
	cout << "Vertex Shader Log: \n\n" << log << "\n\n" << endl;

	return len;
}

int Shader::PrintLog_F() const{
	const int max(1024);
	int len(0);
	char log[max];
	glGetShaderInfoLog(m_hFragShader, max, &len, log);
	cout << "Fragment Shader Log: \n\n" << log << "\n\n" << endl;

	return len;
}

// Print Source
int Shader::PrintSrc_V() const{
	cout << "Vertex Shader Source: \n\n" << m_VertShaderSrc << "\n\n" << endl;
	return m_VertShaderSrc.length();
}

int Shader::PrintSrc_F() const{
	cout << "Fragment Shader Source: \n\n" << m_FragShaderSrc << "\n\n" << endl;
	return m_FragShaderSrc.length();
}