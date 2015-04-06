#ifndef __RENDERER__INCLUDED__
#define __RENDERER__INCLUDED__

#ifdef __APPLE__
#include "gl.h"
#elif _WIN32
#include "GL/gl.h"
#endif

#include <vector>
#include <string>
#include <iostream>

typedef struct
{
    float x;    //Position on screen (pixels?)
    float y;
    float r;    //Color
    float g;
    float b;
    float a;
    float u;    //Texcoord
    float v;
} vertex;

namespace Renderer
{
    void initAll();
    void initVBO();
    void initShaders();
    void startFrame();
    void endFrame();
    void dispose();
    void drawQuad(vertex v1, vertex v2, vertex v3, vertex v4);
    void addVertex(vertex v);
    //Vertex array is interleaved: xyrgbauvxyrgbauv...
    void color3f(float r, float g, float b);
    void color4f(float r, float g, float b, float a);
    void vertex2i(float x, float y);
    void texCoord2f(float u, float v);
    void saveVertex();          //push stateVertex onto 'vertices'
    void enableTexture();       //use uniform variable to enable/disable
    void disableTexture();      //2d texture blitting (like glEnable(...))
    extern bool textureOn;
    extern std::vector<vertex> vertices;
    extern int vertexIter;             //Keep track of # of vertices in current frame
    extern GLuint vboID;
    extern GLuint programID;
    extern GLuint vshadeID;
    extern GLuint fshadeID;
    extern GLuint positionAttrib;
    extern GLuint colorAttrib;
    extern GLuint texcoordAttrib;
    extern int preloadSize;   //Number of vertices to reserve initially
    extern int vboSize;                //Size of allocated space in VRAM
    extern vertex stateVertex;         //use to best imitate immediate mode vertices

}

#endif