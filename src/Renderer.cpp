#include "Renderer.h"

using namespace std;
using namespace glm;
using namespace Coord;

Renderer::Renderer() :
window(640, 480),
shaders(),
mainAtlas("main", window.getRenderer()),
vboMan(shaders.getProgramID()),
imm(<#args#>),
guiRend(<#args#>),
worldRend(<#args#>)
{
    textureOn = false;
    initShaders();
    //For now, model matrix in both dimensions is I
    stateVertex.pos = {0, 0};
    //    stateVertex.z = 0;
    stateVertex.texcoord = {0, 0};
    stateVertex.color = {255, 255, 255, 255};
    //Set initial camera position and direction
    update2DMatrices();
    update3DMatrices();
    initVBO();
    numWorldVertices = VBO_CHUNKS * CHUNK_SIZE * CHUNK_SIZE * 4;
}

void Renderer::prepareFrame()
{
    numGuiLineVertices = 0;
    numGuiQuadVertices = 0;
}

void Renderer::endFrame()
{
    
    uploadMatrices(3);
    //draw world VBO
    bindWorldVBO();
    glEnable(GL_DEPTH_TEST);
    glDrawArrays(GL_QUADS, 0, numWorldVertices);
    bindGuiVBO();
    uploadMatrices(2);
    int totalVertices = numGuiQuadVertices + numGuiLineVertices;
    if(sizeof(Vertex2D) * totalVertices > guiVBOSize)
    {
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex2D) * totalVertices, NULL, GL_STREAM_DRAW);
        guiVBOSize = sizeof(Vertex2D) * totalVertices;
    }
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex2D) * numGuiQuadVertices, &guiQuadVertices.front());
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertex2D) * numGuiQuadVertices, sizeof(Vertex2D) * numGuiLineVertices, &guiLineVertices.front());
    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_QUADS, 0, numGuiQuadVertices);
    glDrawArrays(GL_LINES, numGuiQuadVertices, numGuiLineVertices);
}

void Renderer::color3f(float r, float g, float b)
{
    stateVertex.color.r = r * 255;
    stateVertex.color.g = g * 255;
    stateVertex.color.b = b * 255;
    stateVertex.color.a = 255;
}

void Renderer::color3b(unsigned char r, unsigned char g, unsigned char b)
{
    stateVertex.color.r = r;
    stateVertex.color.g = g;
    stateVertex.color.b = b;
    stateVertex.color.a = 255;
}

void Renderer::color4f(float r, float g, float b, float a)
{
    stateVertex.color.r = r * 255;
    stateVertex.color.g = g * 255;
    stateVertex.color.b = b * 255;
    stateVertex.color.a = a * 255;
}

void Renderer::color4b(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    stateVertex.color.r = r;
    stateVertex.color.g = g;
    stateVertex.color.b = b;
    stateVertex.color.a = a;
}

void Renderer::vertex2i(short x, short y)
{
    stateVertex.pos = {x, y};
    addQuadVertex();
}

void Renderer::lineVertex2i(short x, short y)
{
    stateVertex.pos = {x, y};
    addLineVertex();
}

void Renderer::texCoord2i(short u, short v)
{
    stateVertex.texcoord = {u, v};
    textureOn = true;
}

void Renderer::texCoord2f(float u, float v)
{
    short newU = u * ATLAS_SIZE;
    short newV = v * ATLAS_SIZE;
    stateVertex.texcoord = {newU, newV};
    textureOn = true;
}

void Renderer::addQuadVertex()
{
    if(!textureOn)
    {
        //Shaders will know that these coords mean to not use texture2D for color
        stateVertex.texcoord.u = -1;
    }
    //else: texcoord must have already been set by program
    if(guiQuadVertices.size() > numGuiQuadVertices)
    {
        guiQuadVertices[numGuiQuadVertices] = stateVertex;
    }
    else
    {
        guiQuadVertices.push_back(stateVertex);
    }
    numGuiQuadVertices++;
}

void Renderer::addLineVertex()
{
    //Shaders will know that these coords mean to not use texture2D for color
    //else: texcoord must have already been set by program
    stateVertex.texcoord.u = -1;
    if(guiLineVertices.size() > numGuiLineVertices)
    {
        guiLineVertices[numGuiLineVertices] = stateVertex;
    }
    else
    {
        guiLineVertices.push_back(stateVertex);
    }
    numGuiLineVertices++;
}

void Renderer::enableTexture()
{
    textureOn = true;
}

void Renderer::disableTexture()
{
    textureOn = false;
}

void Renderer::update2DMatrices()
{
    proj2 = ortho<float>(0, WINDOW_W, WINDOW_H, 0, 1, -1);
    view2 = glm::mat4(); //ID
}

void Renderer::update3DMatrices()
{
    Coord::proj3 = glm::perspective<float>(FOV, float(WINDOW_W) / WINDOW_H, NEAR, FAR);
    Coord::view3 = Camera::getViewMatrix();
}

void Renderer::uploadMatrices(int dims)
{
    if(dims == 2)
    {
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view2));
        GLERR
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(proj2));
        GLERR
    }
    else
    {
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(Coord::view3));
        GLERR
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(Coord::proj3));
        GLERR
    }
}

void Renderer::bindWorldVBO()
{
    glBindBuffer(GL_ARRAY_BUFFER, worldVBO);
    glVertexAttribPointer(colorAttribLoc, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex3D), (GLvoid*) 0);
    glVertexAttribPointer(texCoordAttribLoc, 2, GL_SHORT, GL_FALSE, sizeof(Vertex3D), (GLvoid*) 4);
    glVertexAttribPointer(posAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*) 8);
    GLERR
}

void Renderer::bindBuildingVBO()
{
    glBindBuffer(GL_ARRAY_BUFFER, buildingVBO);
    glVertexAttribPointer(colorAttribLoc, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex3D), (GLvoid*) 0);
    glVertexAttribPointer(texCoordAttribLoc, 2, GL_SHORT, GL_FALSE, sizeof(Vertex3D), (GLvoid*) 4);
    glVertexAttribPointer(posAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*) 8);
    GLERR
}

void Renderer::bindGuiVBO()
{
    glBindBuffer(GL_ARRAY_BUFFER, guiVBO);
    glVertexAttribPointer(colorAttribLoc, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex2D), (GLvoid*) 0);
    glVertexAttribPointer(texCoordAttribLoc, 2, GL_SHORT, GL_FALSE, sizeof(Vertex2D), (GLvoid*) 4);
    glVertexAttribPointer(posAttribLoc, 2, GL_SHORT, GL_FALSE, sizeof(Vertex2D), (GLvoid*) 8);
    GLERR
}

vec2 Renderer::xzPlaneIntersect(vec3 pos, vec3 dir)
{
    float mult = pos.y / dir.y;
    return {pos.x + dir.x * mult, pos.z + dir.z * mult};
}

void Renderer::update(World& world, Component* guiMaster)
{
    
}