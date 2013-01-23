#version 330

#define SKYBOX 0
#define TRIANGLES 1
#define VEGET 7

layout(triangles) in;
layout(triangle_strip, max_vertices=5) out;

in vec3 vNormal[];
in vec2 vTexCoords[];
in float vBending[];
in float vDrain[];
in float vGradient[];
in float vSurface[];

uniform mat4 uMVPMatrix = mat4(1.f);
uniform int uMode;
uniform int uChoice;

out vec3 gNormal;
out vec2 gTexCoords;
out float gBending;
out float gDrain;
out float gGradient;
out float gSurface;

void main(){	
	if(uMode == SKYBOX){
	  for(int i=0; i<gl_in.length(); i++){
			gTexCoords = vTexCoords[i];
		  gl_Position = gl_in[i].gl_Position;
	 	  EmitVertex();
		}
	 	EndPrimitive();
	}
	else if(uMode == TRIANGLES){
		if(uChoice == VEGET){
			for(int i=0; i<gl_in.length(); i++){
				gl_Position = gl_in[i].gl_Position;
			  gTexCoords = vec2(0.0, 1.0);
		 	  EmitVertex();
				gl_Position.x = gl_in[i].gl_Position.x;
				gl_Position.y = gl_in[i].gl_Position.y + 0.1f;
			  gTexCoords = vec2(0.0, 0.0);
		 	  EmitVertex();
				gl_Position.x = gl_in[i].gl_Position.x + 0.05f;
				gl_Position.y = gl_in[i].gl_Position.y + 0.1f;
			  gTexCoords = vec2(1.0, 0.0);
		 	  EmitVertex();
				gl_Position.x = gl_in[i].gl_Position.x + 0.05f;
				gl_Position.y = gl_in[i].gl_Position.y;
			  gTexCoords = vec2(1.0, 1.0);
		 	  EmitVertex();
				gl_Position.x = gl_in[i].gl_Position.x;
				gl_Position.y = gl_in[i].gl_Position.y;
			  gTexCoords = vec2(0.0, 1.0);
		 	  EmitVertex();

		 	}
		 	EndPrimitive();
		}
		else{
			for(int i=0; i<gl_in.length(); i++){
				gNormal = vNormal[i];
				gBending = vBending[i];
				gDrain = vDrain[i];
				gGradient = vGradient[i];
				gSurface = vSurface[i];
				gl_Position = gl_in[i].gl_Position;
		 	  EmitVertex();

		 	}
		 	EndPrimitive();
		}
	}
}