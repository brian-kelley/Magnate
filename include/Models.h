#ifndef __MODELS_H__
#define __MODELS_H__

#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include "SdlHeaders.h"
#include "FileIO.h"
#include "VBO.h"

using namespace std;

struct MPos
{
    float x, y, z;
};

struct MUV
{
    float u, v;
};

struct MNorm
{
    float nx, ny, nz;
};

struct Face
{
    //Needs index of
    int v1;
    int vt1;
    int vn1;
    int v2;
    int vt2;
    int vn2;
    int v3;
    int vt3;
    int vn3;
};

istream& operator>>(istream& is, MPos& pos);
istream& operator>>(istream& is, MUV& uv);
istream& operator>>(istream& is, MNorm& norm);
istream& operator>>(istream& is, Face& face);

struct Model
{
    string name;
    vector<MPos> vertices;
    vector<MUV> uvs;
    vector<MNorm> norms;
    vector<Face> faces;
    int vboStart;
    int numVertices;
};

class ModelRenderer
{
public:
    ModelRenderer(GLuint modelLoc, bool readOBJs = true);
private:
    void loadOBJs();        //load all obj files in models folder
    void createVBO();       //store all models in one vbo
    void readModelFile(boost::filesystem::path fpath);
    unordered_map<string, Model> models;
    VBO vbo;
};

#endif
