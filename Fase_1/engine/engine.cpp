#include <cstdio>
#include <string>
#include <type_traits>
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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include "../rapidxml-1.13/rapidxml.hpp"
#include "box.h"
#include "cone.h"
#include "plane.h"

using namespace std;
using namespace rapidxml;

//const char* relative_path = "/home/tomas/Desktop/CG_2/TP23/3d_files/";
//const char* xml_file_path = "/home/tomas/Desktop/CG_2/TP23/xml_test_files/";

const char* relative_path = "../3d_files/";
const char* xml_file_path = "../xml_test_files/";

float width = 0.0f; float height = 0.0f;
float position_x = 5.0f; float position_y = 5.0f; float position_z = 5.0f;
float lookat_x = 0.0f; float lookat_y = 0.0f; float lookat_z = 0.0f;
float up_x = 0.0f; float up_y = 1.0f; float up_z = 0.0f;
float fov = 0.0f; float near = 0.0f; float far = 0.0;

vector<string> files3D = {};

vector<string> set_up_from_files(char* xml_file) {
    vector<string> files = {};
    xml_document<> ficheiro;
    xml_node<>* world_node = NULL;
    
    ifstream file(xml_file);
    vector<char> buffer((istreambuf_iterator<char>(file)),istreambuf_iterator<char>());
    buffer.push_back('\0');
    file.close();
    ficheiro.parse<0>(&buffer[0]);

    world_node = ficheiro.first_node("world");
    xml_node<> * window_node = world_node->first_node("window");
    width = stof(window_node->first_attribute("width")->value());
    height = stof(window_node->first_attribute("height")->value());
    xml_node<> * camera_node = world_node->first_node("camera");
    xml_node<> * position_node = camera_node->first_node("position");
    position_x = stof(position_node->first_attribute("x")->value());
    position_y = stof(position_node->first_attribute("y")->value());
    position_z = stof(position_node->first_attribute("z")->value());
    xml_node<> * lookAt_node = camera_node->first_node("lookAt");
    lookat_x = stof(lookAt_node->first_attribute("x")->value());
    lookat_y = stof(lookAt_node->first_attribute("y")->value());
    lookat_z = stof(lookAt_node->first_attribute("z")->value());
    xml_node<> * up_node = camera_node->first_node("up");
    up_x = stof(up_node->first_attribute("x")->value());
    up_y = stof(up_node->first_attribute("y")->value());
    up_z = stof(up_node->first_attribute("z")->value());
    xml_node<> * projection_node = camera_node->first_node("projection");
    fov = stof(projection_node->first_attribute("fov")->value());
    near = stof(projection_node->first_attribute("near")->value());
    far = stof(projection_node->first_attribute("far")->value());
    xml_node<> * group_node = world_node->first_node("group");
    xml_node<> * models_node = group_node->first_node("models");
    for (xml_node<> * model_node = models_node->first_node("model"); model_node; model_node = model_node->next_sibling())
    {
        string s;
        files.push_back(s.append(relative_path).append(model_node->first_attribute("file")->value()));
    }
    return files;
}

void changeSize(int w, int h) {
    if (h == 0) h = 1;
    float ratio = w * 1.0f / h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0,0,w,h);
    gluPerspective(45.0f,ratio,1.0f,1000.0f);
    glMatrixMode(GL_MODELVIEW);
}

void fromFileToShape(vector<string> files) { 
    ifstream reader;
    for (string file : files) {
        reader.open(file.c_str());
        if (!reader.is_open()) {
            cout << "Erro, ficheiro não abriu.\n";
        } else {
            string read_x, read_y, read_z;
            string line;

            glBegin(GL_TRIANGLES);
                while (getline(reader, line)) {
                    stringstream ss(line);
                    getline(ss, read_x, ';');
                    getline(ss, read_y, ';');
                    getline(ss, read_z, ';');

                    float x = strtof(read_x.c_str(), NULL);
                    float y = strtof(read_y.c_str(), NULL);
                    float z = strtof(read_z.c_str(), NULL);

                    glVertex3f(x, y, z);
                }
            glEnd();
        }
    }
    reader.close();
}

void renderScene(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(position_x,position_y,position_z,
    lookat_x,lookat_y,lookat_z,
    up_x,up_y,up_z);
    glBegin(GL_LINES);
		glColor3f(1.0f,0.0f,0.0f);
		glVertex3f(-100.0f,0.0f,0.0f);
		glVertex3f(100.0f,0.0f,0.0f);
		glColor3f(0.0f,1.0f,0.0f);
		glVertex3f(0.0f,-100.0f,0.0f);
		glVertex3f(0.0f,100.0f,0.0f);
		glColor3f(0.0f,0.0f,1.0f);
		glVertex3f(0.0f,0.0f,-100.0f);
		glVertex3f(0.0f,0.0f,100.0f);
	glEnd();
    glColor3f(1.0f, 1.0f, 1.0f);
    fromFileToShape(files3D);
    glutSwapBuffers();
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Erro, número incorreto de argumentos.";
        return -1;
    }
    files3D = set_up_from_files(argv[1]);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(width,height);
    glutCreateWindow("Engine Online.");

    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);


    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

    glutMainLoop();
    return 1;
}