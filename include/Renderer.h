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
    float x;    //Position on screen (pixels)
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
    void addQuadVertex();
    void addLineVertex();
    //Vertex attributes interleaved: xyrgbauvxyrgbauv...
    void color3f(float r, float g, float b);
    void color4f(float r, float g, float b, float a);
    void vertex2i(int x, int y);
    void lineVertex2i(int x, int y);
    void texCoord2f(float u, float v);
    void enableTexture();       //use uniform variable to enable/disable
    void disableTexture();      //2d texture blitting (like glEnable(...))
    extern bool textureOn;
    extern std::vector<vertex> quadVertices;
    extern int numQuadVertices;             //Keep track of # of vertices in current frame
    extern std::vector<vertex> lineVertices;
    extern int numLineVertices;
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