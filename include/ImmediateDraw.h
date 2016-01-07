#ifndef __IMMEDIATE_DRAW_H__
#define __IMMEDIATE_DRAW_H__

#include <iostream>
#include <vector>
#include "GenTypes.h"
#include "SdlHeaders.h"
#include "DebugTools.h"
#include "Atlas.h"

class ImmediateDraw
{
public:
    ImmediateDraw(const Atlas* mainAtlas);
    void* getQuadBuffer();
    void* getLineBuffer();
    int getQuadVertices();
    int getLineVertices();
    void color3f(float r, float g, float b);
    void color3b(u8 r, u8 g, u8 b);
    void color4f(float r, float g, float b, float a);
    void color4b(u8 r, u8 g, u8 b, u8 a);
    void vertex2i(short x, short y);
    void lineVertex2i(short x, short y);
    void texCoord2f(float u, float v);
    void texCoord2i(short u, short v);
private:
    const Atlas* atlas;
    std::vector<Vertex2D> quadVertices;
    std::vector<Vertex2D> lineVertices;
    Vertex2D state;
};

#endif