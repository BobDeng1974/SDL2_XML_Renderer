#pragma once

#include "Light.h"
#include "Camera.h"
#include "Shader.h"

#include <vector>

// Monolith used to contain everything else
// Used to own shader and camera, but what's the point
// (maybe camera, but not really I guess)

// Using this allows multiple instances of same GPU resources
using GeomVec = std::vector <Geometry > ;
using LightVec = std::vector < Light >;

class Scene
{
public:
    // Constructors
	Scene();
	Scene(std::string XmlSrc, ShaderPtr& shader, Camera& cam);
    
    // Render function
	int Draw();
    
private:
    // Will probably need more
	GeomVec m_vGeometry;
	LightVec m_vLights;

	GLuint m_EnvMap;
	static GLint s_EnvMapHandle;
};

