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

std::ostream& operator<<(std::ostream& os, const Face& f);

struct Model
{
    void computeNormals(); //do this if .obj doensn't have normals
    void fixPosition();    //translate so model in 1st octant at origin
    std::string name;
    std::vector<MPos> vertices;
    std::vector<MUV> uvs;
    std::vector<MNorm> norms;
    std::vector<Face> faces;
    int vboStart;
};

namespace ModelRenderer
{
    void init(GLint modelLoc);
    void drawModel(int modelID, glm::mat4& model);
    void drawModel(std::string name, glm::mat4& model);
    void loadOBJs();
    void createVBO();
    void readModelFile(boost::filesystem::path fpath);
    extern std::vector<Model> models;
    extern std::unordered_map<std::string, int> modelNames;
    extern GLint modelLoc;
    extern VBO vbo;
};

#endif
