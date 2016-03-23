#include "Models.h"

using namespace std;
using namespace boost::filesystem;

istream& operator>>(istream& is, MPos& pos)
{
    is >> pos.x >> pos.y >> pos.z;
    return is;
}

istream& operator>>(istream& is, MUV& uv)
{
    is >> uv.u >> uv.v;
    return is;
}

istream& operator>>(istream& is, MNorm& norm)
{
    is >> norm.nx >> norm.ny >> norm.nz;
    return is;
}

istream& operator>>(istream& is, Face& face)
{
    is >> face.v1;
    is.ignore(); //expect "/" as delimiter
    is >> face.vt1;
    is.ignore();
    is >> face.vn1;
    is >> face.v2;
    is.ignore();
    is >> face.vt2;
    is.ignore();
    is >> face.vn2;
    is.ignore();
    is >> face.v3;
    is.ignore();
    is >> face.vt3;
    is.ignore();
    is >> face.vn3;
    return is;
}

ModelRenderer::ModelRenderer(GLuint modelLoc, bool readOBJs) : vbo(0, VBO::v3D, GL_STATIC_DRAW)
{
    if(readOBJs)
        loadOBJs();
}

void ModelRenderer::loadOBJs()
{
    directory_iterator iter(FileIO::root / "models");
    directory_iterator end;
    for(; iter != end; iter++)
    {
        if(iter->path().extension() == ".obj")
        {
            readModelFile(iter->path().string());
        }
    }
}

void ModelRenderer::readModelFile(path fpath)
{
    Model model;
    ifstream obj(fpath.string());
    while(!obj.eof())
    {
        string type;
        obj >> type;
        if(type == "v")
        {
            MPos pos;
            obj >> pos;
            model.vertices.push_back(pos);
        }
        else if(type == "vt")
        {
            MUV uv;
            obj >> uv;
            model.uvs.push_back(uv);
        }
        else if(type == "vn")
        {
            MNorm norm;
            obj >> norm;
            model.norms.push_back(norm);
        }
        else if(type == "f")
        {
            Face face;
            obj >> face;
            model.faces.push_back(face);
        }
        else
        {
            //skip to next line
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    obj.close();
    models[fpath.stem().string()] = model;
}

void ModelRenderer::createVBO()
{
    //TODO: Created an indexed vertex version of VBO class
    //Right now, vbo is bigger than necessary by a factor of 5ish
    Color4 white(255, 255, 255, 255);
    for(auto& modelEntry : models)
    {
        auto& model = modelEntry.second;
        int numVertices = model.faces.size() * 3;
        Vertex3D* vertbuf = new Vertex3D[numVertices];
        int vertIndex = 0;
        for(auto& face : model.faces)
        {
            auto* pos = &model.vertices[face.v1];
            vertbuf[vertIndex].pos = Pos3(pos->x, pos->y, pos->z);
            
        }
        delete[] vertbuf;
    }
}
