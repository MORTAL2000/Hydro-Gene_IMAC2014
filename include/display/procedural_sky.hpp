#ifndef __PROCEDURAL_SKY_HPP__
#define __PROCEDURAL_SKY_HPP__

#include <GL/glew.h>

/* Create FBO */
GLuint createFBO();

/* Test for dynamique texturing the sky */
void paintTheSky(GLuint skyFboID, GLuint texID, GLuint skyProgram, GLuint quadVAO);

#endif