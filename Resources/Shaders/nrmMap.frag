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

struct Material{
	float shininess;
	vec4 diff;
	vec4 spec;
};

uniform Light TheLights[NUM_LIGHTS];
uniform Material Mat;
uniform sampler2D u_TextureMap;
uniform sampler2D u_NormalMap;
uniform samplerCube u_EnvMap;

varying vec2 v_Tex;
varying vec3 v_Eye;
varying vec3 v_Refl;
//varying vec3 v_Half[NUM_LIGHTS];
varying vec3 v_Light[NUM_LIGHTS];

void main(){
	// Compute total light contribution from all lights, all diffuse for now
	vec4 light = vec4(0,0,0,1);
	// Normal is stored [0,1] but must be mapped [-1,1]
	vec3 nrm = 2. * texture2D(u_NormalMap, v_Tex).rgb - 1.;
	
	for (int i = 0; i < NUM_LIGHTS; i++){
		// Grab intensity from each light,
		vec4 lightColor = vec4(TheLights[i].Intensity, 1);
	
		// multiply by each light's contribution
		if (TheLights[i].Type == DIRECTIONAL)
		{ 
			vec3 L = normalize(v_Light[i]);
			vec3 H = 0.5 * (L + normalize(v_Eye));
			float nDotL = max(0, dot(nrm, L));
			float nDotHV = max(0, dot(nrm, L));
			float pf = nDotHV == 0 ? 0 : pow(nDotHV, Mat.shininess);
			
			vec4 contrib = nDotL * Mat.diff + pf * Mat.spec;
			light += contrib * lightColor;
		}
		else if (TheLights[i].Type == 1) // point
		{ // For point lights, 
			// attenuation factor
			vec3 L = normalize(v_Light[i]);
			vec3 H = 0.5 * (L + normalize(v_Eye));
			float d = length(v_Light[i]);
			float nDotL = max(0, dot(nrm, L));
			float nDotHV = max(0, dot(nrm, H));
			float pf = nDotHV == 0 ? 0 : pow(nDotHV, Mat.shininess);
			float attenuation = 1.0 / // DirOrAtten becomes quadratic polynomial coefs
				(TheLights[i].DirOrAtten[0] + 
				TheLights[i].DirOrAtten[1] * d + 
				TheLights[i].DirOrAtten[2] * d*d);
				
			vec4 contrib = nDotL * Mat.diff + pf * Mat.spec;
			light += attenuation * contrib * lightColor;
		}
		else if (TheLights[i].Type == 2) // ambient
		{
			light += lightColor;
		}
	}

	vec4 clr = textureCube(u_EnvMap, v_Refl);
	
	gl_FragColor = mix(texture2D (u_TextureMap, v_Tex) * light, clr, 1.0);
}
