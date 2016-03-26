#ifndef __VBO_H__
#define __VBO_H__

#include <iostream>
#include <vector>
#include <string>
#include "SdlHeaders.h"
#include "GlmHeaders.h"
#include "Coord.h"
#include "DebugTools.h"
#include "GenTypes.h"
#include "Input.h"

struct ClipMarker
{
    bool enable;
    int numVertices;
    Rectangle bounds;
};

class VBO
{
public:
    enum Type
    {
        v3D,     //use Vertex3D
        v2D      //use Vertex2D
    };
    VBO(int numVertices, Type type, int updateHint, bool hasIndex = false, int numIndices = 0);
    void resize(int numVertices);
    void resizeIndexBuf(int numIndices);
    void writeData(int vertexIndex, int numVertices, void* data);   //no automatic resizing!
    void writeIndexBuf(int startIndex, int num, void* data);
    void draw(int startIndex, int numVertices, int geom);
    void drawIndexed(int startIndex, int numIndices, int geom);
    void drawWithClip(int startIndex, int numVertices, int geom, const std::vector<ClipMarker>& clipMarkers);  //note: clipMarker indices map to the entire VBO, not just the given range
    int getNumVertices();
    static void loadAttribLocs(int programID);
    static GLint colorAttribLoc;
    static GLint texCoordAttribLoc;
    static GLint posAttribLoc;
    static GLint normAttribLoc;
    static GLint useNormalsLoc;
private:
    int getByteSize(int vertices);  //just multiplies vertices by the size of a vertex in bytes
    void bind();                    //bind the VBO and set attribute pointers
    GLuint vboID;                   //VBO ID# given by OpenGL
    GLuint indexID;                 //ID of index buffer, if any
    int numVertices;
    int numIndices;
    Type type;
    int updateHint;                 //static, dynamic or stream
    bool hasIndexBuf;
    static VBO* currentBound;
};

#endif
