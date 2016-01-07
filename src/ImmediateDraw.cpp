#include "ImmediateDraw.h"

using namespace std;

ImmediateDraw::ImmediateDraw(const Atlas* mainAtlas)
{
    this->atlas = mainAtlas;
    state.pos = Pos2(0, 0);
    state.color = {255, 255, 255, 255};
    state.texcoord = {-1, -1};
}

void* ImmediateDraw::getQuadBuffer()
{
    return &quadVertices[0];
}

int ImmediateDraw::getQuadVertices()
{
    return quadVertices.size();
}

void* ImmediateDraw::getLineBuffer()
{
    return &lineVertices[0];
}

int ImmediateDraw::getLineVertices()
{
    return lineVertices.size();
}

void ImmediateDraw::color3f(float r, float g, float b)
{
    state.color = Color4(r * 255, g * 255, b * 255, 255);
}

void ImmediateDraw::color3b(u8 r, u8 g, u8 b)
{
    state.color = {r, g, b, 255};
}

void ImmediateDraw::color4f(float r, float g, float b, float a)
{
    
}

void ImmediateDraw::color4b(u8 r, u8 g, u8 b, u8 a)
{
    
}

void ImmediateDraw::vertex2i(short x, short y)
{
    
}

void ImmediateDraw::lineVertex2i(short x, short y)
{
    
}

void ImmediateDraw::texCoord2f(float u, float v)
{
    
}

void ImmediateDraw::texCoord2i(short u, short v)
{
    
}