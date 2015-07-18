#pragma once

#include "Light.h"
#include "Camera.h"
#include "Shader.h"
#include "Geometry.h"

#include <vector>
using GeomVec = std::vector < Geometry >;
using LightVec = std::vector < Light >;

class Scene
{
public:
	Scene();
	Scene(std::string XmlSrc, Shader& shader, Camera& cam);
	~Scene();
	int Draw();
private:
	//Camera m_Camera;
	//Shader m_Shader;
	GeomVec m_vGeometry;
	LightVec m_vLights;
};

