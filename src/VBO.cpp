#include "VBO.h"

using namespace std;

int VBO::colorAttribLoc = -1;
int VBO::texCoordAttribLoc = -1;
int VBO::posAttribLoc = -1;
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
    auto clipIter = clipMarkers.begin();
    //Determine whether any clipping will happen at all
    while(clipIter != clipMarkers.end() && clipIter->startVertex < startIndex)
        clipIter++;
    if(clipIter == clipMarkers.end())
    {
        //clip markers didn't affect the vertex range
        glDrawArrays(geom, startIndex, verticesToDraw);
        return;
    }
    else
    {
        //clipIter at first relevant clip marker
        //while there are vertices to draw:
        //  is the next vertex in a clipped range?
        //  if yes, draw up to the end of the clipped range or end of drawing range (whichever lower)
        //  if no, draw unclipped up to the beginning of next clipped range or end of drawing range (whichever lower)
        int vertexIndex = startIndex;
        int vertexEnd = startIndex + verticesToDraw;
        while(vertexIndex < startIndex + verticesToDraw)
        {
            if(clipIter->startVertex > vertexIndex)
            {
                //next draw call not clipped
                int endIndex = min(clipIter->startVertex, vertexEnd);
                glDrawArrays(geom, vertexIndex, endIndex - vertexIndex);
                vertexIndex = endIndex;
            }
            else
            {
                //next draw call will be clipped (clipIter has the current clip rectangle)
                int endIndex = min(clipIter->startVertex + clipIter->numVertices, vertexEnd);
                glScissor(clipIter->bounds.x, clipIter->bounds.y, clipIter->bounds.w, clipIter->bounds.h);
                glEnable(GL_SCISSOR_TEST);
                glDrawArrays(geom, vertexIndex, endIndex - vertexIndex);
                vertexIndex = endIndex;
            }
        }
    }
}

void VBO::loadAttribLocs(int programID)
{
    colorAttribLoc = glGetAttribLocation(programID, "color");
    GLERR;
    texCoordAttribLoc = glGetAttribLocation(programID, "texCoord");
    GLERR;
    posAttribLoc = glGetAttribLocation(programID, "vertex");
    GLERR;
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
