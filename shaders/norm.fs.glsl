#version 330

#define SKYBOX 0
#define TRIANGLES 1

#define NORMAL 2
#define BENDING 3
#define DRAIN 4
#define GRADIENT 5
#define SURFACE 6

#define VEGET 7

in vec3 gNormal;
in vec2 gTexCoords;
in float gBending;
in float gDrain;
in float gGradient;
in float gSurface;

uniform vec3 uLightVect = vec3(0.,0.,0.);
uniform sampler2D uTexture;
uniform sampler2D uGrassTex;
uniform sampler2D uWaterTex;
uniform int uMode;
uniform int uChoice;
uniform float uMaxBending = 0;
uniform float uMaxDrain = 0;
uniform float uMaxGradient = 0;
uniform float uMaxSurface = 0;

out vec4 fFragColor;

void main() {
	if(uMode == TRIANGLES){
	
		/* ratios */
		float ratio;
		if(uMaxDrain == 0){
			ratio = 0;
		}else{
			ratio = gDrain/uMaxDrain;
		}
		vec3 dColor = (1.-ratio)*texture(uGrassTex, gTexCoords).rgb + ratio*texture(uWaterTex, gTexCoords).rgb;

		if(uChoice == BENDING){
			ratio = gBending/uMaxBending;
			dColor = vec3(1.f - ratio, ratio, 1.f - ratio);
		}
		else if(uChoice == DRAIN){
			dColor = vec3(1.f - ratio, 1.f - ratio, ratio);
		}
		else if(uChoice == GRADIENT){
			ratio = gGradient/uMaxGradient;
			dColor = vec3(ratio, 1.f - ratio, 1.f - ratio);
		}
		else if(uChoice == SURFACE){
			ratio = gSurface/uMaxSurface;
			dColor = vec3(0.5f - ratio, ratio, 0.5f - ratio);
		}
	
		float dCoeff = max(0, dot(normalize(gNormal), -normalize(uLightVect)));

		vec3 aColor = vec3(0.1f, 0.1f, 0.1f);
		vec3 color = vec3(0.8f, 0.8f, 0.8f) * (aColor + dColor*dCoeff);
		
		fFragColor = vec4(color, 1.f);

		if(uChoice == VEGET){
			vec4 texel = texture(uTexture, gTexCoords);
				if(texel.a <0.5){
					discard;
			}
			fFragColor = texel;
		}	

	}
	else if(uMode == SKYBOX){
		fFragColor = texture(uTexture, gTexCoords);
	}
}
