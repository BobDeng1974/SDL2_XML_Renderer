#version 120

#define NUM_LIGHTS 3

struct Light{
	int type;
	vec3 PosOrHalf;
	vec3 DirOrAtten; // or attenuation
	vec3 I;
};

struct Material{
//	sampler2D NormalMap;
//	sampler2D HeightMap;
	float shininess;
	vec4 diff;
	vec4 spec;
};

uniform Light L[NUM_LIGHTS];
uniform Material Mat;
uniform vec3 u_Eye;
uniform sampler2D u_TextureMap;

varying vec3 v_Nrm;
varying vec3 v_Pos;
varying vec2 v_Tex;
//varying vec3 v_BiTan;

void main(){
	// Compute total light contribution from all lights, all diffuse for now
	vec4 light = vec4(0);
	for (int i = 0; i < NUM_LIGHTS; i++){
		vec4 lightColor = vec4(L[i].I, 1);
		if (L[i].type == 0) // directional
		{
			float nDotVP = max(0, dot(v_Nrm, L[i].DirOrAtten));
			float nDotHV = max(0, dot(v_Nrm, L[i].PosOrHalf));
			float pf = nDotVP == 0 ? 0 : pow(nDotHV, Mat.shininess);
			vec4 contrib = nDotVP * Mat.diff + pf * Mat.spec;
			light += contrib * lightColor;
		}
		else if (L[i].type == 1) // point
		{
			vec3 VP = (L[i].PosOrHalf - v_Pos);
			vec3 H = normalize(VP + u_Eye);
			float d = length(VP);
			float nDotVP = max(0, dot(v_Nrm, normalize(VP)));
			float nDotHV = max(0, dot(v_Nrm, H));
			float pf = nDotVP == 0 ? 0 : pow(nDotHV, Mat.shininess);
			float attenuation = 1.0 / // DirOrAtten becomes quadratic polynomial coefs
				(L[i].DirOrAtten[0] + L[i].DirOrAtten[1] * d + L[i].DirOrAtten[2] * d*d);
			vec4 contrib = nDotVP * Mat.diff + pf * Mat.spec;
			light += attenuation * contrib * lightColor;// * attenuation * nDotVP;
		}
		else if (L[i].type == 2) // ambient
		{
			light += lightColor;
		}
	}
	
	gl_FragColor = texture2D(u_TextureMap, v_Tex) * light;
}
