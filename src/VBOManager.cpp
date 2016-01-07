#include "VBOManager.h"

using namespace std;

VBO::VBO(int numVertices, Type type, int updateHint)
{
    this->type = type;
    this->numVertices = numVertices;
    this->updateHint = updateHint;
    glGenBuffers(1, &vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    DBASSERT(updateHint == GL_STREAM_DRAW || updateHint == GL_DYNAMIC_DRAW || updateHint == GL_STATIC_DRAW);
    //initialize space for data in GPU
    glBufferData(GL_ARRAY_BUFFER, getByteSize(numVertices), NULL, updateHint);
    GLERR
}

void VBO::resize(int numVertices)
{
    this->numVertices = numVertices;
    int newsize = getByteSize(numVertices);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, newsize, NULL, updateHint);
    GLERR
}

void VBO::writeData(int vertexIndex, int numVertices, void *data)
{
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    DBASSERT(vertexIndex + numVertices < this->numVertices); //do quick bounds checking in debug build
    int byteOffset = getByteSize(vertexIndex);
    int byteSize = getByteSize(numVertices);
    glBufferSubData(GL_ARRAY_BUFFER, byteOffset, byteSize, (const GLvoid*) data);
}

void VBO::draw(int startIndex, int numVertices, int geom)
{
    bind();
    DBASSERT(geom == GL_TRIANGLES || geom == GL_QUADS || geom == GL_LINES || geom == GL_TRIANGLE_STRIP)
    glDrawArrays(geom, startIndex, numVertices);
}

void VBO::setAttribLocations(int color, int tex, int pos)
{
    colorAttribLoc = color;
    texCoordAttribLoc = tex;
    posAttribLoc = pos;
}

int VBO::getByteSize(int vertices)
{
    if(type == v2D)
        return sizeof(Vertex2D) * vertices;
    else
        return sizeof(Vertex3D) * vertices;
}

void VBO::bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    if(type == v3D)
    {
        glVertexAttribPointer(colorAttribLoc, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex3D), (GLvoid*) 0);
        glVertexAttribPointer(texCoordAttribLoc, 2, GL_SHORT, GL_FALSE, sizeof(Vertex3D), (GLvoid*) 4);
        glVertexAttribPointer(posAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*) 8);
    }
    else if(type == v2D)
    {
        glVertexAttribPointer(colorAttribLoc, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex2D), (GLvoid*) 0);
        glVertexAttribPointer(texCoordAttribLoc, 2, GL_SHORT, GL_FALSE, sizeof(Vertex2D), (GLvoid*) 4);
        glVertexAttribPointer(posAttribLoc, 2, GL_SHORT, GL_FALSE, sizeof(Vertex2D), (GLvoid*) 8);
    }
}

VBOManager::VBOManager(GLuint programID)
{
    guiQuadVertices.reserve(GUI_QUAD_PRELOAD);
    guiLineVertices.reserve(GUI_LINE_PRELOAD);
    //Set up vertex attributes for shader, which are the same
    //for all 3 VBOs
    posAttribLoc = glGetAttribLocation(programID, "vertex");
    colorAttribLoc = glGetAttribLocation(programID, "color");
    texCoordAttribLoc = glGetAttribLocation(programID, "texCoord");
    glEnableVertexAttribArray(posAttribLoc);
    glEnableVertexAttribArray(colorAttribLoc);
    glEnableVertexAttribArray(texCoordAttribLoc);
    setupWorldVBO();
    setupBuildingVBO();
    setupGuiVBO();
}