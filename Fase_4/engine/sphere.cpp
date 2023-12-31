#ifdef __APPLE__
#include <GLUT/glut.h>
#else
//#include <Windows.h>
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <iostream>
#include <stdio.h>

using namespace std;

//------------------------------------------------------------------------------------------------------------------------------------


void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if (h == 0)
		h = 1;

	// compute window's aspect ratio 
	float ratio = w * 1.0 / h;

	// Set the projection matrix as current
	glMatrixMode(GL_PROJECTION);
	// Load Identity Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set perspective
	gluPerspective(45.0f, ratio, 1.0f, 1000.0f);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}


//------------------------------------------------------------------------------------------------------------------------------------


void drawSphere(const char* filename) {
	//ESTA FUNÇÃO DESENHA A FIGURA DA ESFERA LENDO OS VÉRTICES QUE ESTÃO CONTIDOS NO FICHEIRO sphere.3d

	glColor3f(1.0f, 1.0f, 1.0f);

	//FILE* f = std::fopen(filename, "r");
	ifstream myFileStream(filename);

	if (!myFileStream.is_open()) {
		cout << "Error" << endl;
	}

	string read_x, read_y, read_z;

	string line;

	glBegin(GL_TRIANGLES);

		while (getline(myFileStream, line)) {
		//-------------------------------------------------
		//ESTE PEDAÇO DE CODIGO COLOCA A INFORMAÇÃO EXTRAIDA
		//DA LINHA E CRIA UM VÉRTICE

			stringstream ss(line);
			getline(ss, read_x, ';');
			getline(ss, read_y, ';');
			getline(ss, read_z, ';');

			float x = (float)stoi(read_x);
			float y = (float)stoi(read_y);
			float z = (float)stoi(read_z);

			glVertex3f(x, y, z);


		//-------------------------------------------------

		}

		myFileStream.close();
	glEnd();
	

}


//------------------------------------------------------------------------------------------------------------------------------------


void renderScene(void) {

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();
	gluLookAt(5.0, 5.0, 5.0,
		0.0, 0.0, 0.0,
		0.0f, 1.0f, 0.0f);

	glBegin(GL_LINES);
		// X axis in red
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(-100.0f, 0.0f, 0.0f);
		glVertex3f(100.0f, 0.0f, 0.0f);
		// Y Axis in Green
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, -100.0f, 0.0f);
		glVertex3f(0.0f, 100.0f, 0.0f);
		// Z Axis in Blue
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, -100.0f);
		glVertex3f(0.0f, 0.0f, 100.0f);
	glEnd();

	//put cone drawing instructions here
	drawSphere("sphere.3d");

	// End of frame
	glutSwapBuffers();
}


//------------------------------------------------------------------------------------------------------------------------------------


void processKeys(unsigned char c, int xx, int yy) {

	// put code to process regular keys in here

}


//------------------------------------------------------------------------------------------------------------------------------------


void processSpecialKeys(int key, int xx, int yy) {

	// put code to process special keys in here

}


//------------------------------------------------------------------------------------------------------------------------------------


int main(int argc, char** argv) {

	// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("CG_TP_cone_3d");

	// Required callback registry 
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);


	// put here the registration of the keyboard callbacks



	//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// enter GLUT's main cycle
	glutMainLoop();

	return 1;
}


//------------------------------------------------------------------------------------------------------------------------------------