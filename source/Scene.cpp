#include "Scene.h"
#include "Camera.h"
#include "Shader.h"
#include "IqmFile.h"
#include "Textures.h"

#include <string>
#include <iostream>
using namespace std;

#include <tinyxml2.h>
using namespace tinyxml2;

#include <gtc/type_ptr.hpp>
#include <gtx/transform.hpp>
#include <gtc/random.hpp>

// TODO : Reserve IqmFile::IQM_T::CUSTOM for extras, make a multimap
using IqmTypeMap = map < IqmFile::IQM_T, GLint > ;

GLint Scene::s_EnvMapHandle(-1);

// Implementations below (you should really just make constructors outta these)
static string getGeom(XMLElement& elGeom, Geometry& geom);
static IqmTypeMap getShader(XMLElement& elShade, Shader& shader);
static Camera::Type getCamera(XMLElement& elCam, Camera& cam);
static Light::Type getLight(XMLElement& elLight, Light& l, vec3 view);
static void createGPUAssets(IqmTypeMap iqmTypes, Geometry& geom, string fileName);
static void createGPUAssets(Light& l);

// tinyxml returns null if not found; my attempt at handling it here
static inline float safeAtoF(XMLElement& el, string query){
	const char * buf = el.Attribute(query.c_str());
	if (buf)
		return atof(buf);
	else{
		cout << "Error: null char ptr passed to atof" << endl;
		// Should I really exit here?
		exit(9);
	}
}

// Get a child XML element, if it exists, otherwise die
static inline XMLElement * check(XMLElement& parent, string name){
	XMLElement * ret = parent.FirstChildElement(name.c_str());
	if (!ret){
		cout << "Could not find XML element " << name << endl;
		exit(12);
	}
	return ret;
}

// TODO: Move constructor...
Scene::Scene(){}

// Source Constructor
// Take an XML file and use it to initialize a shader, camera,
// as well as any scene geometry and lights
Scene::Scene(string XmlSrc, Shader& shader, Camera& cam){
	XMLDocument doc;
	doc.LoadFile(XmlSrc.c_str());

	// Verify XML element exists
	auto check = [](string name, XMLElement * parent){
		XMLElement * ret = parent->FirstChildElement(name.c_str());
		if (!ret){
			cout << "XML Element " << name << " not found. " << endl;
			exit(5);
		}
		return ret;
	};

	// Get the root (Scene) element
	XMLElement * elScene = doc.FirstChildElement("Scene");
	if (!elScene){
		cout << "XML Root not found. " << endl;
		exit(5);
	}
	// Get and verify that important things are there
	XMLElement * elCam = check("Camera", elScene);
	XMLElement * elShade = check("Shader", elScene);
	XMLElement * elGeom = check("Geom", elScene);
	XMLElement * elLight = check("Light", elScene);

	// Init Camera
	Camera::Type camType = getCamera(*elCam, cam);
	if (camType == Camera::Type::NIL){
		cout << "Error creating Camera" << endl;
		exit(7);
	}

	// Init Shader, get map of vertex attributes
	IqmTypeMap iqmTypes = getShader(*elShade, shader);
	if (iqmTypes.empty()){
		cout << "Error: no attributes found in shader" << endl;
		exit(8);
	}

	// Bind shader, create GPU assets for geometry
	auto sBind = shader.ScopeBind();

	// Uniform Handles (really shouldn't be hard coded like this)
	Camera::SetProjHandle(shader["P"]); // Projection Matrix
	Camera::SetCHandle(shader["C"]); // Camera Matrix

	Geometry::setMVHandle(shader["MV_w"]); // World transform Matrix
	Geometry::setNHandle(shader["N"]); // Normal Matrix

	Material::setShinyHandle(shader["Mat.shininess"]); // Shininess
	Material::setDiffHandle(shader["Mat.diff"]); // Diffuse Color
	Material::setSpecHandle(shader["Mat.spec"]); // Specular color

	// If these end up negative, so be it
	Geometry::setTexMapHandle(shader["u_TextureMap"]); // Texture Map Sampler
	Geometry::setNrmMapHandle(shader["u_NormalMap"]); // Normal Map Sampler
	Scene::s_EnvMapHandle = shader["u_EnvMap"];

	// This causes GL_TEXTUREi to be associated with an int
	// Make a manager for this inside Textures namespace
	glUniform1i(shader["u_TextureMap"], 0);
	glUniform1i(shader["u_NormalMap"], 1);
	glUniform1i(shader["u_EnvMap"], 2);

	// Create Geometry
	for (XMLElement * el = elGeom->FirstChildElement(); el; el = el->NextSiblingElement()){
		Geometry g;
		string fileName = getGeom(*el, g);

		// I suppose, rather than a multimap,
		// you could just do a linear search in a vector by string

		// Check if we've already created the stuff on the GPU
		auto it = m_mapGeometry.find(fileName);
		if (it == m_mapGeometry.end())
			createGPUAssets(iqmTypes, g, fileName);
		else{
			// The only two things shared by instances are nIdx and VAO
			g.setVAO(it->second.getVAO());
			g.setNumIndices(it->second.getNumIdx());
		}
		m_mapGeometry.insert({ fileName, g });
	}

	assert(m_mapGeometry.size());

	// Set up the lights, arbitrarily assigning pre-existing geometry as its representation
	for (auto el = elLight->FirstChildElement(); el; el = el->NextSiblingElement()){
		// Create light struct
		Light l;
		getLight(*el, l, cam.getView());

		// Upload to shader (Must be accessed like "TheLights[i].Type, GL3)
		string s = "TheLights[_].";
		s[s.length() - 3] = '0' + m_vLights.size();

		// Store handles per light, since they could move
		l.SetTypeHandle(shader[s + "Type"]);
		l.SetPosOrHalfHandle(shader[s + "PosOrHalf"]);
		l.SetDirOrAttenHandle(shader[s + "DirOrAtten"]);
		l.SetIntensityHandle(shader[s + "Intensity"]);

		Geometry lightGeom = m_mapGeometry.begin()->second;
		lightGeom.identity(); // Maybe scale it somehow?
		lightGeom.leftMultMV(glm::scale(vec3(2.f)));

		// Give point lights a material
		if (l.getType() == Light::Type::POINT)
			lightGeom.setMaterial(Material(10.f, vec4(glm::linearRand(vec3(0), vec3(1)), 0.5f), vec4(1)));

		l.SetGeometry(lightGeom);

		// Put data on GPU
		createGPUAssets(l);
		m_vLights.push_back(l);
	}

	// Find a better place for this, do XML
	std::string cubeFaces[6] = { "posX.png", "negX.png", "posY.png", "negY.png", "posZ.png", "negZ.png" };
	m_EnvMap = Textures::CubeMap(cubeFaces);
}

// Client must bind shader
int Scene::Draw(){

	if (m_EnvMap > 0){
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_EnvMap);
	}

	// Draw each geom struct
	for (auto& G : m_mapGeometry)
		G.second.Draw();

	//Also draw the lights
	for (auto& light : m_vLights)
		if (light.getType() == Light::Type::POINT)
			light.GetGeometry().Draw();

	glBindVertexArray(0);

	// No need for this...
	return m_mapGeometry.size();
}

// Initialize a Geometry class given an XML element, return IQM file name
static string getGeom(XMLElement& elGeom, Geometry& geom){
	XMLElement * trEl = check(elGeom, "Transform");
	vec3 T(safeAtoF(*trEl, "Tx"), safeAtoF(*trEl, "Ty"), safeAtoF(*trEl, "Tz"));
	vec3 S(safeAtoF(*trEl, "Sx"), safeAtoF(*trEl, "Sy"), safeAtoF(*trEl, "Sz"));
	vec3 R(safeAtoF(*trEl, "Rx"), safeAtoF(*trEl, "Ry"), safeAtoF(*trEl, "Rz"));
	float rot = safeAtoF(*trEl, ("R"));
	mat4 MV = glm::translate(T) * glm::rotate(rot, R) * glm::scale(S);

	// Create a Material
	XMLElement * matEl = check(elGeom, "Material");
	float shininess(safeAtoF(*matEl, "shininess"));
	vec4 diff(safeAtoF(*matEl, "Dr"), safeAtoF(*matEl, "Dg"), safeAtoF(*matEl, "Db"), safeAtoF(*matEl, "Da"));
	vec4 spec(safeAtoF(*matEl, "Sr"), safeAtoF(*matEl, "Sg"), safeAtoF(*matEl, "Sb"), safeAtoF(*matEl, "Sa"));
	Material M(shininess, diff, spec);

	// Generate any textures
	GLuint tex = -1;
	if (matEl->Attribute("Texture"))
		tex = Textures::FromImage("../Resources/Textures/" + string(matEl->Attribute("Texture")));
	else // if no texture, make a color texture with diffuse color
		tex = Textures::FromSolidColor(diff);

	GLuint nrm = -1;
	if (matEl->Attribute("Normal"))
		nrm = Textures::NormalTexture("../Resources/Normals/" + string(matEl->Attribute("Normal")));
	//else // if no normal map, make a normal map with all zeros (?)
	//	nrm = Textures::FromSolidColor(diff);

	// Set values
	geom.leftMultMV(MV);
	geom.setMaterial(M);
	geom.setTexMap(tex); // Move to material?
	geom.setNrmMap(nrm);

	// Should I load the file into memory here?
	string iqmFileName = elGeom.Attribute("fileName");

	return "../Resources/IQM/" + iqmFileName;
}

// Creates a Shader from an XML element
static IqmTypeMap getShader(XMLElement& elShade, Shader& shader){
	// this will store all vertex attributes
	IqmTypeMap ret;

	// Check to see if all variables described in XML are present
	string vSrc(check(elShade, "Vertex")->Attribute("src"));
	string fSrc(check(elShade, "Fragment")->Attribute("src"));
	shader = Shader(vSrc, fSrc);

	// Declared vertex attributes
	XMLElement * attrs = check(elShade, "Attributes");

	auto sBind = shader.ScopeBind();
	// Make sure those variables exist in the shader
	for (auto el = attrs->FirstChildElement(); el; el = el->NextSiblingElement()){
		string type(el->Value());
		string var(el->GetText());
		GLint handle = shader[var]; // Should I have the shader ensure it's an attribute?
		// A negative handle means the query was unsuccessful
		if (handle < 0){
			cout << "Invalid variable queried in shader " << var << endl;
			continue; //exit(6);
		}
		// Populate returned map
		if (type.compare("Position") == 0)
			ret[IqmFile::IQM_T::POSITION] = handle;
		else if (type.compare("TexCoord") == 0)
			ret[IqmFile::IQM_T::TEXCOORD] = handle;
		else if (type.compare("Normal") == 0)
			ret[IqmFile::IQM_T::NORMAL] = handle;
		else if (type.compare("Tangent") == 0)
			ret[IqmFile::IQM_T::TANGENT] = handle;
	}

	return ret;
}

// Generate camera from XML element
static Camera::Type getCamera(XMLElement& elCam, Camera& cam){
	// See if ortho
	auto test = elCam.Attribute("left");
	if (!test){
		test = elCam.Attribute("fovy");
		if (!test)
			return Camera::Type::NIL;

		// Handle persp case
		float fovy = safeAtoF(elCam, "fovy");
		float aspect = safeAtoF(elCam, "aspect");
		vec2 NF(safeAtoF(elCam, "near"), safeAtoF(elCam, "far"));
		cam = Camera(fovy, aspect, NF);
		return Camera::Type::PERSP;
	}

	// Get scene dims, set up camera
	vec2 LR(safeAtoF(elCam, "left"), safeAtoF(elCam, "right"));
	vec2 BT(safeAtoF(elCam, "bottom"), safeAtoF(elCam, "top"));
	vec2 NF(safeAtoF(elCam, "near"), safeAtoF(elCam, "far"));
	cam = Camera(LR, BT, NF);

	return Camera::Type::ORTHO;
}

// Generate light from XML
static Light::Type getLight(XMLElement& elLight, Light& l, vec3 view){
	Light::Type ret(Light::Type::NIL);

	// Position, direction (or attenuation coefs), intensity (color)
	vec3 pos(safeAtoF(elLight, "pX"), safeAtoF(elLight, "pY"), safeAtoF(elLight, "pZ"));
	vec3 dir(safeAtoF(elLight, "dX"), safeAtoF(elLight, "dY"), safeAtoF(elLight, "dZ"));
	vec3 intensity(safeAtoF(elLight, "iR"), safeAtoF(elLight, "iG"), safeAtoF(elLight, "iB"));

	// If this stays nil the shader won't use it
	string lType(elLight.Value());

	if (lType.compare("Directional") == 0)
		ret = Light::Type::DIRECTIONAL;

	if (lType.compare("Point") == 0)
		ret = Light::Type::POINT;

	if (lType.compare("Ambient") == 0)
		ret = Light::Type::AMBIENT;

	l = Light(ret, pos, dir, intensity);
	return ret;
}

// Caller must bind shader before this can work
static void createGPUAssets(IqmTypeMap iqmTypes, Geometry& geom, string fileName){
	IqmFile iqmFile(fileName);

	// Lambda to generate a VBO
	auto makeVBO = []
		(GLuint buf, GLint handle, void * ptr, GLsizeiptr numBytes, GLuint dim, GLuint type){
		glBindBuffer(GL_ARRAY_BUFFER, buf);
		glBufferData(GL_ARRAY_BUFFER, numBytes, ptr, GL_STATIC_DRAW);
		glEnableVertexAttribArray(handle);
		glVertexAttribPointer(handle, dim, type, 0, 0, 0);
		//Disable?
	};

	// Create VAO, then create all VBOs
	GLuint VAO(0), bIdx(0), nIndices(0);
	vector<GLuint> bufVBO(iqmTypes.size() + 1);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(bufVBO.size(), bufVBO.data());

	// Get all VBO data
	for (auto it = iqmTypes.cbegin(); it != iqmTypes.cend(); ++it){
		switch (it->first){
		case IqmFile::IQM_T::POSITION:
		{
			auto pos = iqmFile.Positions();
			makeVBO(bufVBO[bIdx++], it->second, pos.ptr(), pos.numBytes(), pos.nativeSize() / sizeof(float), GL_FLOAT);
		}
		break;
		case IqmFile::IQM_T::TEXCOORD:
		{
			auto tex = iqmFile.TexCoords();
			makeVBO(bufVBO[bIdx++], it->second, tex.ptr(), tex.numBytes(), tex.nativeSize() / sizeof(float), GL_FLOAT);
		}
		break;
		case IqmFile::IQM_T::NORMAL:
		{
			auto nrm = iqmFile.Normals();
			makeVBO(bufVBO[bIdx++], it->second, nrm.ptr(), nrm.numBytes(), nrm.nativeSize() / sizeof(float), GL_FLOAT);
		}
		break;
		case IqmFile::IQM_T::TANGENT:
		{
			auto tng = iqmFile.Tangents();
			makeVBO(bufVBO[bIdx++], it->second, tng.ptr(), tng.numBytes(), tng.nativeSize() / sizeof(float), GL_FLOAT);
		}
		break;
		default:
			it = iqmTypes.erase(it);
		}
	}

	// Indices
	auto idx = iqmFile.Indices();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufVBO[bIdx]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.numBytes(), idx.ptr(), GL_STATIC_DRAW);
	geom.setNumIndices(idx.count());

	geom.setVAO(VAO);
	glBindVertexArray(0);
}

static void createGPUAssets(Light& l){
	vec3 pos(l.getPos()), dir(l.getDir()), I(l.getIntensity());
	glUniform1i(l.GetTypeHandle(), (int)l.getType());
	glUniform3f(l.GetPosOrHalfHandle(), pos[0], pos[1], pos[2]);
	glUniform3f(l.GetDirOrAttenHandle(), dir[0], dir[1], dir[2]);
	glUniform3f(l.GetIntensityHandle(), I[0], I[1], I[2]);
}
