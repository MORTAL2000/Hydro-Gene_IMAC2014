#version 330
#define SUN_RADIUS 0.05
#define HALO_RADIUS 0.07

in vec2 vPos;

uniform vec3 uPlanOr;
uniform vec3 uPlanU;
uniform vec3 uPlanV;
uniform vec3 uSunPos;
uniform float uTime;
uniform samplerCube uSkyTex;
uniform samplerCube uEnvmapTex;
uniform float uSampleStep;
uniform int uIsSkybox;
uniform int uIsInitialBlur;

out vec4 fFragColor;


vec3 mod289(vec3 x){
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 mod289(vec4 x){
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x){
  return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(vec4 r){
  return 1.79284291400159 - 0.85373472095314 * r;
}

vec3 fade(vec3 t){
  return t*t*t*(t*(t*6.0-15.0)+10.0);
}

// Classic Perlin noise
float cnoise(vec3 P){
  vec3 Pi0 = floor(P); // Integer part for indexing
  vec3 Pi1 = Pi0 + vec3(1.0); // Integer part + 1
  Pi0 = mod289(Pi0);
  Pi1 = mod289(Pi1);
  vec3 Pf0 = fract(P); // Fractional part for interpolation
  vec3 Pf1 = Pf0 - vec3(1.0); // Fractional part - 1.0
  vec4 ix = vec4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
  vec4 iy = vec4(Pi0.yy, Pi1.yy);
  vec4 iz0 = Pi0.zzzz;
  vec4 iz1 = Pi1.zzzz;

  vec4 ixy = permute(permute(ix) + iy);
  vec4 ixy0 = permute(ixy + iz0);
  vec4 ixy1 = permute(ixy + iz1);

  vec4 gx0 = ixy0 * (1.0 / 7.0);
  vec4 gy0 = fract(floor(gx0) * (1.0 / 7.0)) - 0.5;
  gx0 = fract(gx0);
  vec4 gz0 = vec4(0.5) - abs(gx0) - abs(gy0);
  vec4 sz0 = step(gz0, vec4(0.0));
  gx0 -= sz0 * (step(0.0, gx0) - 0.5);
  gy0 -= sz0 * (step(0.0, gy0) - 0.5);

  vec4 gx1 = ixy1 * (1.0 / 7.0);
  vec4 gy1 = fract(floor(gx1) * (1.0 / 7.0)) - 0.5;
  gx1 = fract(gx1);
  vec4 gz1 = vec4(0.5) - abs(gx1) - abs(gy1);
  vec4 sz1 = step(gz1, vec4(0.0));
  gx1 -= sz1 * (step(0.0, gx1) - 0.5);
  gy1 -= sz1 * (step(0.0, gy1) - 0.5);

  vec3 g000 = vec3(gx0.x,gy0.x,gz0.x);
  vec3 g100 = vec3(gx0.y,gy0.y,gz0.y);
  vec3 g010 = vec3(gx0.z,gy0.z,gz0.z);
  vec3 g110 = vec3(gx0.w,gy0.w,gz0.w);
  vec3 g001 = vec3(gx1.x,gy1.x,gz1.x);
  vec3 g101 = vec3(gx1.y,gy1.y,gz1.y);
  vec3 g011 = vec3(gx1.z,gy1.z,gz1.z);
  vec3 g111 = vec3(gx1.w,gy1.w,gz1.w);

  vec4 norm0 = taylorInvSqrt(vec4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
  g000 *= norm0.x;
  g010 *= norm0.y;
  g100 *= norm0.z;
  g110 *= norm0.w;
  vec4 norm1 = taylorInvSqrt(vec4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
  g001 *= norm1.x;
  g011 *= norm1.y;
  g101 *= norm1.z;
  g111 *= norm1.w;

  float n000 = dot(g000, Pf0);
  float n100 = dot(g100, vec3(Pf1.x, Pf0.yz));
  float n010 = dot(g010, vec3(Pf0.x, Pf1.y, Pf0.z));
  float n110 = dot(g110, vec3(Pf1.xy, Pf0.z));
  float n001 = dot(g001, vec3(Pf0.xy, Pf1.z));
  float n101 = dot(g101, vec3(Pf1.x, Pf0.y, Pf1.z));
  float n011 = dot(g011, vec3(Pf0.x, Pf1.yz));
  float n111 = dot(g111, Pf1);

  vec3 fade_xyz = fade(Pf0);
  vec4 n_z = mix(vec4(n000, n100, n010, n110), vec4(n001, n101, n011, n111), fade_xyz.z);
  vec2 n_yz = mix(n_z.xy, n_z.zw, fade_xyz.y);
  float n_xyz = mix(n_yz.x, n_yz.y, fade_xyz.x);
  return 2.2 * n_xyz;
}


// Hue - Saturation - Luminance to Red - Green - Blue model conversion
vec3 HSLtoRGB(int h, float s, float l){
	vec3 color;	
	float c = (1. - abs(2 * l - 1.)) * s;
	float m = 1. * (l - 0.5 * c);
	float x = c * (1. - abs(mod(h / 60., 2) - 1.));
	
	h = h%360;
	if(h >= 0 && h < 60){ color = vec3(c + m, x + m, m); }
	else if(h >= 60 && h < 120){ color = vec3(x + m, c + m, m);	}
	else if(h >= 120 && h < 180){ color = vec3(m, c + m, x + m); }
	else if(h >= 180 && h < 240){ color = vec3(m, x + m, c + m); }
	else if(h >= 240 && h < 300){ color = vec3(x + m, m, c + m); }
	else if(h >= 300 && h < 360){ color = vec3(c + m, m, x + m); }
	else{ color = vec3(m, m, m); }
	return color;
}

vec3 getBluredTexel(vec3 position){
	vec3 color = vec3(0.f);
	int nbIt = 0;
	int blurWidth = 1;
	//~ /* case of X-Y plane */
	if(uPlanU.z == 0 && uPlanV.z == 0){
		float blurBeginX = position.x-blurWidth*uSampleStep;
		float blurBeginY = position.y-blurWidth*uSampleStep;
		float blurEndX = position.x+(blurWidth+1)*uSampleStep;
		float blurEndY = position.y+(blurWidth+1)*uSampleStep;
		for(float i=blurBeginX;i<blurEndX;i+=uSampleStep){
			for(float j=blurBeginY;j<blurEndY;j+=uSampleStep){
				color+= texture(uEnvmapTex, vec3(i, j, position.z)).rgb;
				++nbIt;
			}
		}
	}
	/* case of Z-Y plane */
	else if(uPlanU.x == 0 && uPlanV.x == 0){
		float blurBeginZ = position.z-blurWidth*uSampleStep;
		float blurBeginY = position.y-blurWidth*uSampleStep;
		float blurEndZ = position.z+(blurWidth+1)*uSampleStep;
		float blurEndY = position.y+(blurWidth+1)*uSampleStep;
		for(float k=blurBeginZ;k<blurEndZ;k+=uSampleStep){
			for(float j=blurBeginY;j<blurEndY;j+=uSampleStep){
				color+= texture(uEnvmapTex, vec3(position.x, j, k)).rgb;
				++nbIt;
			}
		}
	}
	/* case of X-Z plane */
	else{
		float blurBeginX = position.x-blurWidth*uSampleStep;
		float blurBeginZ = position.z-blurWidth*uSampleStep;
		float blurEndX = position.x+(blurWidth+1)*uSampleStep;
		float blurEndZ = position.z+(blurWidth+1)*uSampleStep;
		for(float i=blurBeginX;i<blurEndX;i+=uSampleStep){
			for(float k=blurBeginZ;k<blurEndZ;k+=uSampleStep){
				color+= texture(uEnvmapTex, vec3(i, position.y, k)).rgb;
				++nbIt;
			}
		}
	}
	color /= nbIt;
	return color;
}

void main(){
	vec3 absolutePos = normalize(uPlanOr + vPos.x*uPlanU + vPos.y*uPlanV);
	
	//Draw the skybox
	if(uIsSkybox == 1){
		float time = uTime*0.125f;
		float sunY = (uSunPos.y+1.)*0.5;
		float sunX = (uSunPos.x+1.)*0.5;
		float normPosX = (absolutePos.x+1)*0.5;
		float distanceToSun = distance(absolutePos, uSunPos);
		float satGradient = 0.18*pow((1. - absolutePos.y), 2);
		float lighnessGradient = 0.28 + 0.31*pow((1. - absolutePos.y), 2);

		/* sky color */
		vec3 skyColor;
		skyColor.x = 209;
		skyColor.y = 0.76 + satGradient;
		skyColor.z = 0.1 + lighnessGradient*((2-distanceToSun*0.5)*sunY);
		
		/* dawn effect */
		if(sunX > 0.9){
			vec3 dawnColor;
			dawnColor.x = 359;
			dawnColor.y = 0.82+satGradient;
			dawnColor.z = 0.325+lighnessGradient;
			float dawnCoef = (1-(1-sunX)/0.1)*max(0., normPosX);
			skyColor = mix(skyColor, dawnColor, dawnCoef);
		}
		
		/* twillight effect */
		if(sunX < 0.1){
			vec3 twillightColor;
			twillightColor.x = 384;
			twillightColor.y = 0.8+satGradient;
			twillightColor.z = 0.165+lighnessGradient;
			float twillightCoef = (1-(sunX)/0.1)*max(0., 1-normPosX);
			skyColor = mix(skyColor, twillightColor, twillightCoef);
		}
		
		/* stars noise */
		float starsCoef = 0.f;
		if(absolutePos.y > 0.f && sunY < 0.6){
			starsCoef = cnoise(absolutePos*100);
			if(starsCoef < 0.99f) starsCoef *= pow(0.5f, (1.f-starsCoef)*15.f);
		}

		/* day */
		if(distanceToSun <= SUN_RADIUS){
			skyColor.z = 1;
		}else if(distanceToSun <= HALO_RADIUS){
			skyColor.z += (1-skyColor.z)*pow((1-((distanceToSun-SUN_RADIUS)/(HALO_RADIUS-SUN_RADIUS))), 3);
		}


		/* clouds noise */
		// where we draw clouds
		float cloudCoef = 0.f;
		if(absolutePos.y > 0.f && sunY > 0.2){
			float cloudZone = ((cnoise((absolutePos+time)*2)+1.)/2.)*0.9;
			if(cloudZone < 0.f) cloudZone = 0.f;

			// clouds noise inside this zone
			float posX = absolutePos.x + time*2;
			cloudCoef = cnoise(vec3(posX*2., (1.-absolutePos.y)*4., absolutePos.z*0.75)*2)*3;
			cloudCoef = (cloudCoef + 2.f)*0.3f;

			cloudCoef *= cloudZone;
			if(cloudCoef < 0.f) cloudCoef = 0.f;

			/* no clouds on the horizon */
			if(absolutePos.y < 0.1){
				cloudCoef = cloudCoef*(absolutePos.y*10);
			}
		}
		
		float cloudTempo = max(sunY-0.2,0)/0.8; //sun position influence the density of clouds and stars
		float starsTempo = max((1.-sunY)-0.4, 0)/0.6;
		
		fFragColor = vec4( mix(HSLtoRGB(int(skyColor.x), skyColor.y, skyColor.z), vec3(1.f), cloudCoef*cloudTempo), 1.f );
		fFragColor = mix( fFragColor, vec4(1.), starsCoef*starsTempo);
		
		//~ test skybox
		//~ vec3 testColor = vec3(0.f);
		//~ if(absolutePos.x >= 0.) testColor.r = 1.f;
		//~ else testColor.g = 1.f;
		//~ if(absolutePos.z >= 0.) testColor.b = 1.;
		//~ fFragColor = vec4(testColor, 1.f);
	}
	//Draw the envmap
	else{
		if(uIsInitialBlur == 1){
			fFragColor = texture(uSkyTex, absolutePos);
		}else{
			fFragColor = vec4(getBluredTexel(absolutePos), 1.f);
		}
	}
}
