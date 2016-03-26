#include "VBO.h"

using namespace std;

#define INDEX_SIZE (sizeof(unsigned int))

int VBO::colorAttribLoc = -1;
int VBO::texCoordAttribLoc = -1;
int VBO::posAttribLoc = -1;
int VBO::modelLoc = -1;
VBO* VBO::currentBound = nullptr;

VBO::VBO(int numVertices, Type type, int updateHint, bool hasIndex, int numIndices)
{
    this->type = type;
    this->numVertices = numVertices;
    this->updateHint = updateHint;
    this->hasIndexBuf = hasIndex;
    GLERR
    glGenBuffers(1, &vboID);
    GLERR
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    DBASSERT(updateHint == GL_STREAM_DRAW || updateHint == GL_DYNAMIC_DRAW || updateHint == GL_STATIC_DRAW);
    GLERR
    //initialize space for data in GPU
    if(numVertices > 0)
    {
        glBufferData(GL_ARRAY_BUFFER, getByteSize(numVertices), NULL, updateHint);
        GLERR;
    }
    currentBound = nullptr;     //initially, no vbo is bound
    if(0)
    //if(hasIndexBuf)
    {
        glGenBuffers(1, &indexID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * INDEX_SIZE, NULL, updateHint);
    }
}

void VBO::resize(int numVertices)
{
    bind();
    this->numVertices = numVertices;
    int newsize = getByteSize(numVertices);
    glBufferData(GL_ARRAY_BUFFER, newsize, NULL, updateHint);
    GLERR
}

void VBO::resizeIndexBuf(int numIndices)
{
    bind();
    this->numIndices = numIndices;
    int newSize = INDEX_SIZE * numIndices;
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, newSize, NULL, updateHint);
} 

void VBO::writeData(int vertexIndex, int verticesToCopy, void *data)
{
    bind();
    DBASSERT(vertexIndex + verticesToCopy <= numVertices); //do quick bounds checking in debug build
    int byteOffset = getByteSize(vertexIndex);
    int byteSize = getByteSize(verticesToCopy);
    glBufferSubData(GL_ARRAY_BUFFER, byteOffset, byteSize, (const GLvoid*) data);
}

void VBO::writeIndexBuf(int startIndex, int num, void* data)
{
    bind();
    DBASSERT(startIndex + num <= numIndices);
    int byteOffset = INDEX_SIZE * startIndex;
    int byteSize = INDEX_SIZE * num;
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, byteOffset, byteSize, (const GLvoid*) data);
}

void VBO::draw(int startIndex, int vertsToDraw, int geom)
{
    bind();
    DBASSERT(geom == GL_TRIANGLES || geom == GL_QUADS || geom == GL_LINES || geom == GL_TRIANGLE_STRIP);
    DBASSERT(startIndex >= 0);
    DBASSERT(startIndex + vertsToDraw <= this->numVertices);
    glDrawArrays(geom, startIndex, vertsToDraw);
}

void VBO::drawWithClip(int startIndex, int verticesToDraw, int geom, const vector<ClipMarker>& clipMarkers)
{
    bind();
    DBASSERT(geom == GL_TRIANGLES || geom == GL_QUADS || geom == GL_LINES || geom == GL_TRIANGLE_STRIP)
    int numDrawn = 0;
    int numPassed = 0;  //counting the vertices in markers until startIndex reached
    for(auto& marker : clipMarkers)
    {
        if(numPassed < startIndex)
        {
            numPassed += marker.numVertices;
            continue;
        }
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
        if(numDrawn >= verticesToDraw)
            break;
    }
    glDisable(GL_SCISSOR_TEST);
}

void VBO::drawIndexed(int startIndex, int numIndices, int geom)
{
    if(!hasIndexBuf)
    {
        PRINT("Warning: this vbo has no index buffer");
        return;
    } 
    bind();
    DBASSERT(geom == GL_TRIANGLES || geom == GL_QUADS || geom == GL_LINES || geom == GL_TRIANGLE_STRIP)
    glDrawElements(geom, numIndices, GL_UNSIGNED_INT, (GLvoid*) (INDEX_SIZE * startIndex));
}

void VBO::loadAttribLocs(int programID)
{
    colorAttribLoc = glGetAttribLocation(programID, "color");
    texCoordAttribLoc = glGetAttribLocation(programID, "texCoord");
    posAttribLoc = glGetAttribLocation(programID, "vertex");
    modelLoc = glGetUniformLocation(programID, "model");
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
    //and mark this vbo as currently bound
    currentBound = this;
}

int VBO::getNumVertices()
{
    return numVertices;
}
