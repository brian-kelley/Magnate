#ifndef __MODELS_H__
#define __MODELS_H__

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdio>
#include <cstring>
#include "SdlHeaders.h"
#include "FileIO.h"
#include "VBO.h"

using namespace std;

typedef glm::vec3 MPos;
typedef glm::vec2 MUV;
typedef glm::vec3 MNorm;

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

ostream& operator<<(ostream& os, const Face& f);

struct Model
{
    void computeNormals(); //do this if .obj doensn't have normals
    void fixPosition();    //translate so model in 1st octant at origin
    string name;
    vector<MPos> vertices;
    vector<MUV> uvs;
    vector<MNorm> norms;
    vector<Face> faces;
    int vboStart;
};

class ModelRenderer
{
public:
    ModelRenderer(bool readOBJs = true);
    void drawModel(string modelName, glm::mat4& model);
private:
    void loadOBJs();        //load all obj files in models folder
    void createVBO();       //store all models in one vbo
    void createIndexBuf();
    void readModelFile(boost::filesystem::path fpath);
    unordered_map<string, Model> models;
    VBO vbo;
};

#endif
