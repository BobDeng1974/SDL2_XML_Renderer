// We still need the uniform light
struct Light{
	vec3 P;  // Position in world space
	vec3 I;  // Intensity (light color as RGB float)
};
uniform Light u_Light;

// Uniform material color, passed in from host
uniform vec3 u_Color;

// Normal Map
uniform sampler2D u_NormalMap;

// Same varying vectors from before
varying vec2 v_TexCoord; 
varying vec3 v_HalfVec;  
varying vec3 v_LightVec;

void main(){
	// Grab RGB color from normal map, take from [0,1] to [-1,1]
	vec3 nrm = 2. * texture2D(u_NormalMap, v_Tex).rgb - 1.;
	
	// Use cosine law to find light contribution
	float nDotL = max(0, dot(nrm, v_LightVec));
	float nDotH = max(0, dot(nrm, v_HalfVec));
	float powerFactor = nDotHV == 0 ? 0 : pow(nDotHV, 100.); // arbitrary shininess
	
	// Assume light color, specular color are both white
	vec3 lightColor = u_Light.I * (nDotL + powerFactor);
	
	gl_FragColor = u_Color * lightColor;
}