#version 120

// I am holding off on the normal / height map stuff until I know lighting actually works

// Definition of a Light Struct; guess how many there are
//struct Light{
//	int type;
//	vec3 PosOrHalf;
//	vec3 DirOrAttenuation; // or attenuation
//	vec4 I;
//};

// Where I keep 'em
//uniform Light L[NUM_LIGHTS];

// Model View, Projection, Normal (Inv(Trans(MV)))
uniform mat4 MV;
uniform mat4 P;
uniform mat3 N;

// Vertex Attributes for position, normal; more to come
attribute vec2 a_Tex;
attribute vec3 a_Pos;
attribute vec3 a_Nrm;
//attribute vec4 a_Tan; // I still don't really understand why it's a vec4

// I interpolate them for lighting calculations; is this common?
varying vec2 v_Tex;
varying vec3 v_Nrm;
varying vec3 v_Pos;
//varying vec3 v_BiTan;

// Lighting stuff

void main(){
	// Find position in world space, for lighting calc
	vec4 w_Pos = MV * vec4(a_Pos, 1);
	// Screen space
	gl_Position = P * w_Pos;
	
	// Interpolate
	v_Nrm = normalize(N * a_Nrm);
	v_Pos = w_Pos.xyz;
	v_Tex = a_Tex;
	//vec3 t_Tan = mat3(MV) * a_Tan.xyz; // ?
	//v_BiTan = cross(v_Nrm, t_Tan);
}