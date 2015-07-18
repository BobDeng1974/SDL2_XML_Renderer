#version 120

#define NUM_LIGHTS 3

struct Light{
	int type;
	vec3 pos;
	vec3 dir; // or attenuation
	vec3 intensity;
};

uniform Light L[NUM_LIGHTS];

uniform vec3 u_Color;

varying vec3 v_Nrm;
varying vec3 v_Pos;

void main(){
	// Compute total light contribution from all lights, all diffuse for now
	vec3 light = vec3(0,0,0);
	for (int i = 0; i < NUM_LIGHTS; i++){
		if (L[i].type == 0) // directional
		{
			float nDotVP = max(0, dot(v_Nrm, L[i].dir));
			light += nDotVP * L[i].intensity;
		}
		else if (L[i].type == 1){ // point
			vec3 VP = (L[i].pos - v_Pos);
			float d = length(VP);
			float nDotVP = max(0, dot(v_Nrm, normalize(VP)));
			float attenuation = 1.0 /
				(L[i].dir[0] + L[i].dir[1] * d + L[i].dir[2] * d*d);
			light += L[i].intensity * attenuation * nDotVP;
		}
		else if (L[i].type == 2){ // ambient
			light += L[i].intensity;
		}
	}
	
	gl_FragColor = vec4(light * u_Color, 1);
}
