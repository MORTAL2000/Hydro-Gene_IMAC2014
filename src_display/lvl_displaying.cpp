#include "display/lvl_displaying.hpp"

#include <stdint.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "data_types.hpp"
#include "tools/MatrixStack.hpp"
#include "cameras/FreeFlyCamera.hpp"
#include "display/cube_model.hpp"

#define TRIANGLES 1
#define NORMAL 2
#define BENDING 3
#define DRAIN 4
#define GRADIENT 5
#define SURFACE 6
#define VEGET 7

void getLocations(GLint* locations, GLuint program){
	/* Matrices */
	locations[MVP] = glGetUniformLocation(program, "uMVPMatrix");
	locations[MODELVIEW] = glGetUniformLocation(program, "uModelView");
	locations[INV_VIEWMATRIX] = glGetUniformLocation(program, "uInvViewMatrix");
	
	/* Light */
	locations[SUNDIR] = glGetUniformLocation(program, "uSunDir");

	/* Time */
	locations[WATERTIME] = glGetUniformLocation(program, "uWaterTime");
	locations[MOVEWATERTIME] = glGetUniformLocation(program, "uMoveWaterTime");
	locations[CLOUDSTIME] = glGetUniformLocation(program, "uCloudsTime");

	/* Shaders modes */
	locations[MODE] = glGetUniformLocation(program, "uMode");
	locations[CHOICE] = glGetUniformLocation(program, "uChoice");
	
	/* Controlers  */
	locations[FOG] = glGetUniformLocation(program, "uFog");
	locations[OCEAN] = glGetUniformLocation(program, "uOcean");
	/* Textures */
	locations[SKYTEX] = glGetUniformLocation(program, "uSkyTex");
	locations[ENVMAPTEX] = glGetUniformLocation(program, "uEnvmapTex");
	locations[CLOUDTEX] = glGetUniformLocation(program, "uCloudTex");
	locations[GRASSTEX] = glGetUniformLocation(program, "uGrassTex");
	locations[WATERTEX] = glGetUniformLocation(program, "uWaterTex");
	locations[WATERGROUNDTEX] = glGetUniformLocation(program, "uWaterGroundTex");
	locations[STONETEX] = glGetUniformLocation(program, "uStoneTex");
	locations[SNOWTEX] = glGetUniformLocation(program, "uSnowTex");
	locations[SANDTEX] = glGetUniformLocation(program, "uSandTex");
	
	locations[ROCKTEX] = glGetUniformLocation(program, "uRockTex");
	locations[PLANTTEX] = glGetUniformLocation(program, "uPlantTex");
	locations[TREETEX] = glGetUniformLocation(program, "uTreeTex");
	locations[PINETREETEX] = glGetUniformLocation(program, "uPineTreeTex");
	locations[SNOWTREETEX] = glGetUniformLocation(program, "uSnowTreeTex");
	
	/* Max properties */
	locations[MAXBENDING] = glGetUniformLocation(program, "uMaxBending");
	locations[MAXDRAIN] = glGetUniformLocation(program, "uMaxDrain");
	locations[MAXGRADIENT] = glGetUniformLocation(program, "uMaxGradient");
	locations[MAXSURFACE] = glGetUniformLocation(program, "uMaxSurface");
	locations[MAXALTITUDE] = glGetUniformLocation(program, "uMaxAltitude");
	
	/* Vegetation */
	locations[DISTANCE] = glGetUniformLocation(program, "uDistance");
	
	/* Terrain */
	locations[TERR_SCALE] = glGetUniformLocation(program, "uTerrainScale");
	
	/* Cameras */
	locations[FF_FRONT_VECTOR] = glGetUniformLocation(program, "uFrontVector");
}

void sendUniforms(GLint* locations, float* maxCoeffArray, float thresholdDistance, float terrainScale){
	/**** SEND ****/
	glUniform1f(locations[MAXBENDING], maxCoeffArray[0]);	
	glUniform1f(locations[MAXDRAIN], maxCoeffArray[1]);
	glUniform1f(locations[MAXGRADIENT], maxCoeffArray[2]);
	glUniform1f(locations[MAXSURFACE], maxCoeffArray[3]);
	glUniform1f(locations[MAXALTITUDE], maxCoeffArray[4]);
	
	// Send terrain textures
	glUniform1i(locations[GRASSTEX], 0);
	glUniform1i(locations[WATERTEX], 1);
	glUniform1i(locations[STONETEX], 2);
	glUniform1i(locations[SNOWTEX], 3);
	glUniform1i(locations[SANDTEX], 4);
	glUniform1i(locations[WATERGROUNDTEX], 5);
	//Send sky textures
	glUniform1i(locations[SKYTEX], 6);
	glUniform1i(locations[ENVMAPTEX], 7);
	glUniform1i(locations[CLOUDTEX], 8);
	
	// Send details textures
	glUniform1i(locations[ROCKTEX], 0);
	glUniform1i(locations[PLANTTEX], 1);
	glUniform1i(locations[TREETEX], 2);
	glUniform1i(locations[PINETREETEX], 3);
	glUniform1i(locations[SNOWTREETEX], 4);
	
	glUniform1i(locations[DISTANCE], thresholdDistance);
	glUniform1f(locations[TERR_SCALE], terrainScale);
}

void display_lvl2(GLuint cubeVAO, MatrixStack& ms, GLuint MVPLocation, GLint NbIntersectionLocation, GLint NormSumLocation, uint32_t nbIntersectionMax, uint32_t nbVertices, VoxelData* voxArray, Leaf& l, uint16_t nbSub, double cubeSize, FreeFlyCamera& ffCam, CamType camType){
	for(uint32_t k=0;k<nbSub;++k){
		for(uint32_t j=0;j<nbSub;++j){
			for(uint32_t i=0;i<nbSub;++i){
				uint32_t currentIndex = k*nbSub*nbSub + j*nbSub + i;
				uint32_t currentNbIntersection = voxArray[k*nbSub*nbSub + j*nbSub + i].nbFaces;
				if(currentNbIntersection != 0){
					if(camType == FREE_FLY){
						if(frustumTest(l, i, j, k, cubeSize, ffCam)){
							ms.push();
								ms.translate(glm::vec3(i*cubeSize + l.pos.x, j*cubeSize + l.pos.y, k*cubeSize + l.pos.z)); //PLACEMENT OF EACH GRID CUBE
								ms.scale(glm::vec3(cubeSize));// RE-SCALE EACH GRID CUBE
							
								glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, glm::value_ptr(ms.top()));
								glUniform2i(NbIntersectionLocation, currentNbIntersection, nbIntersectionMax);
								glUniform3f(NormSumLocation, voxArray[currentIndex].sumNormal.x, voxArray[currentIndex].sumNormal.y, voxArray[currentIndex].sumNormal.z);
								glBindVertexArray(cubeVAO);
									glDrawArrays(GL_TRIANGLES, 0, nbVertices);
								glBindVertexArray(0);
							ms.pop();
						}
					}else{
						ms.push();
							ms.translate(glm::vec3(i*cubeSize + l.pos.x, j*cubeSize + l.pos.y, k*cubeSize + l.pos.z)); //PLACEMENT OF EACH GRID CUBE
							ms.scale(glm::vec3(cubeSize));// RE-SCALE EACH GRID CUBE
						
							glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, glm::value_ptr(ms.top()));
							glUniform2i(NbIntersectionLocation, currentNbIntersection, nbIntersectionMax);
							glUniform3f(NormSumLocation, voxArray[currentIndex].sumNormal.x, voxArray[currentIndex].sumNormal.y, voxArray[currentIndex].sumNormal.z);
							glBindVertexArray(cubeVAO);
								glDrawArrays(GL_TRIANGLES, 0, nbVertices);
							glBindVertexArray(0);
						ms.pop();
					}
				}
			}
		}
	}
}

void display_lvl1(GLuint cubeVAO, MatrixStack& ms, GLuint MVPLocation, glm::dvec3 pos, double halfLeafSize){
	ms.push();
		ms.translate(glm::vec3(pos.x + halfLeafSize, pos.y + halfLeafSize, pos.z + halfLeafSize)); //PLACEMENT OF THE LEAF
		ms.scale(glm::vec3(2*halfLeafSize));// RE-SCALE EACH GRID CUBE
	
		glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, glm::value_ptr(ms.top()));
	
		glBindVertexArray(cubeVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	ms.pop();
}

void display_triangle(GLuint meshVAO, MatrixStack& ms, GLuint MVPLocation, uint32_t nbVertices, GLuint* textures){
	glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, glm::value_ptr(ms.top()));
	BindTexture(textures[0], GL_TEXTURE0);
	BindTexture(textures[1], GL_TEXTURE1);
	BindTexture(textures[2], GL_TEXTURE2);
	BindTexture(textures[3], GL_TEXTURE3);
	BindTexture(textures[4], GL_TEXTURE4);
	BindTexture(textures[6], GL_TEXTURE8);
		glBindVertexArray(meshVAO);
			glDrawArrays(GL_TRIANGLES, 0, nbVertices);
		glBindVertexArray(0);
	BindTexture(0, GL_TEXTURE0);
	BindTexture(0, GL_TEXTURE1);
	BindTexture(0, GL_TEXTURE2);
	BindTexture(0, GL_TEXTURE3);
	BindTexture(0, GL_TEXTURE4);
	BindTexture(0, GL_TEXTURE8);
}

void display_vegetation(GLuint meshVAO, MatrixStack& ms, GLuint MVPLocation, uint32_t nbVertices, GLint ChoiceLocation, GLuint* textures){
	glUniform1i(ChoiceLocation, VEGET);
	glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, glm::value_ptr(ms.top()));
	BindTexture(textures[0], GL_TEXTURE0);
	BindTexture(textures[1], GL_TEXTURE1);
	BindTexture(textures[2], GL_TEXTURE2);
	BindTexture(textures[3], GL_TEXTURE3);
	BindTexture(textures[4], GL_TEXTURE4);
		glBindVertexArray(meshVAO);
			glDrawArrays(GL_TRIANGLES, 0, nbVertices);
		glBindVertexArray(0);
	BindTexture(0, GL_TEXTURE0);
	BindTexture(0, GL_TEXTURE1);
	BindTexture(0, GL_TEXTURE2);
	BindTexture(0, GL_TEXTURE3);
	BindTexture(0, GL_TEXTURE4);
}

bool frustumTest(Leaf& l, uint32_t i, uint32_t j, uint32_t k, double cubeSize, FreeFlyCamera& ffCam){
	
	if(glm::dot(ffCam.m_frustumNearPlaneNormal, (ffCam.m_frustumNearPlanePoint - glm::vec3(l.pos.x + i*cubeSize + cubeSize/2., l.pos.y + j*cubeSize + cubeSize/2., l.pos.z + k*cubeSize + cubeSize/2.))) < 0.){
		return false;
	}
	if(glm::dot(ffCam.m_frustumRightPlaneNormal, (glm::vec3(l.pos.x + i*cubeSize + cubeSize/2., l.pos.y + j*cubeSize + cubeSize/2., l.pos.z + k*cubeSize + cubeSize/2.) - ffCam.m_frustumRightPlanePoint)) < 0.){
		return false;
	}
	if(glm::dot(ffCam.m_frustumLeftPlaneNormal, (glm::vec3(l.pos.x + i*cubeSize + cubeSize/2., l.pos.y + j*cubeSize + cubeSize/2., l.pos.z + k*cubeSize + cubeSize/2.) - ffCam.m_frustumLeftPlanePoint)) < 0.){
		return false;
	}
	if(glm::dot(ffCam.m_frustumTopPlaneNormal, (glm::vec3(l.pos.x + i*cubeSize + cubeSize/2., l.pos.y + j*cubeSize + cubeSize/2., l.pos.z + k*cubeSize + cubeSize/2.) - ffCam.m_frustumTopPlanePoint)) < 0.){
		return false;
	}
	if(glm::dot(ffCam.m_frustumBottomPlaneNormal, (glm::vec3(l.pos.x + i*cubeSize + cubeSize/2., l.pos.y + j*cubeSize + cubeSize/2., l.pos.z + k*cubeSize + cubeSize/2.) - ffCam.m_frustumBottomPlanePoint)) < 0.){
		return false;
	}
	
	return true;
}

/***** Time management *****/
bool timePauseTrigger(bool timePause){
	if(!timePause){
		timePause = true;
	}else{
		timePause = false;
	}
	return timePause;
}
