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
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

// TODO : Reserve IqmFile::IQM_T::CUSTOM for extras, make a multimap
using IqmTypeMap = map < IqmFile::IQM_T, GLint > ;

// Implementations below
static string getGeom(XMLElement& elGeom, Geometry& geom);
static IqmTypeMap getShader(XMLElement& elShade, Shader& shader);
static Camera::Type getCamera(XMLElement& elCam, Camera& cam);
static Light::Type getLight(XMLElement& elLight, Light& l, vec3 view);
static void createGPUAssets(IqmTypeMap iqmTypes, Geometry& geom, string fileName);
static void createGPUAssets(GLint handles[4], Light& l);

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

// TODO: Move constructor...
Scene::Scene(){}

Scene::~Scene(){}

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

	// Grab MV, P handles (make this better)
	GLint MVHandle = shader["MV"], PHandle = shader["P"];
	Camera::setProjHandle(PHandle);
	Geometry::setMVHandle(MVHandle);

	// Get texture handle
	GLint TexHandle = shader["u_Sampler"];
	Geometry::setTexHandle(TexHandle);

	for (auto el = elLight->FirstChildElement(); el; el = el->NextSiblingElement()){
		// Set up lights, going by light struct (individual array elements must be accessed because GL3)
		Light l;
		getLight(*el, l, cam.getView());
		string s = "L[i].";
		s[2] = '0' + m_vLights.size();
		GLint handles[4] = {
			shader[s + "type"],
			shader[s + "pos"],
			shader[s + "dir"],
			shader[s + "intensity"],
		};
		// Put data on GPU
		createGPUAssets(handles, l);
		m_vLights.push_back(l);
	}

	// Create Geometry
	for (auto el = elGeom->FirstChildElement(); el; el = el->NextSiblingElement()){
		Geometry g;
		string fileName = getGeom(*el, g);
		createGPUAssets(iqmTypes, g, fileName);
		m_vGeometry.push_back(g);
	}
}

// Client must bind shader
int Scene::Draw(){
	for (auto& geom : m_vGeometry){
		glUniformMatrix4fv(Geometry::getMVHandle(), 1, GL_FALSE, (const GLfloat *)geom.getMVPtr());
		glBindVertexArray(geom.getVAO());
		glBindTexture(GL_TEXTURE_2D, geom.getTex());
		glDrawElements(GL_TRIANGLES, geom.getNumIdx(), GL_UNSIGNED_INT, NULL);
	}
	glBindVertexArray(0);

	return m_vGeometry.size();
}

static string getGeom(XMLElement& elGeom, Geometry& geom){
	// Get Color, Translate, Scale, Rotate from XML - Could easily segfault here...
	vec4 C(safeAtoF(elGeom, "Cr"), safeAtoF(elGeom, "Cg"), safeAtoF(elGeom, "Cb"), safeAtoF(elGeom, "Ca"));
	vec3 T(safeAtoF(elGeom, "Tx"), safeAtoF(elGeom, "Ty"), safeAtoF(elGeom, "Tz"));
	vec3 S(safeAtoF(elGeom, "Sx"), safeAtoF(elGeom, "Sy"), safeAtoF(elGeom, "Sz"));
	vec3 R(safeAtoF(elGeom, "Rx"), safeAtoF(elGeom, "Ry"), safeAtoF(elGeom, "Rz"));
	float rot = safeAtoF(elGeom, ("R"));
	mat4 MV = glm::translate(T) * glm::rotate(rot, R) * glm::scale(S);

	GLuint tex;
	if (elGeom.Attribute("Texture"))
		tex = Textures::FromImage("../Resources/Textures/"+string(elGeom.Attribute("Texture")));
	else
		tex = Textures::FromSolidColor(C);

	geom.setColor(C);
	geom.leftMultMV(MV);
	geom.setTex(tex);

	// Should I load the file into memory here?
	string iqmFileName = elGeom.GetText();

	return "../Resources/IQM/" + iqmFileName;
}

static IqmTypeMap getShader(XMLElement& elShade, Shader& shader){
	IqmTypeMap ret;

	// Also check to see if all variables described in XML are present
	string vSrc(elShade.Attribute("vert")), fSrc(elShade.Attribute("frag"));
	shader = Shader(vSrc, fSrc);
	auto sBind = shader.ScopeBind();

	// Check all shader variables
	for (auto el = elShade.FirstChildElement(); el; el = el->NextSiblingElement()){
		string type(el->Value());
		if (type.compare("Position") == 0){
			string var(el->GetText());
			GLint handle = shader[var];
			if (handle < 0){
				cout << "Something bad" << endl;
				exit(6);
			}
			// Should I have the shader ensure it's an attribute?
			ret[IqmFile::IQM_T::POSITION] = handle;
		}
		if (type.compare("TexCoord") == 0){
			string var(el->GetText());
			GLint handle = shader[var];
			if (handle < 0){
				cout << "Something bad" << endl;
				exit(6);
			}
			// Should I have the shader ensure it's an attribute?
			ret[IqmFile::IQM_T::TEXCOORD] = handle;
		}
		else if (type.compare("Normal") == 0){
			string var(el->GetText());
			GLint handle = shader[var];
			if (handle < 0){
				cout << "Something bad" << endl;
				exit(6);
			}
			// Should I have the shader ensure it's an attribute?
			ret[IqmFile::IQM_T::NORMAL] = handle;
		}
	}

	return ret;
}

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

static Light::Type getLight(XMLElement& elLight, Light& l, vec3 view){
	Light::Type ret(Light::Type::NIL);

	vec3 pos(safeAtoF(elLight, "pX"), safeAtoF(elLight, "pY"), safeAtoF(elLight, "pZ"));
	vec3 dir(safeAtoF(elLight, "dX"), safeAtoF(elLight, "dY"), safeAtoF(elLight, "dZ"));
	vec3 intensity(safeAtoF(elLight, "iR"), safeAtoF(elLight, "iG"), safeAtoF(elLight, "iB"));

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

// Caller must bind shader (SBind could be an arg...) (does it have to be bound?)
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
		default:
			it = iqmTypes.erase(it);
		}
	}

	// Indices
	auto idx = iqmFile.Indices();
	nIndices = idx.count();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufVBO[bIdx]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.numBytes(), idx.ptr(), GL_STATIC_DRAW);

	geom.setNumIndices(nIndices);
	geom.setVAO(VAO);

	glBindVertexArray(0);
}

static void createGPUAssets(GLint handles[4], Light& l){
	glUniform1i(handles[0], (int)l.m_Type);
	glUniform3f(handles[1], l.m_Pos[0], l.m_Pos[1], l.m_Pos[2]);
	glUniform3f(handles[2], l.m_Dir[0], l.m_Dir[1], l.m_Dir[2]);
	glUniform3f(handles[3], l.m_Intensity[0], l.m_Intensity[1], l.m_Intensity[2]);
}
