#ifndef _BOX_H_
#define _BOX_H_
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

void drawBox(float length, int divisions, char* filename);
    
#endif 