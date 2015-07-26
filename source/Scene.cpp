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

// TODO : Reserve IqmFile::IQM_T::CUSTOM for extras, make a multimap
using IqmTypeMap = map < IqmFile::IQM_T, GLint > ;

// Implementations below
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

	// Uniform Handles
	GLint PHandle = shader["P"];				// Projection Mat
	GLint wMVHandle = shader["MV_w"];			// World Space MV
	GLint eMVHandle = shader["MV_e"];			// Eye Space MV
	GLint NHandle = shader["N"];				// Normal Matrix
	GLint shinyHandle = shader["Mat.shininess"];// Shininess
	GLint diffHandle = shader["Mat.diff"];		// Diffuse
	GLint specHandle = shader["Mat.spec"];		// Specular

	Camera::SetProjHandle(PHandle);
	Camera::SetMVHandle(eMVHandle);

	Geometry::setMVHandle(wMVHandle);
	Geometry::setNormalHandle(NHandle);

	Material::setShinyHandle(shinyHandle);
	Material::setDiffHandle(diffHandle);
	Material::setSpecHandle(specHandle);

	// If these end up negative, so be it
	Geometry::setTexHandle(shader["u_TextureMap"]);
	Geometry::setNormalHandle(shader["u_NormalMap"]);

	for (auto el = elLight->FirstChildElement(); el; el = el->NextSiblingElement()){
		// Set up lights, going by light struct (individual array elements must be accessed because GL3)
		Light l;
		getLight(*el, l, cam.getView());
		string s = "L[i].";
		s[2] = '0' + m_vLights.size();
        
        l.SetTypeHandle(shader[s + "Type"]);
        l.SetPosOrHalfHandle(shader[s + "PosOrHalf"]);
        l.SetDirOrAttenHandle(shader[s + "DirOrAtten"]);
        l.SetIntensityHandle(shader[s + "I"]);
        
		// Put data on GPU
		createGPUAssets(l);
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
	// You've got to update each directional light's half vector, 
	// but for now I don't care (computed in frag shader)
	//for (int i = 0; i < m_vLights.size(); i++){
	//	if (m_vLights[i].m_Type() == Light::Type::DIRECTIONAL)
	//		// Upload norm(eye, m_vLights[i].dir)
	//}
	for (auto& geom : m_vGeometry){
		// Upload world MV, N matrices, uplaod
		mat4 wMV = geom.getMV();
		mat3 N(glm::inverse(glm::transpose(wMV)));
		glUniformMatrix4fv(Geometry::getMVHandle(), 1, GL_FALSE, (const GLfloat *)&wMV);
		glUniformMatrix3fv(Geometry::getNormalHandle(), 1, GL_FALSE, (const GLfloat *)&N);

		// Gotta get geom's material properties and upload them as uniforms (every call?)
		Material M = geom.getMaterial();
		vec4 diff = M.getDiff(), spec = M.getSpec();
		glUniform1f(Material::getShinyHandle(), M.getShininess());
		glUniform4f(Material::getDiffHandle(), diff[0], diff[1], diff[2], diff[3]);
		glUniform4f(Material::getSpecHandle(), spec[0], spec[1], spec[2], spec[3]);

		glBindVertexArray(geom.getVAO());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, geom.GetTexMap());

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, geom.GetNrmMap());

		glDrawElements(GL_TRIANGLES, geom.getNumIdx(), GL_UNSIGNED_INT, NULL);
	}
	glBindVertexArray(0);

	return m_vGeometry.size();
}

static string getGeom(XMLElement& elGeom, Geometry& geom){
	Material M;
	XMLElement * trEl = check(elGeom, "Transform");
	vec3 T(safeAtoF(*trEl, "Tx"), safeAtoF(*trEl, "Ty"), safeAtoF(*trEl, "Tz"));
	vec3 S(safeAtoF(*trEl, "Sx"), safeAtoF(*trEl, "Sy"), safeAtoF(*trEl, "Sz"));
	vec3 R(safeAtoF(*trEl, "Rx"), safeAtoF(*trEl, "Ry"), safeAtoF(*trEl, "Rz"));
	float rot = safeAtoF(*trEl, ("R"));
	mat4 MV = glm::translate(T) * glm::rotate(rot, R) * glm::scale(S);

	XMLElement * matEl = check(elGeom, "Material");
	float shininess(safeAtoF(*matEl, "shininess"));
	vec4 diff(safeAtoF(*matEl, "Dr"), safeAtoF(*matEl, "Dg"), safeAtoF(*matEl, "Db"), safeAtoF(*matEl, "Da"));
	vec4 spec(safeAtoF(*matEl, "Sr"), safeAtoF(*matEl, "Sg"), safeAtoF(*matEl, "Sb"), safeAtoF(*matEl, "Sa"));
	M = Material(shininess, diff, spec);

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

	geom.leftMultMV(MV);
	geom.setMaterial(M);
	geom.setTex(tex); // Move to material?
	geom.setNrm(nrm);

	// Should I load the file into memory here?
	string iqmFileName = elGeom.Attribute("fileName");

	return "../Resources/IQM/" + iqmFileName;
}

static IqmTypeMap getShader(XMLElement& elShade, Shader& shader){
	IqmTypeMap ret;

	// Also check to see if all variables described in XML are present
	string vSrc(check(elShade, "Vertex")->Attribute("src"));
	string fSrc(check(elShade, "Fragment")->Attribute("src"));
	shader = Shader(vSrc, fSrc);

	XMLElement * attrs = check(elShade, "Attributes");

	auto sBind = shader.ScopeBind();

	// Check all shader variables
	for (auto el = attrs->FirstChildElement(); el; el = el->NextSiblingElement()){
		string type(el->Value());
		string var(el->GetText());
		GLint handle = shader[var]; // Should I have the shader ensure it's an attribute?
		if (handle < 0){
			cout << "Invalid varibale queried in shader " << var << endl;
			continue; //exit(6);
		}
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
