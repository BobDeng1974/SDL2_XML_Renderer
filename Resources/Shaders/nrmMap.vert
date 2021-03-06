#version 140

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

// Model View, Projection, Normal (Inv(Trans(MV)))
uniform mat4 MV_w;
uniform mat4 C;
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
varying vec3 v_Refl;
//varying vec3 v_Half[NUM_LIGHTS];
varying vec3 v_Light[NUM_LIGHTS];
//varying LightData v_LightData [NUM_LIGHTS];

void main(){
	// Get world, eye, and screen position of vertex
	vec4 w_Pos = MV_w * vec4(a_Pos,1);
	vec4 e_Pos = C * w_Pos; // normalize?
	gl_Position = P * e_Pos;
	
	// Interpolate texture coordinate
	v_Tex = a_Tex;
	
	// Construct tangent basis
	vec3 n = normalize(N * a_Nrm);
	vec3 t = normalize(N * a_Tan.xyz);
	vec3 b = cross(n, t);
	
	// Transform eye vector (- eye position) to tangent space
	vec3 v = -e_Pos.xyz;
	v_Eye.x = dot(v, t);
	v_Eye.y = dot(v, b);
	v_Eye.z = dot(v, n);
	//v_Eye = normalize(v_Eye);

	// The inverse of a rotation mat is its transpose
	v_Refl = reflect(normalize(e_Pos.xyz*mat3(C)), n);

	// Transform light dir, pos, or half to tangent space
	for (int i=0; i<NUM_LIGHTS; i++)
	{
		int type = TheLights[i].Type;
		if ( type == POINT )
		{
			// Light dir goes from world pos to light pos
			v = TheLights[i].PosOrHalf - w_Pos.xyz;
			v_Light[i].x = dot(v, t);
			v_Light[i].y = dot(v, b);
			v_Light[i].z = dot(v, n); 
			//v_Half[i] = 0.5*(normalize(v_Light[i]) + v_Eye);
		}
		else if ( type == DIRECTIONAL )
		{
			// Light dir goes from world pos to light pos
			v = TheLights[i].DirOrAtten;
			v_Light[i].x = dot(v, t);
			v_Light[i].y = dot(v, b);
			v_Light[i].z = dot(v, n); 
			//v_Half[i] = 0.5*(normalize(v_Light[i]) + v_Eye);
		}
	}
}