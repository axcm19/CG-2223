#ifndef _PLANE_H_
#define _PLANE_H_
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

void drawPlane(float length, int divisions, char* filename);
    
#endif 