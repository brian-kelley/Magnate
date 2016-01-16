#include "ImmediateDraw.h"

using namespace std;

ImmediateDraw::ImmediateDraw(int quadInit, int lineInit) : vbo(quadInit * 4 + lineInit * 2, VBO::v2D, GL_STREAM_DRAW), quadVertices(quadInit), lineVertices(lineInit)
{
    state.pos = Pos2(0, 0);
    state.color = {255, 255, 255, 255};
    state.texcoord = {-1, -1};
    texturesEnabled = false;
    beginFrame();       //zero out quad/line indices
}

void ImmediateDraw::beginFrame()
{
    quadIndex = 0;
    lineIndex = 0;
    clipMarkers.clear();
}

void ImmediateDraw::draw()
{
    //send data to GPU
    int numVertices = quadIndex + lineIndex;
    //make sure VBO is big enough (do not shrink if there is extra space)
    if(numVertices > vbo.getNumVertices())
        vbo.resize(numVertices);
    vbo.writeData(0, quadIndex, &quadVertices[0]);
    vbo.writeData(quadIndex, lineIndex, &lineVertices[0]);
    vbo.drawWithClip(0, quadIndex, GL_QUADS, clipMarkers);
    //TODO: add another vector of clip markers for lines if line clipping becomes necessary for UI
    vbo.draw(quadIndex, lineIndex, GL_LINES);
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
    state.color = Color4(r * 255, g * 255, b * 255, a * 255);
}

void ImmediateDraw::color4b(u8 r, u8 g, u8 b, u8 a)
{
    state.color = Color4(r, g, b, a);
}

void ImmediateDraw::vertex2i(short x, short y)
{
    if(!texturesEnabled)
        state.texcoord = {-1, -1};
    state.pos = {x, y};
    if(quadIndex < quadVertices.size())
        quadVertices[quadIndex] = state;
    else
        quadVertices.push_back(state);
}

void ImmediateDraw::lineVertex2i(short x, short y)
{
    state.texcoord = {-1, -1};
    state.pos = {x, y};
    if(lineIndex < lineVertices.size())
        lineVertices[lineIndex] = state;
    else
        lineVertices.push_back(state);
}

void ImmediateDraw::texCoord2f(float u, float v)
{
    state.texcoord = TexCoord(u, v);
}

void ImmediateDraw::texCoord2i(short u, short v)
{
    state.texcoord = {u, v};
}

void ImmediateDraw::enableTextures()
{
    texturesEnabled = true;
}

void ImmediateDraw::disableTextures()
{
    texturesEnabled = false;
}

void ImmediateDraw::drawString(std::string text, int x, int y, int w, int h)
{
    for(int i = 0; i < text.size(); i++)
    {
        genericBlitChar(text[i], {x + w * i, y, w, h});
    }
}

void ImmediateDraw::drawStringScaled(std::string text, Rectangle dest)
{
    int glyphW = 0.5 + double(dest.w) / text.length();
    for(int i = 0; i < text.size(); i++)
    {
        genericBlitChar(text[i], {dest.x + glyphW * i, dest.y, glyphW, dest.h});
    }
}

void ImmediateDraw::drawStringAuto(std::string text, Rectangle dest, Justify just)
{
    //first, find font scale factor
    
}

void ImmediateDraw::blit(int texID, int x, int y)
{
    auto tex = Atlas::textureFromID(texID);
    genericTexturedQuad(tex, {x, y, tex.width, tex.height});
}

void ImmediateDraw::blit(std::string texName, int x, int y)
{
    auto tex = Atlas::textureFromName(texName);
    genericTexturedQuad(tex, {x, y, tex.width, tex.height});
}

void ImmediateDraw::blit(int texID, int x1, int y1, int x2, int y2)
{
    auto tex = Atlas::textureFromID(texID);
    genericTexturedQuad(tex, {x1, y1, x2 - x1, y2 - y1});
}

void ImmediateDraw::genericTexturedQuad(const Texture &tex, Rectangle dest)
{
    texCoord2i(tex.x, tex.y);
    vertex2i(dest.x, dest.y);
    texCoord2i(tex.x + tex.width, tex.y);
    vertex2i(dest.x + dest.w, dest.y);
    texCoord2i(tex.x + tex.width, tex.y + tex.height);
    vertex2i(dest.x + dest.w, dest.y + dest.h);
    texCoord2i(tex.x, tex.y + tex.height);
    vertex2i(dest.x, dest.y + dest.h);
}

void ImmediateDraw::genericBlitChar(char c, Rectangle dest)
{
    auto tex = Atlas::textureFromID(Atlas::tileFromChar(c));
    genericTexturedQuad(tex, dest);
}