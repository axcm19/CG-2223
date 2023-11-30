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
#include "tinyxml2.h"
#include "box.h"
#include "cone.h"
#include "plane.h"
#include "sphere.h"

using namespace std;
using namespace tinyxml2;

const char* dot_file_path = "../3d_files/";
const char* xml_file_path = "../xml_test_files/";

struct transformation {
    string type; // Qual o tipo de transformação?
    float alter_x,alter_y,alter_z,alter_angle; // Variáveis necessárias para transformação GLUT.
};

struct scene {
    vector<int> ordem; // Ordem pela qual são aplicadas os subgroups (1 = Transformation; 2 = Figuras; 3 = Cenas Adicionais)
    vector<vector<transformation>> trans; // Transformações possíveis.
    vector<vector<string>> figuras; // Figuras a desenhar neste group.
    vector<scene> cenas_adicionais; // Quando existe um group dentro de um group.
};

struct storage {
    float width, height; // Definições da Janela.
    float position_x, position_y, position_z; // Posição da Câmera.
    float lookat_x, lookat_y, lookat_z; // Para onde a Câmera está a olhar.
    float up_x, up_y, up_z; // Variável Up.
    float fov, near, far; // Projeção da Câmera.
    vector<scene> cenas; // Várias modelos e transformações a desenhar.
} xml_data;

scene group_set_up(XMLElement* group) {
    scene result; 
    vector<int> order;
    vector<vector<transformation>> transformations;
    vector<vector<string>> ficheiros;
    vector<scene> adicionais;
    
    for (XMLElement* subgroup = group->FirstChildElement(); subgroup; subgroup = subgroup->NextSiblingElement()) {
        if (strcmp(subgroup->Name(),"transform") == 0) {
            order.push_back(1);
            vector<transformation> trans;
            for (XMLElement* transform = subgroup->FirstChildElement(); transform; transform = transform->NextSiblingElement()) {
                transformation t;
                if (strcmp(transform->Name(),"translate") == 0) {
                    t.type = "translate";
                    t.alter_x = stof(transform->Attribute("x")); 
                    t.alter_y = stof(transform->Attribute("y"));
                    t.alter_z = stof(transform->Attribute("z"));
                    trans.push_back(t);
                } else if (strcmp(transform->Name(),"rotate") == 0) {
                    t.type = "rotate";
                    t.alter_angle = stof(transform->Attribute("angle")); 
                    t.alter_x = stof(transform->Attribute("x"));
                    t.alter_y = stof(transform->Attribute("y"));
                    t.alter_z = stof(transform->Attribute("z"));
                    trans.push_back(t);
                } else if (strcmp(transform->Name(),"scale") == 0) {
                    t.type = "scale";
                    t.alter_x = stof(transform->Attribute("x")); 
                    t.alter_y = stof(transform->Attribute("y"));
                    t.alter_z = stof(transform->Attribute("z"));
                    trans.push_back(t);
                } else {
                    cerr << "Erro: Transformação inválida encontrada." << endl; 
                }
            }
            transformations.push_back(trans);
        } else if (strcmp(subgroup->Name(),"models") == 0) {
            order.push_back(2);
            vector<string> files;
            for (XMLElement* model = subgroup->FirstChildElement(); model; model = model->NextSiblingElement()) {
                string s;
                files.push_back(s.append(dot_file_path).append(model->Attribute("file")));
            }
            ficheiros.push_back(files);
        } else if (strcmp(subgroup->Name(),"group") == 0) {
            order.push_back(3);
            scene cena = group_set_up(subgroup);
            adicionais.push_back(cena);
        } else {
            cerr << "Erro: Membro de grupo não existe." << endl;
        }
    }
    
    result.ordem = order; result.trans = transformations; result.figuras = ficheiros; result.cenas_adicionais = adicionais;
    return result;
}

void set_up_from_files(const char* xml_file) {
    XMLDocument doc;
    if (doc.LoadFile(xml_file) == XML_SUCCESS) {
        XMLElement* world = doc.RootElement();
        
        // Settings para Window
        XMLElement* janela = world->FirstChildElement("window");
        xml_data.width = stof(janela->Attribute("width"));
        xml_data.height = stof(janela->Attribute("height"));
       
        // Settings para Camera
        XMLElement* camera = world->FirstChildElement("camera");

        XMLElement* position = camera->FirstChildElement("position");
        xml_data.position_x = stof(position->Attribute("x"));
        xml_data.position_y = stof(position->Attribute("y"));
        xml_data.position_z = stof(position->Attribute("z"));
        
        XMLElement* lookat = camera->FirstChildElement("lookAt");
        xml_data.lookat_x = stof(lookat->Attribute("x"));
        xml_data.lookat_y = stof(lookat->Attribute("y"));
        xml_data.lookat_z = stof(lookat->Attribute("z"));
        
        XMLElement* up = camera->FirstChildElement("up");
        xml_data.up_x = stof(up->Attribute("x"));
        xml_data.up_y = stof(up->Attribute("y"));
        xml_data.up_z = stof(up->Attribute("z"));
        
        XMLElement* projection = camera->FirstChildElement("projection");
        xml_data.fov = stof(projection->Attribute("fov"));
        xml_data.near = stof(projection->Attribute("near"));
        xml_data.far = stof(projection->Attribute("far"));
        
        // Figuras para desenhar
        vector<scene> scenes;
        for (XMLElement* group = world->FirstChildElement("group"); group; group = group->NextSiblingElement()) {
            scene cena = group_set_up(group);
            scenes.push_back(cena);
        }
        xml_data.cenas = scenes;
        
    } else {
        cerr << "Erro: Ficheiro não foi aberto sucessivamente." << endl;
    }
}

void fromFileToShape(string file) { 
    ifstream reader;
    reader.open(file.c_str());
    if (!reader.is_open()) {
        cout << "Erro, ficheiro não abriu.\n";
    } else {
        string read_x, read_y, read_z, line;
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
    reader.close();
}

void drawing_time(vector<scene> para_desenhar) {
    vector<scene>::iterator it_cenas;
    vector<int>::iterator it_ordem;
    for (it_cenas = para_desenhar.begin(); it_cenas < para_desenhar.end(); it_cenas++) {
        glPushMatrix();
        scene desenho = *it_cenas;
        for (it_ordem = desenho.ordem.begin(); it_ordem < desenho.ordem.end(); it_ordem++) {
            int caso = *it_ordem;
            switch (caso) {
                case 1:
                    {
                        vector<transformation> transformacoes = desenho.trans[0];
                        vector<transformation>::iterator it_tr;
                        for (it_tr = transformacoes.begin(); it_tr < transformacoes.end(); it_tr++) {
                            transformation subject = *it_tr;
                            if (subject.type == "translate") { 
                                glTranslatef(subject.alter_x,subject.alter_y,subject.alter_z);
                            } else if (subject.type == "rotate") {
                                glRotatef(subject.alter_angle,subject.alter_x,subject.alter_y,subject.alter_z);
                            } else if (subject.type == "scale") {
                                glScalef(subject.alter_x,subject.alter_y,subject.alter_z);
                            } else {
                                cerr << "Transformação Inválida." << endl;
                            }
                        }
                        desenho.trans.erase(desenho.trans.begin());
                    }
                    break;
                case 2:
                    {
                        vector<string> files = desenho.figuras[0];
                        vector<string>::iterator it_strs;
                        for (it_strs = files.begin(); it_strs < files.end(); it_strs++) {
                            string subject = *it_strs;
                            fromFileToShape(subject);
                        } 
                        desenho.figuras.erase(desenho.figuras.begin());
                    }
                    break;
                case 3:
                    {
                        drawing_time(desenho.cenas_adicionais);
                    }
                    break;
                default:
                    cerr << "Erro: Componente de ordem não existe." << endl;
                    break;
            }
        }
        glPopMatrix();
    }
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

void renderScene(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(xml_data.position_x,xml_data.position_y,xml_data.position_z,xml_data.lookat_x,xml_data.lookat_y,xml_data.lookat_z,xml_data.up_x,xml_data.up_y,xml_data.up_z);
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
    drawing_time(xml_data.cenas);
    glutSwapBuffers();
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Erro, número incorreto de argumentos." << endl;
    }
    string s;
    set_up_from_files(s.append(xml_file_path).append(argv[1]).c_str());

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(xml_data.width,xml_data.height);
    glutCreateWindow("Engine Online");

    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);

    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

    glutMainLoop();

    return 0;
}