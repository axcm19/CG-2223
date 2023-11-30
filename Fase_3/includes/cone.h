#ifndef _CONE_H_
#define _CONE_H_
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

void drawCone(float radius, float height, const int slices, int stacks, char* filename);  

#endif 
