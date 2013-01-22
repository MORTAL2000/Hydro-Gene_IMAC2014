#ifndef _IMAC2GL3_SHADER_TOOLS_HPP_
#define _IMAC2GL3_SHADER_TOOLS_HPP_

#include <GL/glew.h>

namespace hydrogene {

// Charge, compile et link les fichier sources GLSL passés en paramètre. Renvoit l'identifiant du programme compilé.
GLuint loadProgram(const char* vertexShaderFile, const char* fragmentShaderFile, const char* geometryShaderFile);

}

#endif
