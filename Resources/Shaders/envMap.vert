#version 120

#define NUM_LIGHTS 4

struct Light{
	int Type;
	vec3 PosOrHalf;
	vec3 DirOrAtten; // or attenuation
	vec3 Intensity;
};
const int DIRECTIONAL = 0;
const int POINT = 1;
const int AMBIENT = 2;

uniform Light TheLights[NUM_LIGHTS];

uniform mat4 M;
uniform mat4 PV;
uniform mat3 N;

uniform vec3 u_wCameraPos;

// Vertex Attributes for position, normal; more to come
attribute vec2 a_Tex;
attribute vec3 a_Pos;
attribute vec3 a_Nrm;
attribute vec4 a_Tan; // I still don't really understand why it's a vec4

// I interpolate them for lighting calculations; is this common?
varying vec2 v_Tex;
varying vec3 v_Refl;
varying vec3 v_LightDir[NUM_LIGHTS];
varying vec3 v_Half[NUM_LIGHTS];

// Given three (unit) basis vectors, take v to a new space
vec3 changeBasis(vec3 v, vec3 nX, vec3 nY, vec3 nZ){
	vec3 v_T = vec3(0);
	v_T.x = dot(v, nX);
	v_T.y = dot(v, nY);
	v_T.z = dot(v, nZ);
	return v_T;
}

void main(){
	vec4 w_Pos = M * vec4(a_Pos, 1.0);
	gl_Position = PV * w_Pos;
	
	// Interpolate texture coordinate
	v_Tex = a_Tex;
	
	// Construct world space tangent basis
	vec3 n = normalize(N * a_Nrm);
	vec3 t = normalize(N * a_Tan.xyz);
	vec3 b = cross(n, t);
	
	// So eye vector doesn't depend on rotation?
	vec3 w_Eye = normalize(w_Pos.xyz - u_wCameraPos);
	vec3 t_Eye = changeBasis(w_Eye, t, b, n);
	
	v_Refl = reflect(w_Eye, n);
	
		// Transform light dir, pos, or half to tangent space
	for (int i=0; i<NUM_LIGHTS; i++)
	{
		int type = TheLights[i].Type;
		
		if ( type == POINT )
		{
			// Light dir goes from world pos to light pos
			vec3 v = TheLights[i].PosOrHalf - w_Pos.xyz;
			v_LightDir[i].x = dot(v, t);
			v_LightDir[i].y = dot(v, b);
			v_LightDir[i].z = dot(v, n); 
		}
		else if ( type == DIRECTIONAL )
		{
			// Light dir goes from world pos to light pos
			vec3 v = TheLights[i].DirOrAtten;
			v_LightDir[i].x = dot(v, t);
			v_LightDir[i].y = dot(v, b);
			v_LightDir[i].z = dot(v, n); 
		}
		else if (type == AMBIENT)
			continue;
		
		v_Half[i] = normalize(v_LightDir[i])+normalize(t_Eye);
	}
}