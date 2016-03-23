#include "VBO.h"

using namespace std;

int VBO::colorAttribLoc = -1;
int VBO::texCoordAttribLoc = -1;
int VBO::posAttribLoc = -1;
int VBO::normAttribLoc = -1;
int VBO::useNormalsLoc = -1;
VBO* VBO::currentBound = nullptr;

VBO::VBO(int numVertices, Type type, int updateHint)
{
    this->type = type;
    this->numVertices = numVertices;
    this->updateHint = updateHint;
    GLERR
    glGenBuffers(1, &vboID);
    GLERR
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    DBASSERT(updateHint == GL_STREAM_DRAW || updateHint == GL_DYNAMIC_DRAW || updateHint == GL_STATIC_DRAW);
    GLERR
    //initialize space for data in GPU
    glBufferData(GL_ARRAY_BUFFER, getByteSize(numVertices), NULL, updateHint);
    GLERR
    currentBound = nullptr;     //initially, no vbo is bound
}

void VBO::resize(int numVertices)
{
    if(this->numVertices < numVertices)
    {
        bind();
        this->numVertices = numVertices;
        int newsize = getByteSize(numVertices);
        glBindBuffer(GL_ARRAY_BUFFER, vboID);
        glBufferData(GL_ARRAY_BUFFER, newsize, NULL, updateHint);
        GLERR
    }
}

void VBO::writeData(int vertexIndex, int verticesToCopy, void *data)
{
    bind();
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    DBASSERT(vertexIndex + verticesToCopy <= numVertices); //do quick bounds checking in debug build
    int byteOffset = getByteSize(vertexIndex);
    int byteSize = getByteSize(verticesToCopy);
    glBufferSubData(GL_ARRAY_BUFFER, byteOffset, byteSize, (const GLvoid*) data);
}

void VBO::draw(int startIndex, int numVertices, int geom)
{
    bind();
    DBASSERT(geom == GL_TRIANGLES || geom == GL_QUADS || geom == GL_LINES || geom == GL_TRIANGLE_STRIP)
    glDrawArrays(geom, startIndex, numVertices);
}

void VBO::drawWithClip(int startIndex, int verticesToDraw, int geom, const vector<ClipMarker>& clipMarkers)
{
    bind();
    DBASSERT(geom == GL_TRIANGLES || geom == GL_QUADS || geom == GL_LINES || geom == GL_TRIANGLE_STRIP)
    int numDrawn = 0;
    for(auto& marker : clipMarkers)
    {
        if(marker.enable)
        {
            //Scissor uses vertically inverted coordinate system, convert rectangle
            glScissor(marker.bounds.x, Input::winY - (marker.bounds.y + marker.bounds.h), marker.bounds.w, marker.bounds.h);
            glEnable(GL_SCISSOR_TEST);
            glDrawArrays(geom, numDrawn, marker.numVertices);
            numDrawn += marker.numVertices;
        }
        else
        {
            glDisable(GL_SCISSOR_TEST);
            glDrawArrays(geom, numDrawn, marker.numVertices);
            numDrawn += marker.numVertices;
        }
    }
    glDisable(GL_SCISSOR_TEST);
}

void VBO::loadAttribLocs(int programID)
{
    colorAttribLoc = glGetAttribLocation(programID, "color");
    texCoordAttribLoc = glGetAttribLocation(programID, "texCoord");
    posAttribLoc = glGetAttribLocation(programID, "vertex");
    normAttribLoc = glGetAttribLocation(programID, "normal");
    useNormalsLoc = glGetUniformLocation(programID, "useNormals");
    glEnableVertexAttribArray(colorAttribLoc);
    glEnableVertexAttribArray(texCoordAttribLoc);
    glEnableVertexAttribArray(posAttribLoc);
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
    //if VBO is already bound, don't waste time binding it again
    if(currentBound == this)
        return;
    //otherwise, currentBound will end up pointing to this vbo instead of the previously bound one
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    if(type == v3D)
    {
        //Enable normals
        glEnableVertexAttribArray(normAttribLoc);
        glUniform1i(useNormalsLoc, 1);
        glVertexAttribPointer(colorAttribLoc, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex3D), (GLvoid*) 0);
        glVertexAttribPointer(texCoordAttribLoc, 2, GL_SHORT, GL_FALSE, sizeof(Vertex3D), (GLvoid*) 4);
        glVertexAttribPointer(posAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*) 8);
        glVertexAttribPointer(normAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*) 20);
    }
    else if(type == v2D)
    {
        //Disable normals (not used for 2D)
        glDisableVertexAttribArray(normAttribLoc);
        glUniform1i(useNormalsLoc, 0);
        glVertexAttribPointer(colorAttribLoc, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex2D), (GLvoid*) 0);
        glVertexAttribPointer(texCoordAttribLoc, 2, GL_SHORT, GL_FALSE, sizeof(Vertex2D), (GLvoid*) 4);
        glVertexAttribPointer(posAttribLoc, 2, GL_SHORT, GL_FALSE, sizeof(Vertex2D), (GLvoid*) 8);
    }
    //and mark this vbo as currently bound
    currentBound = this;
}

int VBO::getNumVertices()
{
    return numVertices;
}
