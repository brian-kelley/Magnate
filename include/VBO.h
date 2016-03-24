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

/*
 Vertex fields/sizes
 (SOA) - all positions, all colors, all texcoords in one VBO
 2D:
 -short vec2 pos                (normalized to [-1, 1])
 -unsigned char vec4 color      (normalized to [0, 1])
 -short vec2 texcoord           (normalized to [-1, 1])
 Total: 12 bytes/vertex
 
 3D:
 -float vec3 pos
 -unsigned char vec4 color
 -short vec2 texcoord
 Total: 20 bytes/vertex
 */

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
    VBO(int numVertices, Type type, int updateHint);
    void resize(int numVertices);
    void writeData(int vertexIndex, int numVertices, void* data);   //no automatic resizing!
    void draw(int startIndex, int numVertices, int geom);
    void drawWithClip(int startIndex, int numVertices, int geom, const std::vector<ClipMarker>& clipMarkers);  //note: clipMarker indices map to the entire VBO, not just the given range
    int getNumVertices();
    static void loadAttribLocs(int programID);
private:
    int getByteSize(int vertices);  //just multiplies vertices by the size of a vertex in bytes
    void bind();                    //bind the VBO and set attribute pointers
    GLuint vboID;       //VBO ID# given by OpenGL
    int numVertices;
    Type type;
    int updateHint;     //static, dynamic or stream
    static int colorAttribLoc;
    static int texCoordAttribLoc;
    static int posAttribLoc;
    static VBO* currentBound;
};

#endif
