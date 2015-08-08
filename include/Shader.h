#pragma once

#include "GL_Includes.h"

// You should make copying / moving safe
#include <string>
#include <map>
#include <memory>

// Useful for looking up shader variables and handles
using HandleMap = std::map < std::string, GLint >;

class Shader;
using ShaderPtr = std::shared_ptr < Shader >;

class Shader
{
	// Private initializer
	int CompileAndLink();
	Shader();
public:
	static ShaderPtr FromSource(std::string v, std::string f);
	static ShaderPtr FromFile(std::string v, std::string f);

    // Constructors (default, source), destructor (move!)
	~Shader();
    
    // Bound status
	bool Bind();
	bool Unbind();
	bool IsBound() const;
    
    // Logging functions
	bool PrintError() const;
	int PrintLog_V() const;
	int PrintLog_F() const;
	int PrintSrc_V() const;
	int PrintSrc_F() const;
    
    // Public Accessors
	GLint getHandle(const std::string idx);
    
    // Why not?
	inline HandleMap getHandleMap() { return m_Handles; }
private:
    // Bound status, program/shaders, source, handles
	bool m_bIsBound;
	GLuint m_Program;
	GLuint m_hVertShader;
	GLuint m_hFragShader;
	std::string m_VertShaderSrc, m_FragShaderSrc;
	HandleMap m_Handles;
    
    // Public scoped bind class
    // binds shader for as long as it lives
public:
    class ScopedBind {
        friend class Shader;
    protected:
        Shader& m_Shader;
        ScopedBind(Shader& s) : m_Shader(s) { m_Shader.Bind(); }
    public:
        ~ScopedBind(){ m_Shader.Unbind(); }
    };
    inline ScopedBind ScopeBind(){ return ScopedBind(*this); }
};

