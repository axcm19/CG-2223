#include <string>
#if _WIN32
#include <Windows.h>
#endif
#if __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

//const char* relative_path = "/home/tomas/Desktop/CG_2/TP23/3d_files/";
const char* relative_path = "../3d_files/";

//------------------------------------------------------------------------------------------------------------------------------------

void generateSphere(float radius, const int slices, int stacks, char* filename) {
	//ESTA FUNÇÃO SERVE APENAS PARA DEFINIR OS VÉRTICES QUE VÃO COMPOR A FIGURA DA ESFERA E ESCREVE-LOS NO FICHEIRO sphere.3d

	string file;
	file.append(relative_path).append(filename);
	FILE * f = fopen(file.c_str(), "w+");

	fprintf(f,"sphere\n");

	fclose(f);
}


//------------------------------------------------------------------------------------------------------------------------------------


void generateCone(float radius, float height, int slices, int stacks, char* filename) {
	//ESTA FUNÇÃO SERVE APENAS PARA DEFINIR OS VÉRTICES QUE VÃO COMPOR A FIGURA DO CONE E ESCREVE-LOS NO FICHEIRO cone.3d

	string file;
	file.append(relative_path).append(filename);
	FILE * f = fopen(file.c_str(), "w+");

	//fprintf(f,"cone\n");

	//glBegin(GL_TRIANGLES);

		float angle = 2 * M_PI / slices;

		for (int i = 0; i < slices; i++) {

			//gera 2 triangulos de cada vez
			//glColor3f(1.0f, 1.0f, 1.0f);


			//base de baixo
			fprintf(f, "%f; %f; %f\n", radius * sin(angle * (i + 1)), 0.0f, radius * cos(angle * (i + 1)));
			fprintf(f, "%f; %f; %f\n", radius * sin(angle * i), 0.0f, radius * cos(angle * i));
			fprintf(f, "%f; %f; %f\n", 0.0f, 0.0f, 0.0f);

			//um triangulo para cada slice
			fprintf(f, "%f; %f; %f\n", 0.0f, height, 0.0f);
			fprintf(f, "%f; %f; %f\n", radius * sin(angle * i), 0.0f, radius * cos(angle * i));
			fprintf(f, "%f; %f; %f\n", radius * sin(angle * (i + 1)), 0.0f, radius * cos(angle * (i + 1)));

		}

	//glEnd();
	fclose(f);
}


//------------------------------------------------------------------------------------------------------------------------------------


void generateBox(float length, int divisions, char* filename) {
	//ESTA FUNÇÃO SERVE APENAS PARA DEFINIR OS VÉRTICES QUE VÃO COMPOR A FIGURA DA CAIXA E ESCREVE-LOS NO FICHEIRO box.3d
	
	//glColor3f(1.0f, 1.0f, 1.0f);

	float h = length / 2;
	float l = length / divisions;
	float d = 0;
	float t = 0;
	
	string file;
	file.append(relative_path).append(filename);
	FILE * f = fopen(file.c_str(), "w+");

	//fprintf(f,"box\n");

	//glBegin(GL_TRIANGLES);

	//base de baixo
	for (d = 0; d < divisions * l; d += l) {

		for (t = 0; t < divisions * l; t += l) {

			fprintf(f, "%f; %f; %f\n", -h + t, -h, -h + d);
			fprintf(f, "%f; %f; %f\n", -h + l + t, -h, -h + d);
			fprintf(f, "%f; %f; %f\n", -h + l + t, -h, -h + l + d);

			fprintf(f, "%f; %f; %f\n", -h + t, -h, -h + d);
			fprintf(f, "%f; %f; %f\n", -h + t, -h, -h + l + d);
			fprintf(f, "%f; %f; %f\n", -h + l + t, -h, -h + l + d);

		}
	}

	//base de cima
	for (d = 0; d < divisions * l; d += l) {

		for (t = 0; t < divisions * l; t += l) {

			//base de cima
			fprintf(f, "%f; %f; %f\n", -h + t, h, -h + d);
			fprintf(f, "%f; %f; %f\n", -h + l + t, h, -h + d);
			fprintf(f, "%f; %f; %f\n", -h + l + t, h, -h + l + d);

			fprintf(f, "%f; %f; %f\n", -h + t, h, -h + d);
			fprintf(f, "%f; %f; %f\n", -h + t, h, -h + l + d);
			fprintf(f, "%f; %f; %f\n", -h + l + t, h, -h + l + d);

		}
	}


	//base 1 (x sempre igual)
	for (d = 0; d < divisions * l; d += l) {

		for (t = 0; t < divisions * l; t += l) {

			//base de cima
			fprintf(f, "%f; %f; %f\n", h, -h + t, -h + d);
			fprintf(f, "%f; %f; %f\n", h, -h + l + t, -h + d);
			fprintf(f, "%f; %f; %f\n", h, -h + l + t, -h + l + d);

			fprintf(f, "%f; %f; %f\n", h, -h + t, -h + d);
			fprintf(f, "%f; %f; %f\n", h, -h + t, -h + l + d);
			fprintf(f, "%f; %f; %f\n", h, -h + l + t, -h + l + d);

		}
	}

	//base 2 (x sempre igual, oposto)
	for (d = 0; d < divisions * l; d += l) {

		for (t = 0; t < divisions * l; t += l) {

			//gera oito triangulos de cada vez
			//glColor3f(1.0f, 1.0f, 1.0f);

			//base de cima
			fprintf(f, "%f; %f; %f\n", -h, -h + t, -h + d);
			fprintf(f, "%f; %f; %f\n", -h, -h + l + t, -h + d);
			fprintf(f, "%f; %f; %f\n", -h, -h + l + t, -h + l + d);

			fprintf(f, "%f; %f; %f\n", -h, -h + t, -h + d);
			fprintf(f, "%f; %f; %f\n", -h, -h + t, -h + l + d);
			fprintf(f, "%f; %f; %f\n", -h, -h + l + t, -h + l + d);

		}
	}


	//base 3 (z sempre igual)
	for (d = 0; d < divisions * l; d += l) {

		for (t = 0; t < divisions * l; t += l) {

			//base de cima
			fprintf(f, "%f; %f; %f\n", -h + t, -h + d, h);
			fprintf(f, "%f; %f; %f\n", -h + l + t, -h + d, h);
			fprintf(f, "%f; %f; %f\n", -h + l + t, -h + l + d, h);

			fprintf(f, "%f; %f; %f\n", -h + t, -h + d, h);
			fprintf(f, "%f; %f; %f\n", -h + t, -h + l + d, h);
			fprintf(f, "%f; %f; %f\n", -h + l + t, -h + l + d, h);

		}
	}


	//base 4 (z sempre igual, oposto)
	for (d = 0; d < divisions * l; d += l) {

		for (t = 0; t < divisions * l; t += l) {

			//base de cima
			fprintf(f, "%f; %f; %f\n", -h + t, -h + d, -h);
			fprintf(f, "%f; %f; %f\n", -h + l + t, -h + d, -h);
			fprintf(f, "%f; %f; %f\n", -h + l + t, -h + l + d, -h);

			fprintf(f, "%f; %f; %f\n", -h + t, -h + d, -h);
			fprintf(f, "%f; %f; %f\n", -h + t, -h + l + d, -h);
			fprintf(f, "%f; %f; %f\n", -h + l + t, -h + l + d, -h);

		}
	}

	//glEnd();
	fclose(f);

}


//------------------------------------------------------------------------------------------------------------------------------------


void generatePlane(float length, int divisions, char* filename) {
	//ESTA FUNÇÃO SERVE APENAS PARA DEFINIR OS VÉRTICES QUE VÃO COMPOR A FIGURA DO PLANO E ESCREVE-LOS NO FICHEIRO plane.3d

	//glColor3f(1.0f, 1.0f, 1.0f);

	float h = length / 2;
	float l = length / divisions;
	float d = 0;
	float t = 0;
	
	string file;
	file.append(relative_path).append(filename);
	FILE * f = fopen(file.c_str(), "w+");

	//fprintf(f,"plane\n");

	//glBegin(GL_TRIANGLES);


	for (d = 0; d < divisions * l; d += l) {

		for (t = 0; t < divisions * l; t += l) {

			/*
			glColor3f(1.0f, 1.0f, 1.0f);

			glVertex3f(-h + t, 0.0f, -h + d);
			glVertex3f(-h + l + t, 0.0f, -h + d);
			glVertex3f(-h + l + t, 0.0f, -h + l + d);

			glVertex3f(-h + t, 0.0f, -h + d);
			glVertex3f(-h + t, 0.0f, -h + l + d);
			glVertex3f(-h + l + t, 0.0f, -h + l + d);
			*/
			//---------------------------------------------------------

			fprintf(f, "%f; %f; %f\n", -h + t, 0.0f, -h + d);
			fprintf(f, "%f; %f; %f\n", -h + l + t, 0.0f, -h + d);
			fprintf(f, "%f; %f; %f\n", -h + l + t, 0.0f, -h + l + d);

			fprintf(f, "%f; %f; %f\n", -h + t, 0.0f, -h + d);
			fprintf(f, "%f; %f; %f\n", -h + t, 0.0f, -h + l + d);
			fprintf(f, "%f; %f; %f\n", -h + l + t, 0.0f, -h + l + d);

		}
	}

	fclose(f);
	//glEnd();
}


//------------------------------------------------------------------------------------------------------------------------------------

int main(int argc, char **argv) {

    if (argc == 1) {
        printf("Falta de argumentos!\n");
		return 1;
    }

    else {
        char* solid = argv[1];

        if (strcmp(solid, "plane") == 0) {
            float length = (float)atoi(argv[2]);
            float num_div = (float)atoi(argv[3]);
            char*  filename = argv[4];
            generatePlane(length, num_div, filename);
            return 0;
        }

        else if (strcmp(solid, "box") == 0) {
            float length = (float)atoi(argv[2]);
            float num_div = (float)atoi(argv[3]);
            char* filename = argv[4];
            generateBox(length, num_div, filename);
            return 0;
        }

        else if (strcmp(solid, "sphere") == 0) {
            float radius = (float)atoi(argv[2]);
            int slices = atoi(argv[3]);
            int stacks = atoi(argv[4]);
            char* filename = argv[5];
            generateSphere(radius, slices, stacks, filename);
            return 0;
        }

        else if (strcmp(solid, "cone") == 0) {
            float radius = (float)atoi(argv[2]);
            float height = (float)atoi(argv[3]);
            int slices = atoi(argv[4]);
            int stacks = atoi(argv[5]);
            char* filename = argv[6];
            generateCone(radius, height, slices, stacks, filename);
            return 0;
        }

    }
	return 1;
}


//------------------------------------------------------------------------------------------------------------------------------------
