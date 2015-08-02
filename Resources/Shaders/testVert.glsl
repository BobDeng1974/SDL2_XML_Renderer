// Light Struct, just owns position and color (intensity)
struct Light{
	vec3 P;  // Position in world space
	vec3 I;  // Intensity (light color as RGB float)
};
uniform Light u_Light;

uniform mat4 MV_w;  // World space transform of geometry
uniform mat4 MV_e;  // Camera transform, brings world space => eye space
uniform mat3 N;     // Normal matrix, N = mat3( inverse( transpose( MV_e * MV_w ) ) )
uniform mat4 P;     // Projection matrix, takes eye space to screen space

//Vertex Attributes
attribute vec2 a_TexCoord; // Texture coordinate
attribute vec3 a_Position; // World position
attribute vec3 a_Normal;   // Vertex normal, normalized
attribute vec4 a_Tangent;  // Vertex tangent (normalized? IQM gives me a vec4...)

// To be interpolated for the fragment shader
varying vec2 v_TexCoord; // the interpolated texture coordinate

// These two are sent to the fragment shader in tangent space; x>tangent, y=>bitangent, z=>vertex normal
varying vec3 v_HalfVec;  // the half vector between the eye vector and the light vector
varying vec3 v_LightVec; // The light direction ( light position - vertex position )

// Given three (unit) basis vectors, take v to a new space
vec3 changeBasis(vec3 v, vec3 nX, vec3 nY, vec3 nZ){
	vec3 v_T = vec3(0);
	v_T.x = dot(nX, t);
	v_T.y = dot(nY, b);
	v_T.z = dot(nZ, n);
	return v_T;
}

void main(){
	// Get world and eye position
	vec4 w_Pos = MV_w * vec4(a_Position, 1.0); // World space vertex position
	vec4 e_Pos = MV_e * w_Pos; // Eye space position
	
	// Interpolate texture coordinate
	v_TexCoord = a_TexCoord;
	
	// new basis (note that N is the normal matrix for the eye space transform; is that bad?)
	vec3 n = normalize(N * a_Normal); 
	vec3 t = normalize(N * a_Tangent.xyz); // What about w?
	vec3 b = cross(n, t);
	
	// Transform light direction to tangent space
	vec3 L = u_Light.P - w_Pos.xyz; // don't normalize for attenuation
	v_LightVec = normalize(changeBasis(L, t, b, n));
	
	// Transform eye vector (- eye position) to tangent space
	vec3 E = -e_Pos.xyz;
	vec3 E_t = normalize(changeBasis(E, t, b, n));
	
	// Find half vector, in tangent space
	v_HalfVec = 0.5 * (v_LightVec + E_t); // These are both unit, so is this OK?
	
	// Find screen space position
	gl_Position = P * e_Pos;
}

