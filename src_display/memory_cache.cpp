#include "display/memory_cache.hpp"

#include <vector>
#include <stdint.h>
#include <stdexcept>
#include <GL/glew.h>
#include "drn/drn_reader.h"
#include "data_types.hpp"
#include "geom_types.hpp"

size_t initMemory(std::vector<Chunk>& memory, Leaf* leafArray, bool* loadedLeaf, uint32_t nbLeaves, uint16_t nbSub_lvl2, size_t chunkBytesSize, glm::mat4 V, double halfLeafSize){
	//if the memcache is to tiny for the chunks
	if(chunkBytesSize >= MAX_MEMORY_SIZE){
		throw std::logic_error("the memcache is to little compare with the chunk size");
	}
	
	//generate the VBO index array
	uint32_t nbVbo = MAX_MEMORY_SIZE/chunkBytesSize;
	GLuint* vbos = new GLuint[nbVbo];
	
	size_t currentMemSize = 0;
	uint32_t currentLeaf = 0;
	while(currentMemSize + chunkBytesSize < MAX_MEMORY_SIZE && currentLeaf < nbLeaves){
		glGenBuffers(1, &(vbos[currentLeaf]));
		loadInMemory(memory, leafArray[currentLeaf], currentLeaf, computeDistanceLeafCamera(leafArray[currentLeaf], V, halfLeafSize), nbSub_lvl2, vbos[currentLeaf]);
		loadedLeaf[currentLeaf] = true;
		currentLeaf++;
		currentMemSize+= chunkBytesSize; 
	}
	
	//free ressources
	delete[] vbos;
	
	return currentMemSize;
}

void loadInMemory(std::vector<Chunk>& memory, Leaf l, uint16_t l_idx, double distance, uint16_t nbSub_lvl2, GLuint idxVbo){
	drn_t cache;
	uint32_t test_cache = drn_open(&cache, "./voxels_data/voxel_intersec_1.data", DRN_READ_NOLOAD);
	if(test_cache <0){ throw std::runtime_error("unable to open data file"); }
	
	/* load the voxel data */
	VoxelData* voxArray = NULL;
	voxArray = new VoxelData[nbSub_lvl2*nbSub_lvl2*nbSub_lvl2];
	test_cache = drn_read_chunk(&cache, l.id, voxArray);
	if(test_cache <0){ throw std::runtime_error("unable to read data file"); }
	
	/* load the triangles and send them to th GPU */
	Vertex* vertices = NULL;
	vertices = new Vertex[l.nbVertices];
	test_cache = drn_read_chunk(&cache, l.id+1, vertices);
	glBindBuffer(GL_ARRAY_BUFFER, idxVbo);
		glBufferData(GL_ARRAY_BUFFER, l.nbVertices*sizeof(Vertex), vertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	delete[] vertices;
	
	test_cache = drn_close(&cache);
	if(test_cache <0){ throw std::runtime_error("unable to close data file"); }
	
	//add to map
	Chunk newChunk;
	newChunk.voxels = voxArray;
	newChunk.pos = l.pos;
	newChunk.idxLeaf = l_idx;
	newChunk.d = distance;
	newChunk.vbo = idxVbo;
	memory.push_back(newChunk);
}

GLuint freeInMemory(std::vector<Chunk>& memory, bool* loadedLeaf){
	Chunk lastElt = memory.back();
	GLuint freeVbo = lastElt.vbo;
	loadedLeaf[lastElt.idxLeaf] = false;
	delete[] lastElt.voxels;
	memory.pop_back();
	
	return freeVbo;
}

double computeDistanceLeafCamera(Leaf currentLeaf, glm::mat4& view, double halfLeafSize){
	glm::vec4 homogenPos = glm::vec4(currentLeaf.pos.x + halfLeafSize, currentLeaf.pos.y + halfLeafSize, currentLeaf.pos.z + halfLeafSize, 1.);
	return glm::length(view*homogenPos) -1;
}  
