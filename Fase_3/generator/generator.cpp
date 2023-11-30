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
#include <vector>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>

using namespace std;

const char* relative_path = "../3d_files/";
const char* bezier_path = "../patch_test_files/";

//------------------------------------------------------------------------------------------------------------------------------------

void generateSphere(float radius, const int slices, int stacks, char* filename) {
	//ESTA FUNÇÃO SERVE APENAS PARA DEFINIR OS VÉRTICES QUE VÃO COMPOR A FIGURA DA ESFERA E ESCREVE-LOS NO FICHEIRO sphere.3d

    // Concatenar o caminho relativo com o nome do ficheiro
	string file;
	file.append(relative_path).append(filename);
	// Abrir o ficheiro em modo de escrita
	FILE * f = fopen(file.c_str(), "w+");
    // Escrever o nome da figura no ficheiro
	//fprintf(f,"sphere\n");

	// Gerar vértices da esfera
	const float TWO_PI = 2.0f * M_PI;
	float theta, phi;
	float x, y, z;
	vector<float> vertices;
	vector<int> indices;
	for (int i = 0; i <= stacks; ++i) {
		theta = i * M_PI / stacks;
		for (int j = 0; j <= slices; ++j) {
			phi = j * TWO_PI / slices;
			x = radius * sin(theta) * cos(phi);
			z = radius * sin(theta) * sin(phi); // Corrigido: z representa a largura
			y = radius * cos(theta); // Corrigido: y representa a altura

			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);
		}
	}

	// Gerar índices da esfera
	int rowStart, nextRowStart;
	for (int i = 0; i < stacks; ++i) {

		rowStart = i * (slices + 1);
		nextRowStart = (i + 1) * (slices + 1);
		for (int j = 0; j < slices; ++j) {
			if (i != 0) {
				indices.push_back(rowStart + j);
				indices.push_back(nextRowStart + j);
				indices.push_back(nextRowStart + j + 1);
			}
			if (i != stacks - 1) {
				indices.push_back(rowStart + j);
				indices.push_back(nextRowStart + j + 1);
				indices.push_back(rowStart + j + 1);
			}
		}
	}

	// Imprimir os pontos para o ficheiro "sphere.3d"
	for (int i = 0; i < indices.size(); i += 3) {

		int index1 = indices[i];
		int index2 = indices[i + 1];
		int index3 = indices[i + 2];

		fprintf(f, "%f; %f; %f\n", vertices[index1 * 3], vertices[index1 * 3 + 1], vertices[index1 * 3 + 2]);
		fprintf(f, "%f; %f; %f\n", vertices[index2 * 3], vertices[index2 * 3 + 1], vertices[index2 * 3 + 2]);
		fprintf(f, "%f; %f; %f\n", vertices[index3 * 3], vertices[index3 * 3 + 1], vertices[index3 * 3 + 2]);

	}
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


struct bezier_storage {
	int n_patches;
	vector<int> indices_por_patch;
	int n_control_points;
	vector<vector<float>> pontos_controlo;  
};

vector<int> parseInfoPatch(string line_patch) {
	string num;
	std::vector<int> indices;
	stringstream input_stringstream(line_patch);

	while (std::getline(input_stringstream, num, ',')) {
		indices.push_back(std::stoi(num));
	}

	return indices;
}


vector<float> parseInfoPoint(string line_point) {
	string num;
	std::vector<float> pontos;
	stringstream input_stringstream(line_point);


	while (std::getline(input_stringstream, num, ',')) {
		pontos.push_back(std::stof(num));
	}
	
	return pontos;
}


void multMatrixVector(float* m, float* v, float* res) {

	for (int j = 0; j < 4; ++j) {
		res[j] = 0;
		for (int k = 0; k < 4; ++k) {
			res[j] += v[k] * m[j * 4 + k];
		}
	}

}


void getBezierPoint(float u, float v, float** matrixX, float** matrixY, float** matrixZ, float* pos) {
	float bezierMatrix[4][4] = { { -1.0f, 3.0f, -3.0f, 1.0f },
							   { 3.0f, -6.0f, 3.0f, 0.0f },
							   { -3.0f, 3.0f, 0.0f, 0.0f },
							   { 1.0f,  0.0f, 0.0f, 0.0f } };

	float vetorU[4] = { u * u * u, u * u, u, 1 };
	float vetorV[4] = { v * v * v, v * v, v, 1 };

	float vetorAux[4];
	float px[4];
	float py[4];
	float pz[4];

	float mx[4];
	float my[4];
	float mz[4];

	multMatrixVector((float*)bezierMatrix, vetorV, vetorAux);
	multMatrixVector((float*)matrixX, vetorAux, px);
	multMatrixVector((float*)matrixY, vetorAux, py);
	multMatrixVector((float*)matrixZ, vetorAux, pz);

	multMatrixVector((float*)bezierMatrix, px, mx);
	multMatrixVector((float*)bezierMatrix, py, my);
	multMatrixVector((float*)bezierMatrix, pz, mz);


	pos[0] = (vetorU[0] * mx[0]) + (vetorU[1] * mx[1]) + (vetorU[2] * mx[2]) + (vetorU[3] * mx[3]);
	pos[1] = (vetorU[0] * my[0]) + (vetorU[1] * my[1]) + (vetorU[2] * my[2]) + (vetorU[3] * my[3]);
	pos[2] = (vetorU[0] * mz[0]) + (vetorU[1] * mz[1]) + (vetorU[2] * mz[2]) + (vetorU[3] * mz[3]);

}

void generateBezier(char* file_patch, char* file_3d, float tesselation) {

	string file1;
	file1.append(bezier_path).append(file_patch);
	FILE * fp = fopen(file1.c_str(), "r");

	string file2;
	file2.append(relative_path).append(file_3d);
	FILE * f3d = fopen(file2.c_str(), "w+");

	//FILE* fp = std::fopen(file_patch, "r");
	//FILE* f3d = std::fopen(file_3d, "w");
	//fprintf(f3d, "bezier\n");

	//leitura do ficheiro patch e preenchimento da estrutura bezier
	struct bezier_storage bs;

	//ifstream myFileStream(file_patch);
	ifstream myFileStream(file1);

	if (!myFileStream.is_open()) {
		cout << "Error" << endl;
	}

	string num_patch, num_point;
	string line_patch, line_point;

	if (myFileStream.is_open()) {
		//-----------------------------------------------------------

		// BUSCAR NUMERO DE PATCH'S
		getline(myFileStream, num_patch, '\n');

		bs.n_patches = strtod(num_patch.c_str(), NULL);

		std::cout << bs.n_patches << std::endl;
		
		// BUSCAR INFO DOS PATCH'S
		for (int i = 0; i < bs.n_patches; i++) {
			
			getline(myFileStream, line_patch);
			std::vector<int> numeros = parseInfoPatch(line_patch);
			for (int i = 0; i < numeros.size(); i++) {
				bs.indices_por_patch.push_back(numeros[i]);
			}
			
		}
		for (int j = 0; j < bs.indices_por_patch.size(); j++) {
			char res[50];
			sprintf(res, "%d", bs.indices_por_patch[j]);
			std::cout << res << std::endl;
		}


		// BUSCAR NUMERO DE PONTOS
		getline(myFileStream, num_point, '\n');

		bs.n_control_points = strtod(num_point.c_str(), NULL);

		std::cout << bs.n_control_points << std::endl;

		// BUSCAR INFO DOS PONTOS
		for (int i = 0; i < bs.n_control_points; i++) {
			getline(myFileStream, line_point);
			std::vector<float> numeros = parseInfoPoint(line_point);
			bs.pontos_controlo.push_back(numeros);

			char res[200];
			sprintf(res, "%f, %f, %f", bs.pontos_controlo[i][0], bs.pontos_controlo[i][1], bs.pontos_controlo[i][2]);
			std::cout << res << std::endl;

		}
		std::cout << "\n\n" << std::endl;

		//-----------------------------------------------------------

	}

	myFileStream.close();


	//escrita no ficheiro 3d

	float matrixX[4][4];
	float matrixY[4][4];
	float matrixZ[4][4];

	float pos[4][3];
	float u = 0;
	float v = 0;
	float t = 1 / tesselation;

	//int counter = 0;
	//int number_points = (bs.indices_por_patch.size() / 16) * 6 * tesselation * tesselation;


	for (int s = 0; s < bs.indices_por_patch.size(); s += 16) {

		for (size_t a = 0; a < 4; a++) {

			for (size_t b = 0; b < 4; b++) {
				matrixX[a][b] = bs.pontos_controlo.at(bs.indices_por_patch.at(s + a * 4 + b))[0];
				matrixY[a][b] = bs.pontos_controlo.at(bs.indices_por_patch.at(s + a * 4 + b))[1];
				matrixZ[a][b] = bs.pontos_controlo.at(bs.indices_por_patch.at(s + a * 4 + b))[2];
			}
		}

		for (int i = 0; i < tesselation; i++) {

			for (int j = 0; j < tesselation; j++) {
				//counter++;
				u = t * i;
				v = t * j;
				float next_u = t * (i + 1);
				float next_v = t * (j + 1);

				getBezierPoint(u, v, (float**)matrixX, (float**)matrixY, (float**)matrixZ, pos[0]);
				getBezierPoint(u, next_v, (float**)matrixX, (float**)matrixY, (float**)matrixZ, pos[1]);
				getBezierPoint(next_u, v, (float**)matrixX, (float**)matrixY, (float**)matrixZ, pos[2]);
				getBezierPoint(next_u, next_v, (float**)matrixX, (float**)matrixY, (float**)matrixZ, pos[3]);

				fprintf(f3d, "%f; %f; %f\n", pos[3][0], pos[3][1], pos[3][2]);
				fprintf(f3d, "%f; %f; %f\n", pos[2][0], pos[2][1], pos[2][2]);
				fprintf(f3d, "%f; %f; %f\n", pos[0][0], pos[0][1], pos[0][2]);

				fprintf(f3d, "%f; %f; %f\n", pos[0][0], pos[0][1], pos[0][2]);
				fprintf(f3d, "%f; %f; %f\n", pos[1][0], pos[1][1], pos[1][2]);
				fprintf(f3d, "%f; %f; %f\n", pos[3][0], pos[3][1], pos[3][2]);

			}

		}
	}
	
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

	//./generator bezier "das.patch" "das.3d"

	else if (strcmp(solid, "bezier") == 0) {
            char* file1 = argv[2];
	    	char* file2 = argv[3];
			float tesselation = stof(argv[4]);
            generateBezier(file1, file2, tesselation);
            return 0;
        }

    }
	return 1;
}


//------------------------------------------------------------------------------------------------------------------------------------
