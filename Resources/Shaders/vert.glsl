#version 120

#define NUM_LIGHTS 3

// Definition of a Light Struct; guess how many there are
struct Light{
	int type;
	vec3 pos;
	vec3 dir;
	vec3 intensity;
};

// Where I keep 'em
uniform Light L[NUM_LIGHTS];

// Model View, Projection
uniform mat4 MV;
uniform mat4 P;

// Vertex Attributes for position, normal; more to come
attribute vec3 a_Pos;
attribute vec3 a_Nrm;

// I interpolate them for lighting calculations; is this common?
varying vec3 v_Nrm;
varying vec3 v_Pos;

void main(){
	// Find position in world space, for lighting calc
	vec4 w_Pos = MV * vec4(a_Pos, 1);
	// Screen space
	gl_Position = P * w_Pos;
	// Interpolate
	v_Nrm = a_Nrm;
	v_Pos = w_Pos.xyz;
}