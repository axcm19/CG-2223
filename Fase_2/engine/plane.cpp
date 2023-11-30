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

/*
void drawPlane(float length, int divisions, char* filename) {
	//ESTA FUNÇÃO SERVE APENAS PARA DEFINIR OS VÉRTICES QUE VÃO COMPOR A FIGURA DO PLANO E ESCREVE-LOS NO FICHEIRO plane.3d

	//glColor3f(1.0f, 1.0f, 1.0f);

	float h = length / 2;
	float l = length / divisions;
	float d = 0;
	float t = 0;
	FILE * f = std::fopen(filename, "w");


	//glBegin(GL_TRIANGLES);


	for (d = 0; d < divisions * l; d += l) {

		for (t = 0; t < divisions * l; t += l) {

			
			glColor3f(1.0f, 1.0f, 1.0f);

			glVertex3f(-h + t, 0.0f, -h + d);
			glVertex3f(-h + l + t, 0.0f, -h + d);
			glVertex3f(-h + l + t, 0.0f, -h + l + d);

			glVertex3f(-h + t, 0.0f, -h + d);
			glVertex3f(-h + t, 0.0f, -h + l + d);
			glVertex3f(-h + l + t, 0.0f, -h + l + d);
			
			//---------------------------------------------------------

			fprintf(f, "%f; %f; %f\n", -h + t, 0.0f, -h + d);
			fprintf(f, "%f; %f; %f\n", -h + l + t, 0.0f, -h + d);
			fprintf(f, "%f; %f; %f\n", -h + l + t, 0.0f, -h + l + d);

			fprintf(f, "%f; %f; %f\n", -h + t, 0.0f, -h + d);
			fprintf(f, "%f; %f; %f\n", -h + t, 0.0f, -h + l + d);
			fprintf(f, "%f; %f; %f\n", -h + l + t, 0.0f, -h + l + d);

		}
	}

	std::fclose(f);
	//glEnd();
}
*/


void drawPlane(const char* filename) {
	//ESTA FUNÇÃO DESENHA A FIGURA DO PLANO LENDO OS VÉRTICES QUE ESTÃO CONTIDOS NO FICHEIRO plane.3d

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

	// put axis drawing in here
	glBegin(GL_LINES);
	// X axis in Red
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


	// put the geometric transformations here


	// put box drawing instructions here
	drawPlane("plane.3d");


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
	glutCreateWindow("GC_TP_plane_3d");

	// Required callback registry 
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);

	// Callback registration for keyboard processing
	glutKeyboardFunc(processKeys);
	glutSpecialFunc(processSpecialKeys);

	//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// enter GLUT's main cycle
	glutMainLoop();

	return 1;
}


//------------------------------------------------------------------------------------------------------------------------------------
