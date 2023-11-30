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
#include <IL/il.h>
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
#include "../includes/Transformation.h"

using namespace std;
using namespace tinyxml2;

const char *dot_file_path = "../3d_files/";
const char *xml_file_path = "../xml_test_files/";
const char *texture_file_path = "../texture_files/";

float catmullDefault[4][4] = {{-0.5f, 1.5f, -1.5f, 0.5f},
                              {1.0f, -2.5f, 2.0f, -0.5f},
                              {-0.5f, 0.0f, 0.5f, 0.0f},
                              {0.0f, 1.0f, 0.0f, 0.0f}};

struct Model
{
    string name;                                             // Nome do ficheiro de pontos 3d.
    string texture;                                          // Nome do ficheiro de textura a aplicar.
    float diffuse_R = 200, diffuse_G = 200, diffuse_B = 200; // Componentes para a luz difusa no modelo. (Material)
    float ambient_R = 50, ambient_G = 50, ambient_B = 50;    // Componentes para a luz ambiente no modelo. (Material)
    float specular_R = 0, specular_G = 0, specular_B = 0;    // Componentes para a luz especular no modelo. (Material)
    float emissive_R = 0, emissive_G = 0, emissive_B = 0;    // Componentes para a luz emissiva no modelo. (Material)
    float shininess_value = 0;                               // Componente para a intensidade do brilho no modelo. (Material)
};

struct Scene
{
    vector<int> ordem;                    // Ordem pela qual são aplicadas os subgroups (1 = Transformation; 2 = Figuras; 3 = Cenas Adicionais)
    vector<vector<Transformation>> trans; // Transformações possíveis.
    vector<vector<Model>> figuras;        // Figuras a desenhar neste group.
    vector<Scene> cenas_adicionais;       // Quando existe um group dentro de um group.
};

struct Light
{
    string type;            // Tipo de luz.
    float posX, posY, posZ; // Posicionamento da luz.
    float dirX, dirY, dirZ; // Direção da luz.
    float cutoff;           // Ângulo dentro do qual a luz esta toda iluminada.
};

struct Storage
{
    float width, height;                      // Definições da Janela.
    float position_x, position_y, position_z; // Posição da Câmera.
    float lookat_x, lookat_y, lookat_z;       // Para onde a Câmera está a olhar.
    float up_x, up_y, up_z;                   // Variável Up.
    float fov, near, far;                     // Projeção da Câmera.
    vector<Light> luzes;                      // Definições de luzes.
    vector<Scene> cenas;                      // Várias modelos e transformações a desenhar.
} xml_data;

void fromFileToShape(Model file)
{
    ifstream reader;
    reader.open(file.name);
    if (!reader.is_open())
    {
        cout << "Erro, ficheiro não abriu.\n";
    }
    else
    {

        // Texture
        GLuint teximage;

        if (strcmp(file.texture.c_str(), "") != 0)
        {

            unsigned int t, tw, th;
            unsigned char *texData;
            ilGenImages(1, &t);
            ilBindImage(t);
            ilLoadImage((ILstring)file.texture.c_str());
            tw = ilGetInteger(IL_IMAGE_WIDTH);
            th = ilGetInteger(IL_IMAGE_HEIGHT);
            ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
            texData = ilGetData();
            glGenTextures(1, &teximage);
            glBindTexture(GL_TEXTURE_2D, teximage);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
        }

        // Material diffuse
        float diffuse_parameters[4] = {file.diffuse_R, file.diffuse_G, file.diffuse_B, 1.0};
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_parameters);

        // Material ambient
        float ambient_parameters[4] = {file.ambient_R, file.ambient_G, file.ambient_B, 1.0};
        glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_parameters);

        // Material specular
        float specular_parameters[4] = {file.specular_R, file.specular_G, file.specular_B, 1.0};
        glMaterialfv(GL_FRONT, GL_SPECULAR, specular_parameters);

        // Material emissive
        float emissive_parameters[4] = {file.emissive_R, file.emissive_G, file.emissive_B, 1.0};
        glMaterialfv(GL_FRONT, GL_EMISSION, emissive_parameters);

        // Material shininess
        float shininess_parameter = file.shininess_value;
        glMaterialf(GL_FRONT, GL_SHININESS, shininess_parameter);

        string line;
        string read_x, read_y, read_z;
        string read_text_x, read_text_y;              // coordenadas de textura
        string read_norm_x, read_norm_y, read_norm_z; // coordenadas da normal

        if (strcmp(file.texture.c_str(), "") != 0)
        {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, teximage);
        }

        // Desenhe o objeto com a textura

        glBegin(GL_TRIANGLES);
        while (getline(reader, line))
        {
            stringstream ss(line);
            getline(ss, read_x, ';');
            getline(ss, read_y, ';');
            getline(ss, read_z, ';');

            getline(ss, read_text_x, ';');
            getline(ss, read_text_y, ';');

            getline(ss, read_norm_x, ';');
            getline(ss, read_norm_y, ';');
            getline(ss, read_norm_z, ';');

            float x = strtof(read_x.c_str(), NULL);
            float y = strtof(read_y.c_str(), NULL);
            float z = strtof(read_z.c_str(), NULL);

            float text_x = strtof(read_text_x.c_str(), NULL);
            float text_y = strtof(read_text_y.c_str(), NULL);

            float norm_x = strtof(read_norm_x.c_str(), NULL);
            float norm_y = strtof(read_norm_y.c_str(), NULL);
            float norm_z = strtof(read_norm_z.c_str(), NULL);

            glTexCoord2f(text_x, text_y);
            glVertex3f(x, y, z);
        }
        glEnd();

        glDisable(GL_TEXTURE_2D);
    }
    reader.close();
}

Scene group_set_up(XMLElement *group)
{
    Scene result;
    vector<int> order;
    vector<vector<Transformation>> Transformations;
    vector<vector<Model>> ficheiros;
    vector<Scene> adicionais;

    for (XMLElement *subgroup = group->FirstChildElement(); subgroup; subgroup = subgroup->NextSiblingElement())
    {

        if (strcmp(subgroup->Name(), "transform") == 0)
        {
            order.push_back(1);
            vector<Transformation> trans;
            for (XMLElement *transform = subgroup->FirstChildElement(); transform; transform = transform->NextSiblingElement())
            {
                Transformation t;
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
                            ponto.push_back(stof(translate->Attribute("x")));
                            ponto.push_back(stof(translate->Attribute("y")));
                            ponto.push_back(stof(translate->Attribute("z")));
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
            Transformations.push_back(trans);
        }
        else if (strcmp(subgroup->Name(), "models") == 0)
        {
            order.push_back(2);
            vector<Model> files;
            for (XMLElement *model = subgroup->FirstChildElement(); model; model = model->NextSiblingElement())
            {
                Model m;
                string f;
                m.name = f.append(dot_file_path).append(model->Attribute("file"));
                if (model->FirstChildElement("texture"))
                {
                    string t;
                    XMLElement *texture = model->FirstChildElement("texture");
                    m.texture = t.append(texture_file_path).append(texture->Attribute("file"));
                }
                if (model->FirstChildElement("color"))
                {
                    XMLElement *color = model->FirstChildElement("color");

                    m.diffuse_R = stof(color->FirstChildElement("diffuse")->Attribute("R"));
                    m.diffuse_G = stof(color->FirstChildElement("diffuse")->Attribute("G"));
                    m.diffuse_B = stof(color->FirstChildElement("diffuse")->Attribute("B"));

                    m.ambient_R = stof(color->FirstChildElement("ambient")->Attribute("R"));
                    m.ambient_G = stof(color->FirstChildElement("ambient")->Attribute("G"));
                    m.ambient_B = stof(color->FirstChildElement("ambient")->Attribute("B"));

                    m.specular_R = stof(color->FirstChildElement("specular")->Attribute("R"));
                    m.specular_G = stof(color->FirstChildElement("specular")->Attribute("G"));
                    m.specular_B = stof(color->FirstChildElement("specular")->Attribute("B"));

                    m.emissive_R = stof(color->FirstChildElement("emissive")->Attribute("R"));
                    m.emissive_G = stof(color->FirstChildElement("emissive")->Attribute("G"));
                    m.emissive_B = stof(color->FirstChildElement("emissive")->Attribute("B"));

                    m.shininess_value = stof(color->FirstChildElement("shininess")->Attribute("value"));
                }
                files.push_back(m);
            }
            ficheiros.push_back(files);
        }
        else if (strcmp(subgroup->Name(), "group") == 0)
        {
            order.push_back(3);
            Scene cena = group_set_up(subgroup);
            adicionais.push_back(cena);
        }
        else
        {
            cerr << "Erro: Membro de grupo não existe." << endl;
        }
    }

    result.ordem = order;
    result.trans = Transformations;
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

        // Settings para a luz
        vector<Light> luzes;
        XMLElement *lights = world->FirstChildElement("lights");
        for (XMLElement *light = lights->FirstChildElement("light"); light; light = light->NextSiblingElement())
        {
            Light nova_luz;
            if (strcmp(light->Attribute("type"), "point") == 0)
            {
                nova_luz.type = light->Attribute("type");
                nova_luz.posX = stof(light->Attribute("posx"));
                nova_luz.posY = stof(light->Attribute("posy"));
                nova_luz.posZ = stof(light->Attribute("posz"));
            }
            else if (strcmp(light->Attribute("type"), "directional") == 0)
            {
                nova_luz.type = light->Attribute("type");
                nova_luz.dirX = stof(light->Attribute("dirx"));
                nova_luz.dirY = stof(light->Attribute("diry"));
                nova_luz.dirZ = stof(light->Attribute("dirz"));
            }
            else if (strcmp(light->Attribute("type"), "spotlight") == 0 || strcmp(light->Attribute("type"), "spot") == 0)
            {
                nova_luz.type = light->Attribute("type");

                nova_luz.posX = stof(light->Attribute("posx"));
                nova_luz.posY = stof(light->Attribute("posy"));
                nova_luz.posZ = stof(light->Attribute("posz"));

                nova_luz.dirX = stof(light->Attribute("dirx"));
                nova_luz.dirY = stof(light->Attribute("diry"));
                nova_luz.dirZ = stof(light->Attribute("dirz"));

                nova_luz.cutoff = stof(light->Attribute("cutoff"));
            }
            else
            {
                cerr << "Erro: Tipo de luz não permitido." << endl;
            }
            luzes.push_back(nova_luz);
        }
        xml_data.luzes = luzes;
        // Figuras para desenhar
        vector<Scene> scenes;
        for (XMLElement *group = world->FirstChildElement("group"); group; group = group->NextSiblingElement())
        {
            Scene cena = group_set_up(group);
            scenes.push_back(cena);
        }
        xml_data.cenas = scenes;
    }
    else
    {
        cerr << "Erro: Ficheiro não abriu." << endl;
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
                        float *pos, float *deriv, bool align)
{
    for (int i = 0; i < 3; i++)
    {
        float vp[4] = {p0[i], p1[i], p2[i], p3[i]};
        float res[4];
        multMatrixVector(*catmullDefault, vp, res);

        pos[i] = powf(t, 3.0) * res[0] + powf(t, 2.0) * res[1] + t * res[2] + res[3];

        if (align == true)
        {
            deriv[i] = 3 * powf(t, 2.0) * res[0] + 2 * t * res[1] + res[2];
        }
        else
        {
            deriv[i] = 1;
        }
    }
}

void GlobalCatmullRomPoint(float gt, float *pos, float *deriv, vector<vector<float>> interpolation_points, bool align)
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
                       pos, deriv, align);
}

void renderCatmullCurve(vector<vector<float>> interpolation_points, bool align)
{
    float posicoes[3];
    float derivadas[3];
    glBegin(GL_LINE_LOOP);
    float global_t = 0;
    while (global_t < 1)
    {
        GlobalCatmullRomPoint(global_t, posicoes, derivadas, interpolation_points, align);
        glVertex3f(posicoes[0], posicoes[1], posicoes[2]);
        global_t += 0.01;
    }
    glEnd();
}

void lighting_time(vector<Light> para_iluminar)
{
    vector<Light>::iterator it_luzes;
    int i = 0;
    for (it_luzes = para_iluminar.begin(); it_luzes < para_iluminar.end(); it_luzes++)
    {
        Light luz = *it_luzes;
        switch (i)
        {
        case 0:
        {
            glEnable(GL_LIGHT0);
            if (strcmp(luz.type.c_str(), "point") == 0)
            {
                float pos[4] = {luz.posX, luz.posY, luz.posZ, 1.0};
                glLightfv(GL_LIGHT0, GL_POSITION, pos);
            }
            else if (strcmp(luz.type.c_str(), "directional") == 0)
            {
                float dir[4] = {luz.dirX, luz.dirY, luz.dirZ, 0.0};
                glLightfv(GL_LIGHT0, GL_POSITION, dir);
            }
            else
            {
                glEnable(GL_LIGHT0);
                float spot_pos[4] = {luz.posX, luz.posY, luz.posZ, 1.0};
                float spot_dir[3] = {luz.dirX, luz.dirY, luz.dirZ};
                glLightfv(GL_LIGHT0, GL_POSITION, spot_pos);
                glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_dir);
                glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, luz.cutoff);
            }
        }
        break;
        case 1:
        {
            glEnable(GL_LIGHT1);
            if (strcmp(luz.type.c_str(), "point") == 0)
            {
                float pos[4] = {luz.posX, luz.posY, luz.posZ, 1.0};
                glLightfv(GL_LIGHT1, GL_POSITION, pos);
            }
            else if (strcmp(luz.type.c_str(), "directional") == 0)
            {
                float dir[4] = {luz.dirX, luz.dirY, luz.dirZ, 0.0};
                glLightfv(GL_LIGHT1, GL_POSITION, dir);
            }
            else
            {
                float spot_pos[4] = {luz.posX, luz.posY, luz.posZ, 1.0};
                float spot_dir[3] = {luz.dirX, luz.dirY, luz.dirZ};
                glLightfv(GL_LIGHT1, GL_POSITION, spot_pos);
                glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_dir);
                glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, luz.cutoff);
            }
        }
        break;
        case 2:
        {
            glEnable(GL_LIGHT2);
            if (strcmp(luz.type.c_str(), "point") == 0)
            {
                float pos[4] = {luz.posX, luz.posY, luz.posZ, 1.0};
                glLightfv(GL_LIGHT2, GL_POSITION, pos);
            }
            else if (strcmp(luz.type.c_str(), "directional") == 0)
            {
                float dir[4] = {luz.dirX, luz.dirY, luz.dirZ, 0.0};
                glLightfv(GL_LIGHT2, GL_POSITION, dir);
            }
            else
            {
                float spot_pos[4] = {luz.posX, luz.posY, luz.posZ, 1.0};
                float spot_dir[3] = {luz.dirX, luz.dirY, luz.dirZ};
                glLightfv(GL_LIGHT2, GL_POSITION, spot_pos);
                glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spot_dir);
                glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, luz.cutoff);
            }
        }
        break;
        case 3:
        {
            glEnable(GL_LIGHT3);
            if (strcmp(luz.type.c_str(), "point") == 0)
            {
                float pos[4] = {luz.posX, luz.posY, luz.posZ, 1.0};
                glLightfv(GL_LIGHT3, GL_POSITION, pos);
            }
            else if (strcmp(luz.type.c_str(), "directional") == 0)
            {
                float dir[4] = {luz.dirX, luz.dirY, luz.dirZ, 0.0};
                glLightfv(GL_LIGHT3, GL_POSITION, dir);
            }
            else
            {
                float spot_pos[4] = {luz.posX, luz.posY, luz.posZ, 1.0};
                float spot_dir[3] = {luz.dirX, luz.dirY, luz.dirZ};
                glLightfv(GL_LIGHT3, GL_POSITION, spot_pos);
                glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, spot_dir);
                glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, luz.cutoff);
            }
        }
        break;
        case 4:
        {
            glEnable(GL_LIGHT4);
            if (strcmp(luz.type.c_str(), "point") == 0)
            {
                float pos[4] = {luz.posX, luz.posY, luz.posZ, 1.0};
                glLightfv(GL_LIGHT4, GL_POSITION, pos);
            }
            else if (strcmp(luz.type.c_str(), "directional") == 0)
            {
                float dir[4] = {luz.dirX, luz.dirY, luz.dirZ, 0.0};
                glLightfv(GL_LIGHT4, GL_POSITION, dir);
            }
            else
            {
                float spot_pos[4] = {luz.posX, luz.posY, luz.posZ, 1.0};
                float spot_dir[3] = {luz.dirX, luz.dirY, luz.dirZ};
                glLightfv(GL_LIGHT4, GL_POSITION, spot_pos);
                glLightfv(GL_LIGHT4, GL_SPOT_DIRECTION, spot_dir);
                glLightf(GL_LIGHT4, GL_SPOT_CUTOFF, luz.cutoff);
            }
        }
        break;
        case 5:
        {
            glEnable(GL_LIGHT5);
            if (strcmp(luz.type.c_str(), "point") == 0)
            {
                float pos[4] = {luz.posX, luz.posY, luz.posZ, 1.0};
                glLightfv(GL_LIGHT5, GL_POSITION, pos);
            }
            else if (strcmp(luz.type.c_str(), "directional") == 0)
            {
                float dir[4] = {luz.dirX, luz.dirY, luz.dirZ, 0.0};
                glLightfv(GL_LIGHT5, GL_POSITION, dir);
            }
            else
            {
                float spot_pos[4] = {luz.posX, luz.posY, luz.posZ, 1.0};
                float spot_dir[3] = {luz.dirX, luz.dirY, luz.dirZ};
                glLightfv(GL_LIGHT5, GL_POSITION, spot_pos);
                glLightfv(GL_LIGHT5, GL_SPOT_DIRECTION, spot_dir);
                glLightf(GL_LIGHT5, GL_SPOT_CUTOFF, luz.cutoff);
            }
        }
        break;
        case 6:
        {
            glEnable(GL_LIGHT6);
            if (strcmp(luz.type.c_str(), "point") == 0)
            {
                float pos[4] = {luz.posX, luz.posY, luz.posZ, 1.0};
                glLightfv(GL_LIGHT6, GL_POSITION, pos);
            }
            else if (strcmp(luz.type.c_str(), "directional") == 0)
            {
                float dir[4] = {luz.dirX, luz.dirY, luz.dirZ, 0.0};
                glLightfv(GL_LIGHT6, GL_POSITION, dir);
            }
            else
            {
                float spot_pos[4] = {luz.posX, luz.posY, luz.posZ, 1.0};
                float spot_dir[3] = {luz.dirX, luz.dirY, luz.dirZ};
                glLightfv(GL_LIGHT6, GL_POSITION, spot_pos);
                glLightfv(GL_LIGHT6, GL_SPOT_DIRECTION, spot_dir);
                glLightf(GL_LIGHT6, GL_SPOT_CUTOFF, luz.cutoff);
            }
        }
        break;
        case 7:
        {
            glEnable(GL_LIGHT7);
            if (strcmp(luz.type.c_str(), "point") == 0)
            {
                float pos[4] = {luz.posX, luz.posY, luz.posZ, 1.0};
                glLightfv(GL_LIGHT7, GL_POSITION, pos);
            }
            else if (strcmp(luz.type.c_str(), "directional") == 0)
            {
                float dir[4] = {luz.dirX, luz.dirY, luz.dirZ, 0.0};
                glLightfv(GL_LIGHT7, GL_POSITION, dir);
            }
            else
            {
                float spot_pos[4] = {luz.posX, luz.posY, luz.posZ, 1.0};
                float spot_dir[3] = {luz.dirX, luz.dirY, luz.dirZ};
                glLightfv(GL_LIGHT7, GL_POSITION, spot_pos);
                glLightfv(GL_LIGHT7, GL_SPOT_DIRECTION, spot_dir);
                glLightf(GL_LIGHT7, GL_SPOT_CUTOFF, luz.cutoff);
            }
        }
        break;
        default:
            cerr << "Não são permitidos mais fontes de luz. (Limite mínimo de OpenGL)" << endl;
            break;
        }
    }
}

void drawing_time(vector<Scene> para_desenhar)
{
    int lightNum = 0;
    vector<Scene>::iterator it_cenas;
    vector<int>::iterator it_ordem;
    for (it_cenas = para_desenhar.begin(); it_cenas < para_desenhar.end(); it_cenas++)
    {
        glPushMatrix();
        Scene desenho = *it_cenas;
        for (it_ordem = desenho.ordem.begin(); it_ordem < desenho.ordem.end(); it_ordem++)
        {
            int caso = *it_ordem;
            switch (caso)
            {
            case 1:
            {
                vector<Transformation> transformacoes = desenho.trans[0];
                vector<Transformation>::iterator it_tr;
                for (it_tr = transformacoes.begin(); it_tr < transformacoes.end(); it_tr++)
                {
                    Transformation subject = *it_tr;
                    if (subject.getType() == "translate")
                    {
                        glTranslatef(subject.getVarX(), subject.getVarY(), subject.getVarZ());
                    }
                    else if (subject.getType() == "translate_catmull")
                    {

                        renderCatmullCurve(subject.getCatmullPoints(), subject.getAlign());

                        float pos[3];
                        float deriv[3];

                        float deltaTime;
                        static float timeNow = subject.getTimeNow();

                        GlobalCatmullRomPoint(timeNow, pos, deriv, subject.getCatmullPoints(), subject.getAlign());

                        glTranslatef(pos[0], pos[1], pos[2]);

                        float x[3] = {deriv[0], deriv[1], deriv[2]};
                        normalize(x);
                        float z[3];
                        float prev_y[3] = {subject.getPrevY()[0], subject.getPrevY()[1], subject.getPrevY()[2]};
                        cross(x, prev_y, z);
                        normalize(z);
                        float y[3];
                        cross(z, x, y);
                        normalize(y);

                        vector<float> vec(y, (y + sizeof(y) / sizeof(y[0])));
                        subject.setPrevY(vec);
                        float m[16];
                        buildRotMatrix(x, y, z, m);
                        glMultMatrixf(m);

                        timeNow = glutGet(GLUT_ELAPSED_TIME) / (1000.0f * subject.getTime());
                        deltaTime = timeNow - subject.getTimeNow();
                        subject.setTimeNow(subject.getTimeNow() + (deltaTime / (subject.getTime())));
                    }
                    else if (subject.getType() == "rotate")
                    {
                        glRotatef(subject.getAngle(), subject.getVarX(), subject.getVarY(), subject.getVarZ());
                    }
                    else if (subject.getType() == "rotate_catmull")
                    {
                        static float timeNow;
                        float deltaTime;
                        glRotatef((360 / (subject.getTime() * 1000.0f)) * subject.getTimeNow(),
                                  subject.getVarX(),
                                  subject.getVarY(),
                                  subject.getVarZ());
                        timeNow = glutGet(GLUT_ELAPSED_TIME);
                        deltaTime = timeNow - subject.getTimeNow();
                        subject.setTimeNow(subject.getTimeNow() + deltaTime);
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
                vector<Model> files = desenho.figuras[0];
                vector<Model>::iterator it_strs;
                for (it_strs = files.begin(); it_strs < files.end(); it_strs++)
                {
                    Model subject = *it_strs;
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
            {
                cerr << "Erro: Componente de ordem não existe." << endl;
            }
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

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(xml_data.position_x, xml_data.position_y, xml_data.position_z,
              xml_data.lookat_x, xml_data.lookat_y, xml_data.lookat_z,
              xml_data.up_x, xml_data.up_y, xml_data.up_z);

    glDisable(GL_LIGHTING);

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
    glEnable(GL_LIGHTING);

    lighting_time(xml_data.luzes);
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
    set_up_from_files(s.c_str());

    glutInit(&argc, argv);
    glewInit();
    ilInit();
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(xml_data.width, xml_data.height);
    glutCreateWindow("Engine Online");

    glutDisplayFunc(renderScene);
    glutIdleFunc(renderScene);
    glutReshapeFunc(changeSize);

    float amb[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);
    glEnable(GL_RESCALE_NORMAL);

    glEnable(GL_DEPTH_TEST);
    glutMainLoop();

    return 0;
}