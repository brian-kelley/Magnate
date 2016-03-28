#include "Models.h"

using namespace std;
using namespace glm;
using namespace boost::filesystem;

vector<Model> ModelRenderer::models;
unordered_map<string, int> ModelRenderer::modelNames;
GLint ModelRenderer::modelLoc;
VBO ModelRenderer::vbo;

ostream& operator<<(ostream& os, const Face& f)
{
    os << f.v1 << '/' << f.vt1 << '/' << f.vn1 << ' ' << f.v2 << '/' << f.vt2 << '/' << f.vn2 << ' ' << f.v3 << '/' << f.vt3 << '/' << f.vn3;
    return os;
}

void Model::computeNormals()
{
    for(auto& face : faces)
    {
        if(face.vn1 != -1 && face.vn2 != -1 && face.vn3 != -1)
            continue;
        auto& v1 = vertices[face.v1];
        auto& v2 = vertices[face.v2];
        auto& v3 = vertices[face.v3];
        vec3 d1(v3.x - v2.x, v3.y - v2.y, v3.z - v2.z);
        vec3 d2(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z);
        MNorm newNorm = normalize(cross(d1, d2));
        norms.push_back(newNorm);
        //use the new norm for all vertices whose normals haven't been set
        if(face.vn1 == -1)
            face.vn1 = norms.size() - 1;
        if(face.vn2 == -1)
            face.vn2 = norms.size() - 1;
        if(face.vn3 == -1)
            face.vn3 = norms.size() - 1;
    }
}

void Model::fixPosition()
{
    if(vertices.size() == 0)
    {
        PRINT("Warning: model has 0 vertices");
        return;
    }
    float minX = vertices[0].x;
    float minY = vertices[0].y;
    float minZ = vertices[0].z;
    for(auto& v : vertices)
    {
        minX = MIN(minX, v.x);
        minY = MIN(minY, v.y);
        minZ = MIN(minZ, v.z);
    }
    vec3 lol(minX, minY, minZ);
    for(int i = 0; i < int(vertices.size()); i++)
    {
        vertices[i].x -= minX;
        vertices[i].y -= minY;
        vertices[i].z -= minZ;
    }
}

void ModelRenderer::init(GLint modelLoc)
{
    ModelRenderer::modelLoc = modelLoc;
    vbo = VBO(0, VBO::v3D, GL_STATIC_DRAW);
    loadOBJs();
    createVBO();
}

void ModelRenderer::drawModel(int modelID, mat4& model)
{
    Model& toDraw = models[modelID];
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));
    vbo.draw(toDraw.vboStart, toDraw.faces.size() * 3, GL_TRIANGLES);
}

void ModelRenderer::drawModel(string modelName, mat4& model)
{
    auto it = modelNames.find(modelName);
    if(it == modelNames.end())
    {
        PRINT("Error: model \"" << modelName << "\" not found.");
        return;
    }
    Model& toDraw = models[it->second];
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));
    vbo.draw(toDraw.vboStart, toDraw.faces.size() * 3, GL_TRIANGLES);
}

void ModelRenderer::loadOBJs()
{
    directory_iterator iter(FileIO::root() / "models");
    directory_iterator end;
    for(; iter != end; iter++)
    {
        if(iter->path().extension() == ".obj")
            readModelFile(iter->path().string());
    }
}

void ModelRenderer::readModelFile(path fpath)
{
    PRINT("Loading model \"" << fpath.string() << "\"");
    Model model;
    FILE* input = fopen(fpath.string().c_str(), "r");
    char linebuf[500];
    char* line = linebuf;
    const char* delims = "\n\t, /";
    int lineno = 1;
    while((line = fgets(linebuf, 500, input)))
    {
        cout.flush();
        char* tok = strtok(line, delims);
        if(*tok == '#') //comment line, go to next line
            continue;
        else if(!strcmp("mtllib", tok))
        {
            //TODO: process material here
        }
        else if(!strcmp("usemtl", tok))
        {
            //^^ TODO
        }
        else if(!strcmp("g", tok))
        {
            //TODO: what is .auv file
        }
        else if(!strcmp("o", tok))
        {
            //TODO: process separate sub-objects here
            //note: currently entire model must be in one object
        }
        else if(!strcmp("v", tok))
        {
            //vertex position
            MPos pos;
            tok = strtok(NULL, delims);
            sscanf(tok, "%f", &pos.x);
            tok = strtok(NULL, delims);
            sscanf(tok, "%f", &pos.y);
            tok = strtok(NULL, delims);
            sscanf(tok, "%f", &pos.z);
            model.vertices.push_back(pos);
        }
        else if(!strcmp("vt", tok))
        {
            //vertex texcoord
            MUV uv;
            tok = strtok(NULL, delims);
            sscanf(tok, "%f", &uv.x);
            tok = strtok(NULL, delims);
            sscanf(tok, "%f", &uv.y);
            tok = strtok(NULL, delims);
            model.uvs.push_back(uv);
        }
        else if(!strcmp("vn", tok))
        {
            //vertex normal
            MNorm norm;
            tok = strtok(NULL, delims);
            sscanf(tok, "%f", &norm.x);
            tok = strtok(NULL, delims);
            sscanf(tok, "%f", &norm.y);
            tok = strtok(NULL, delims);
            sscanf(tok, "%f", &norm.z);
            model.norms.push_back(norm);
        }
        else if(!strcmp("f", tok))
        {
            //face: vertex/texcoord/normal 
            //could be "v1 v2 v3", "v1/t1 v2/t2 v3/t3", or "v/t/n ..."
            //(3, 6 or 9 values)
            Face f;
            int values[9];
            int num = 0;
            tok = strtok(NULL, delims);
            while(tok)  //scan integers until end of line
            {
                sscanf(tok, "%i", &values[num]);
                values[num]--;     //obj indices start at 1
                num++;
                tok = strtok(NULL, delims);
            }
            if(num == 3)
            {
                //position only
                f.v1 = values[0];
                f.vt1 = -1;
                f.vn1 = -1;
                f.v2 = values[1];
                f.vt2 = -1;
                f.vn2 = -1;
                f.v3 = values[2];
                f.vt3 = -1;
                f.vn3 = -1;
            }
            else if(num == 6)
            {
                //position and normal 
                f.v1 = values[0];
                f.vt1 = values[1];
                f.vn1 = -1;
                f.v2 = values[2];
                f.vt2 = values[3];
                f.vn2 = -1;
                f.v3 = values[4];
                f.vt3 = values[5];
                f.vn3 = -1;
            }
            else if(num == 9)
            {
                //position, normal, texcoord
                f.v1 = values[0];
                f.vt1 = values[1];
                f.vn1 = values[2];
                f.v2 = values[3];
                f.vt2 = values[4];
                f.vn2 = values[5];
                f.v3 = values[6];
                f.vt3 = values[7];
                f.vn3 = values[8];
            }
            else
            {
                PRINT("Malformed face line:");
                PRINT(line);
            }
            model.faces.push_back(f);
        }
        lineno++;
    }
    fclose(input);
    models.push_back(move(model));
    modelNames[fpath.stem().string()] = models.size() - 1;
}

void ModelRenderer::createVBO()
{
    //Important TODO: Created an indexed vertex version of VBO class
    //Right now, vbo is bigger than necessary by a factor of 5ish
    Color4 white(255, 255, 255, 255);
    int totalVerts = 0;
    for(auto& modelEntry : models)
        totalVerts += 3 * modelEntry.faces.size();
    PRINT("Setting vbo size to " << totalVerts);
    vbo.resize(totalVerts);
    int modelIndex = 0;
    for(auto& modelEntry : models)
    {
        auto& model = modelEntry;
        model.vboStart = modelIndex;
        int modelVertices = model.faces.size() * 3;
        //allocate space for this model
        Vertex3D* vertbuf = new Vertex3D[modelVertices];
        int vertIndex = 0;
        model.computeNormals();
        model.fixPosition();
        for(auto& face : model.faces)
        {
            int verts[3] = {face.v1, face.v2, face.v3};
            int uvs[3] = {face.vt1, face.vt2, face.vt3};
            int norms[3] = {face.vn1, face.vn2, face.vn3};
            for(int i = 0; i < 3; i++)
            {
                Vertex3D temp;
                temp.pos = model.vertices[verts[i]];
                if(0)
                //if(uvs[i] != -1) //test sphereTank with no textures
                {
                    temp.texcoord.u = 2048 * model.uvs[uvs[i]].x;
                    temp.texcoord.v = 2048 * model.uvs[uvs[i]].y;
                }
                else
                {
                    temp.texcoord.u = -1;
                    temp.texcoord.v = -1;
                }
                temp.norm = model.norms[norms[i]];
                temp.color = {255, 0, 0, 255};
                vertbuf[vertIndex] = temp;
                vertIndex++;
            }
        }
        //write to VBO
        vbo.writeData(modelIndex, modelVertices, vertbuf);
        modelIndex += modelVertices;
        delete[] vertbuf;
    }
}
