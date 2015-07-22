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
uniform mat4 MV_w;
uniform mat4 MV_e;
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
varying vec3 v_Eye;
//varying vec3 v_BiTan;

// Lighting stuff

void main(){
	// Find position in world space
	vec4 w_Pos = MV_w * vec4(a_Pos, 1);
	// Find position in eye space (this could be optimized)
	vec4 e_Pos = MV_e * w_Pos;
	// Find position in Screen space
	gl_Position = P * e_Pos;
	
	// Interpolate
	v_Nrm = normalize(N * a_Nrm);
	v_Pos = w_Pos.xyz;
	v_Eye = -e_Pos.xyz;
	v_Tex = a_Tex;
}

/*
vec3 e_Light, e_Half, e_Nrm = normalize(N * a_Nrm);
// So now I need the lights to be in eye space
for (int i = 0; i < NUM_LIGHTS; i++){
if (L[i].type == 2)
continue;
// For directional, Light Vec = light dir transformed into eye space, negated
if (L[i].type == 0)
{
e_Light = -normalize((MV * vec4(L[i].DirOrAtten, 0)).xyz);
}
// For point
else if (L[i].type == 1)
{
// Light Vec = light pos - pos, in eye space
e_Light = (MV * vec4(L[i].PosOrHalf, 1)).xyz - e_Pos.xyz;
// Distance
d[i] = length(e_Light);
e_Light = normalize(e_Light);
}
// Half vec = (lightVec + eyeVec)/2, eyeVec is -Position in eye space
e_Half = 0.5 * (e_Light - normalize(e_Pos.xyz));
nDotVP[i] =  max(0, dot(e_Nrm, normalize(e_Light)));
nDotHV[i] = max(0, dot(e_Nrm, e_Half));
}
*/