#version 120

#define NUM_LIGHTS 3

struct Light{
	int Type;
	vec3 PosOrHalf;
	vec3 DirOrAtten; // or attenuation
	vec3 I;
};
const int DIRECTIONAL = 0;
const int POINT = 1;
const int AMBIENT = 2;

/* struct LightData{
	vec3 L;
	vec3 H;
}; */

// Model View, Projection, Normal (Inv(Trans(MV)))
uniform mat4 MV_w;
uniform mat4 MV_e;
uniform mat4 P;
uniform mat3 N;

// Scene Lights
uniform Light TheLights[NUM_LIGHTS];

// Vertex Attributes for position, normal; more to come
attribute vec2 a_Tex;
attribute vec3 a_Pos;
attribute vec3 a_Nrm;
attribute vec4 a_Tan; // I still don't really understand why it's a vec4

// I interpolate them for lighting calculations; is this common?
varying vec2 v_Tex;
varying vec3 v_Eye;
varying vec3 v_Half[NUM_LIGHTS];
varying vec3 v_Light[NUM_LIGHTS];
//varying LightData v_LightData [NUM_LIGHTS];

void main(){
	// Get world, eye, and screen position of vertex
	vec3 w_Pos = (MV_w * vec4(a_Pos,1)).xyz;
	vec3 e_Pos =  (MV_e * vec4(w_Pos,1)).xyz; // normalize?
	gl_Position = P * vec4(e_Pos,1);
	
	// Interpolate texture coordinate
	v_Tex = a_Tex;
	
	// Construct tangent basis
	vec3 n = normalize(N * a_Nrm);
	vec3 t = normalize(N * a_Tan.z * a_Tan.xyz);
	vec3 b = cross(n, t);
	
	// Transform eye vector (- eye position) to tangent space
	vec3 v;
	v.x = dot(e_Pos, t);
	v.y = dot(e_Pos, b);
	v.z = dot(e_Pos, n);
	v_Eye = -normalize(v);

	// Transform light dir, pos, or half to tangent space
	for (int i=0; i<NUM_LIGHTS; i++)
	{
		int type = TheLights[i].Type;
		if ( type == POINT )
		{
			// Light dir goes from world pos to light pos
			w_Pos = TheLights[i].PosOrHalf - w_Pos;
			v.x = dot(w_Pos, t);
			v.y = dot(w_Pos, b);
			v.z = dot(w_Pos, n);
			v_Light[i] = v; // don't normalize for atten
			
			// Half vector is 0.5(light+eye) 
			e_Pos = 0.5*(v_Light[i] + v_Eye);
			v.x = dot(e_Pos, t);
			v.y = dot(e_Pos, b);
			v.z = dot(e_Pos, n);
		}
		else if ( type == DIRECTIONAL )
		{
		// Negation?
			v.x = dot(TheLights[i].PosOrHalf, t);
			v.y = dot(TheLights[i].PosOrHalf, b);
			v.z = dot(TheLights[i].PosOrHalf, n);
			v_Half[i] = normalize(v);
			
			v.x = dot(TheLights[i].DirOrAtten, t);
			v.y = dot(TheLights[i].DirOrAtten, b);
			v.z = dot(TheLights[i].DirOrAtten, n);
			v_Light[i] = normalize(v);
		}
	}
}