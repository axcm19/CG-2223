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
#include <GL/glew.h>
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
#include <map>
#include <algorithm>
#include "tinyxml2.h"
#include "box.h"
#include "cone.h"
#include "plane.h"
#include "sphere.h"
#include "../includes/transformation.h"

using namespace std;
using namespace tinyxml2;

const char* dot_file_path = "../3d_files/";
const char* xml_file_path = "../xml_test_files/";
//const char* xml_file_path = "/home/afonso/Desktop/CG_2023/Fase_3/xml_test_files/";

float catmullDefault[4][4] = {{-0.5f, 1.5f, -1.5f, 0.5f},
                              {1.0f, -2.5f, 2.0f, -0.5f},
                              {-0.5f, 0.0f, 0.5f, 0.0f},
                              {0.0f, 1.0f, 0.0f, 0.0f}};

map<string, vector<vector<float>>> modelos_em_memoria;

void store_in_memory(string file_name, string file_path)
{
    map<string, vector<vector<float>>>::iterator it;
    it = modelos_em_memoria.find(file_name);
    if (it == modelos_em_memoria.end())
    { // Não encontrou este ficheiro, vamos guardar uma nova cópia.
        ifstream reader;
        reader.open(file_path.c_str());
        if (!reader.is_open())
        {
            cout << "Erro, ficheiro não abriu.\n";
        }
        else
        {
            vector<vector<float>> pontos;
            string read_x, read_y, read_z, line;
            while (getline(reader, line))
            {
                stringstream ss(line);
                getline(ss, read_x, ';');
                getline(ss, read_y, ';');
                getline(ss, read_z, ';');
                vector<float> ponto;
                float x = strtof(read_x.c_str(), NULL);
                float y = strtof(read_y.c_str(), NULL);
                float z = strtof(read_z.c_str(), NULL);
                ponto.push_back(x);
                ponto.push_back(y);
                ponto.push_back(z);
                pontos.push_back(ponto);
            }
            modelos_em_memoria.insert({file_name, pontos});
        }
        reader.close();
    }
}

struct vbo_object
{
    GLuint buffer;
    GLuint verticeCount;
};

map<string, vbo_object> VBOs;

vector<float> fullListVertex(vector<vector<float>> m)
{
    vector<float> result;
    for (int i = 0; i < m.size(); i++)
    {
        for (int j = 0; j < m[i].size(); j++)
        {
            result.push_back(m[i][j]);
        }
    }
    return result;
}

void prepareData()
{
    GLuint vertices[modelos_em_memoria.size()];
    GLuint verticeCounts[modelos_em_memoria.size()];

    glGenBuffers(modelos_em_memoria.size(), vertices);

    int i = 0;
    for (auto &entry : modelos_em_memoria)
    {
        verticeCounts[i] = entry.second.size();
        glBindBuffer(GL_ARRAY_BUFFER, vertices[i]);
        vector<float> fullVertex = fullListVertex(entry.second);
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(float) * fullVertex.size(),
            fullVertex.data(),
            GL_STATIC_DRAW);
        vbo_object new_vbo;
        new_vbo.buffer = vertices[i];
        new_vbo.verticeCount = verticeCounts[i];
        VBOs.insert({entry.first, new_vbo});
        i++;
    }
}

void fromFileToShape(string file)
{
    vbo_object target = VBOs[file];
    glBindBuffer(GL_ARRAY_BUFFER, target.buffer);
    glVertexPointer(3, GL_FLOAT, 0, 0);
    glDrawArrays(GL_TRIANGLES, 0, target.verticeCount);
}

struct scene
{
    vector<int> ordem;                    // Ordem pela qual são aplicadas os subgroups (1 = Transformation; 2 = Figuras; 3 = Cenas Adicionais)
    vector<vector<transformation>> trans; // Transformações possíveis.
    vector<vector<string>> figuras;       // Figuras a desenhar neste group.
    vector<scene> cenas_adicionais;       // Quando existe um group dentro de um group.
};

struct storage
{
    float width, height;                      // Definições da Janela.
    float position_x, position_y, position_z; // Posição da Câmera.
    float lookat_x, lookat_y, lookat_z;       // Para onde a Câmera está a olhar.
    float up_x, up_y, up_z;                   // Variável Up.
    float fov, near, far;                     // Projeção da Câmera.
    vector<scene> cenas;                      // Várias modelos e transformações a desenhar.
} xml_data;

scene group_set_up(XMLElement *group)
{
    scene result;
    vector<int> order;
    vector<vector<transformation>> transformations;
    vector<vector<string>> ficheiros;
    vector<scene> adicionais;

    for (XMLElement *subgroup = group->FirstChildElement(); subgroup; subgroup = subgroup->NextSiblingElement())
    {
        if (strcmp(subgroup->Name(), "transform") == 0)
        {
            order.push_back(1);
            vector<transformation> trans;
            for (XMLElement *transform = subgroup->FirstChildElement(); transform; transform = transform->NextSiblingElement())
            {
                transformation t;
                if (strcmp(transform->Name(), "translate") == 0)
                {
                    if (transform->Attribute("time"))
                    {
                        t.setType("translate_catmull");
                        t.setTime(stof(transform->Attribute("time")));
                        t.setTimeNow(0.0f);
                        t.setAlign(transform->Attribute("align"));
                        t.setPrevY({0, -1, 0});
                        vector<vector<float>> pontos;
                        for (XMLElement *translate = transform->FirstChildElement(); translate; translate = translate->NextSiblingElement())
                        {
                            vector<float> ponto;
                            ponto.push_back(stof(transform->Attribute("x")));
                            ponto.push_back(stof(transform->Attribute("y")));
                            ponto.push_back(stof(transform->Attribute("z")));
                            pontos.push_back(ponto);
                        }
                        t.setCatmullPoints(pontos);
                        trans.push_back(t);
                    }
                    else
                    {
                        t.setType("translate");
                        t.setVarX(stof(transform->Attribute("x")));
                        t.setVarY(stof(transform->Attribute("y")));
                        t.setVarZ(stof(transform->Attribute("z")));
                        trans.push_back(t);
                    }
                }
                else if (strcmp(transform->Name(), "rotate") == 0)
                {
                    if (transform->Attribute("time"))
                    {
                        t.setType("rotate_catmull");
                        t.setTimeNow(0.0f);
                        t.setTime(stof(transform->Attribute("time")));
                        t.setVarX(stof(transform->Attribute("x")));
                        t.setVarY(stof(transform->Attribute("y")));
                        t.setVarZ(stof(transform->Attribute("z")));
                        trans.push_back(t);
                    }
                    else
                    {
                        t.setType("rotate");
                        t.setAngle(stof(transform->Attribute("angle")));
                        t.setVarX(stof(transform->Attribute("x")));
                        t.setVarY(stof(transform->Attribute("y")));
                        t.setVarZ(stof(transform->Attribute("z")));
                        trans.push_back(t);
                    }
                }
                else if (strcmp(transform->Name(), "scale") == 0)
                {
                    t.setType("scale");
                    t.setVarX(stof(transform->Attribute("x")));
                    t.setVarY(stof(transform->Attribute("y")));
                    t.setVarZ(stof(transform->Attribute("z")));
                    trans.push_back(t);
                }
                else
                {
                    cerr << "Erro: Transformação inválida encontrada." << endl;
                }
            }
            cout << "Cheguei aqui!\n";
            transformations.push_back(trans);
        }
        else if (strcmp(subgroup->Name(), "models") == 0)
        {
            order.push_back(2);
            vector<string> files;
            for (XMLElement *model = subgroup->FirstChildElement(); model; model = model->NextSiblingElement())
            {
                string s;
                files.push_back(s.append(dot_file_path).append(model->Attribute("file")));
                store_in_memory(model->Attribute("file"), s);
            }
            ficheiros.push_back(files);
        }
        else if (strcmp(subgroup->Name(), "group") == 0)
        {
            order.push_back(3);
            scene cena = group_set_up(subgroup);
            adicionais.push_back(cena);
        }
        else
        {
            cerr << "Erro: Membro de grupo não existe." << endl;
        }
    }

    result.ordem = order;
    result.trans = transformations;
    result.figuras = ficheiros;
    result.cenas_adicionais = adicionais;
    return result;
}

void set_up_from_files(const char *xml_file)
{
    
    XMLDocument doc;
    if (doc.LoadFile(xml_file) == XML_SUCCESS)
    {
        XMLElement *world = doc.RootElement();

        // Settings para Window
        XMLElement *janela = world->FirstChildElement("window");
        xml_data.width = stof(janela->Attribute("width"));
        xml_data.height = stof(janela->Attribute("height"));

        // Settings para Camera
        XMLElement *camera = world->FirstChildElement("camera");

        XMLElement *position = camera->FirstChildElement("position");
        xml_data.position_x = stof(position->Attribute("x"));
        xml_data.position_y = stof(position->Attribute("y"));
        xml_data.position_z = stof(position->Attribute("z"));

        XMLElement *lookat = camera->FirstChildElement("lookAt");
        xml_data.lookat_x = stof(lookat->Attribute("x"));
        xml_data.lookat_y = stof(lookat->Attribute("y"));
        xml_data.lookat_z = stof(lookat->Attribute("z"));

        XMLElement *up = camera->FirstChildElement("up");
        xml_data.up_x = stof(up->Attribute("x"));
        xml_data.up_y = stof(up->Attribute("y"));
        xml_data.up_z = stof(up->Attribute("z"));

        XMLElement *projection = camera->FirstChildElement("projection");
        xml_data.fov = stof(projection->Attribute("fov"));
        xml_data.near = stof(projection->Attribute("near"));
        xml_data.far = stof(projection->Attribute("far"));

        // Figuras para desenhar
        vector<scene> scenes;
        for (XMLElement *group = world->FirstChildElement("group"); group; group = group->NextSiblingElement())
        {
            scene cena = group_set_up(group);
            scenes.push_back(cena);
        }
        xml_data.cenas = scenes;
    }
    else
    {
        cerr << "Erro: Ficheiro não foi aberto sucessivamente." << endl;
    }
}

void buildRotMatrix(float *x, float *y, float *z, float *m)
{

    m[0] = x[0];
    m[1] = x[1];
    m[2] = x[2];
    m[3] = 0;
    m[4] = y[0];
    m[5] = y[1];
    m[6] = y[2];
    m[7] = 0;
    m[8] = z[0];
    m[9] = z[1];
    m[10] = z[2];
    m[11] = 0;
    m[12] = 0;
    m[13] = 0;
    m[14] = 0;
    m[15] = 1;
}

void cross(float *a, float *b, float *res)
{

    res[0] = a[1] * b[2] - a[2] * b[1];
    res[1] = a[2] * b[0] - a[0] * b[2];
    res[2] = a[0] * b[1] - a[1] * b[0];
}

void normalize(float *a)
{

    float l = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
    a[0] = a[0] / l;
    a[1] = a[1] / l;
    a[2] = a[2] / l;
}

void multMatrixVector(float *m, float *v, float *res)
{
    for (int j = 0; j < 4; ++j)
    {
        res[j] = 0;
        for (int k = 0; k < 4; ++k)
        {
            res[j] += v[k] * m[j * 4 + k];
        }
    }
}

void getCatmullRomPoint(float t,
                        vector<float> p0, vector<float> p1, vector<float> p2, vector<float> p3,
                        float *pos, float *deriv)
{
    for (int i = 0; i < 3; i++)
    {
        float vp[4] = {p0[i], p1[i], p2[i], p3[i]};
        float res[4];
        multMatrixVector(*catmullDefault, vp, res);

        pos[i] = powf(t, 3.0) * res[0] + powf(t, 2.0) * res[1] + t * res[2] + res[3];

        deriv[i] = 3 * powf(t, 2.0) * res[0] + 2 * t * res[1] + res[2];
    }
}

void GlobalCatmullRomPoint(float gt, float *pos, float *deriv, vector<vector<float>> interpolation_points)
{
    float local_t = gt * interpolation_points.size();
    int index = floor(local_t);
    local_t = local_t - index;

    int indices[4];
    indices[0] = (index + interpolation_points.size() - 1) % interpolation_points.size();
    indices[1] = (indices[0] + 1) % interpolation_points.size();
    indices[2] = (indices[1] + 1) % interpolation_points.size();
    indices[3] = (indices[2] + 1) % interpolation_points.size();

    getCatmullRomPoint(local_t,
                       interpolation_points[indices[0]], interpolation_points[indices[1]], interpolation_points[indices[2]], interpolation_points[indices[3]],
                       pos, deriv);
}

void renderCatmullCurve(vector<vector<float>> interpolation_points)
{
    float posicoes[3];
    float derivadas[3];

    glBegin(GL_LINE_LOOP);
    float global_t = 0;
    while (global_t < 1)
    {
        GlobalCatmullRomPoint(global_t, posicoes, derivadas, interpolation_points);
        glVertex3f(posicoes[0], posicoes[1], posicoes[2]);
        global_t += 0.01;
    }
    glEnd();
}

void drawing_time(vector<scene> para_desenhar)
{
    vector<scene>::iterator it_cenas;
    vector<int>::iterator it_ordem;
    for (it_cenas = para_desenhar.begin(); it_cenas < para_desenhar.end(); it_cenas++)
    {
        glPushMatrix();
        scene desenho = *it_cenas;
        for (it_ordem = desenho.ordem.begin(); it_ordem < desenho.ordem.end(); it_ordem++)
        {
            int caso = *it_ordem;
            switch (caso)
            {
            case 1:
            {
                vector<transformation> transformacoes = desenho.trans[0];
                vector<transformation>::iterator it_tr;
                for (it_tr = transformacoes.begin(); it_tr < transformacoes.end(); it_tr++)
                {
                    transformation subject = *it_tr;
                    if (subject.getType() == "translate")
                    {
                        glTranslatef(subject.getVarX(), subject.getVarY(), subject.getVarZ());
                    }
                    else if (subject.getType() == "translate_catmull")
                    {
                        static float t = 0;
                        float deltaTime, timeNow;

                        renderCatmullCurve(subject.getCatmullPoints());

                        float pos[3];
                        float deriv[3];

                        GlobalCatmullRomPoint(t, pos, deriv, subject.getCatmullPoints());

                        glTranslatef(pos[0], pos[1], pos[2]);

                        if (subject.getAlign() == "True")
                        {
                            float x[3] = {deriv[0], deriv[1], deriv[2]};
                            normalize(x);
                            float z[3];
                            cross(x, subject.getPrevY().data(), z);
                            normalize(z);
                            float y[3];
                            cross(z, x, y);
                            normalize(y);
                            vector<float> vec(y, y + sizeof(y) / sizeof(float));
                            subject.setPrevY(vec);

                            float m[16];

                            buildRotMatrix(x, y, z, m);

                            glMultMatrixf(m);
                        }

                        timeNow = glutGet(GLUT_ELAPSED_TIME) / (1000.0f * subject.getTime());
                        deltaTime = timeNow - subject.getTimeNow();
                        subject.setTimeNow(deltaTime / (subject.getTime()));
                    }
                    else if (subject.getType() == "rotate")
                    {
                        glRotatef(subject.getAngle(), subject.getVarX(), subject.getVarY(), subject.getVarZ());
                    }
                    else if (subject.getType() == "rotate_catmull")
                    {
                        float deltaTime, timeNow, time_var;
                        glRotatef((360 / (subject.getTime() * 1000.0f)) * subject.getTimeNow(),
                                  subject.getVarX(),
                                  subject.getVarY(),
                                  subject.getVarZ());
                        timeNow = glutGet(GLUT_ELAPSED_TIME);
                        deltaTime = timeNow - subject.getTimeNow();
                        time_var = subject.getTimeNow();
                        subject.setTimeNow(time_var += deltaTime);
                    }
                    else if (subject.getType() == "scale")
                    {
                        glScalef(subject.getVarX(), subject.getVarY(), subject.getVarZ());
                    }
                    else
                    {
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
                for (it_strs = files.begin(); it_strs < files.end(); it_strs++)
                {
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

void changeSize(int w, int h)
{
    if (h == 0)
        h = 1;
    float ratio = w * 1.0f / h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    gluPerspective(45.0f, ratio, 1.0f, 1000.0f);
    glMatrixMode(GL_MODELVIEW);
}

void renderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(xml_data.position_x, xml_data.position_y, xml_data.position_z, xml_data.lookat_x, xml_data.lookat_y, xml_data.lookat_z, xml_data.up_x, xml_data.up_y, xml_data.up_z);
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-100.0f, 0.0f, 0.0f);
    glVertex3f(100.0f, 0.0f, 0.0f);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, -100.0f, 0.0f);
    glVertex3f(0.0f, 100.0f, 0.0f);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, -100.0f);
    glVertex3f(0.0f, 0.0f, 100.0f);
    glEnd();
    glColor3f(1.0f, 1.0f, 1.0f);
    drawing_time(xml_data.cenas);
    glutSwapBuffers();
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cerr << "Erro, número incorreto de argumentos." << endl;
    }
    string s;
    s.append(xml_file_path).append(argv[1]);
    cout << s << endl;
    
    set_up_from_files(s.c_str());


    prepareData();

    glutInit(&argc, argv);
    glewInit();
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(xml_data.width, xml_data.height);
    glutCreateWindow("Engine Online");

    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);

    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glutMainLoop();

    return 0;
}