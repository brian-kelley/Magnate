#ifndef __RENDERER__INCLUDED__
#define __RENDERER__INCLUDED__

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#ifdef __APPLE__
#include "gl.h"
#elif _WIN32
#include "GL/gl.h"
#endif

#include <vector>
#include <string>
#include <iostream>
#include "Constants.h"
#include "Coord.h"

/*
 Vertex fields/sizes
 (SOA) - all positions, all colors, all texcoords in one VBO
 2D:
 -short vec2 pos                (normalized to [-1, 1])
 -unsigned char vec4 color      (normalized to [0, 1])
 -short vec2 texcoord  (normalized to [-1, 1])
 Total: 12 bytes/vertex!
 
 3D:
 -float vec3 pos
 -unsigned char vec4 color
 -short vec2 texcoord
 Total: 20 bytes/vertex
 */

typedef struct
{
    short u;
    short v;
} TexCoord;

typedef struct
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} Color4;

typedef struct
{
    Color4 color;
    TexCoord texcoord;
    Pos2 pos;
} Vertex2D; //12 bytes total

typedef struct
{
    Color4 color;
    TexCoord texcoord;
    Pos3 pos;
} Vertex3D; //20 bytes total

namespace Renderer
{
    void init();
    void dispose();
    void startFrame();
    void endFrame();
    void color3f(float r, float g, float b);
    void color3b(unsigned char r, unsigned char g, unsigned char b);
    void color4f(float r, float g, float b, float a);
    void color4b(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
    void vertex2i(short x, short y);
    void lineVertex2i(short x, short y);
    void texCoord2f(float u, float v);
    void texCoord2i(short u, short v);
    void enableTexture();
    void disableTexture();
    void getFrustumCorners(double* arr); //get world-space intersectioon of four corners of screen with y = 0
    void update2DMatrices();                 //only call during init
    void updatePerspectiveMatrix();          //when FOV or window changes
    void updateViewMatrix();                 //when camera moves
    void uploadMatrices(int dims); //before draw call, 2 or 3
    /* Internal functions/data */
    void initVBO();
    void initShaders();
    void setupWorldVBO();
    void setupBuildingVBO();
    void setupGuiVBO();
    void bindWorldVBO();
    void bindBuildingVBO();
    void bindGuiVBO();
    //data for easy immediate-mode like GUI drawing
    extern std::vector<Vertex2D> guiQuadVertices;
    extern std::vector<Vertex2D> guiLineVertices;
    extern bool textureOn;
    extern int numWorldVertices;
    extern int numBuildingVertices;
    extern int numGuiLineVertices;
    extern int numGuiQuadVertices;
    extern GLuint worldVBO;
    extern GLuint buildingVBO;
    extern GLuint guiVBO;
    extern int worldVBOSize;
    extern int buildingVBOSize;
    extern int guiVBOSize;     //(in vertices, not bytes)
    extern GLuint programID;  //information about shaders
    extern GLuint vshadeID;
    extern GLuint fshadeID;
    extern GLuint posAttribLoc;
    extern GLuint colorAttribLoc;
    extern GLuint texCoordAttribLoc;
    extern glm::mat4 proj2;
    extern glm::mat4 view2;
    extern glm::mat4 proj3;
    extern glm::mat4 view3;
    extern glm::vec3 camUp;
    extern glm::vec3 camDir;
    extern GLuint projLoc;
    extern GLuint viewLoc;
    extern GLuint modelLoc;
    extern Vertex2D stateVertex;
    const int GUI_QUAD_PRELOAD = 400; //How many vertices to reserve() in
    const int GUI_LINE_PRELOAD = 10;  //GUI quad and line attrib vectors
    void addQuadVertex();
    void addLineVertex();
}
#endif