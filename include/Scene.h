#pragma once

#include "Light.h"
#include "Camera.h"
#include "Shader.h"
#include "Geometry.h"

#include <vector>

// Monolith used to contain everything else
// Used to own shader and camera, but what's the point
// (maybe camera, but not really I guess)

using GeomVec = std::vector < Geometry >;
using LightVec = std::vector < Light >;

class Scene
{
public:
    // Constructors
	Scene();
	Scene(std::string XmlSrc, Shader& shader, Camera& cam);
    
    // Render function
	int Draw();
    
private:
    // Will probably need more
	GeomVec m_vGeometry;
	LightVec m_vLights;
};

