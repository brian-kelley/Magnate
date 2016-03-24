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
    clipState.numVertices = 0;
    clipState.enable = false;
}

void ImmediateDraw::draw()
{
    //terminate last clip marker
    clipMarkers.push_back(clipState);
    //send data to GPU
    int numVertices = quadIndex + lineIndex;
    //make sure VBO is big enough (do not shrink if there is extra space)
    if(numVertices > vbo.getNumVertices())
        vbo.resize(numVertices);
    PRINT("Writing " << quadIndex << " vertices to vbo");
    vbo.writeData(0, quadIndex, &quadVertices[0]);
    vbo.writeData(quadIndex, lineIndex, &lineVertices[0]);
    vbo.drawWithClip(0, quadIndex, GL_QUADS, clipMarkers);
    //add another vector of clip markers for lines if line clipping becomes necessary for UI
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
    if(quadIndex < int(quadVertices.size()))
        quadVertices[quadIndex] = state;
    else
        quadVertices.push_back(state);
    quadIndex++;
    clipState.numVertices++;
}

void ImmediateDraw::lineVertex2i(short x, short y)
{
    state.texcoord = {-1, -1};
    state.pos = {x, y};
    if(lineIndex < int(lineVertices.size()))
        lineVertices[lineIndex] = state;
    else
        lineVertices.push_back(state);
    lineIndex++;
}

void ImmediateDraw::lineVertex2i(Pos2 pos)
{
    lineVertex2i(pos.x, pos.y);
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

void ImmediateDraw::drawString(std::string& text, int x, int y, int w, int h)
{
    for(int i = 0; i < int(text.size()); i++)
    {
        genericBlitChar(text[i], {x + w * i, y, w, h});
    }
}

void ImmediateDraw::drawStringScaled(std::string& text, Rectangle dest)
{
    int glyphW = 0.5 + double(dest.w) / text.length();
    for(int i = 0; i < int(text.size()); i++)
    {
        genericBlitChar(text[i], {dest.x + glyphW * i, dest.y, glyphW, dest.h});
    }
}

void ImmediateDraw::drawStringAuto(std::string& text, Rectangle dest, Justify just)
{
    //decide which direciton is constraint
    int fw = Atlas::getFontW();
    int fh = Atlas::getFontH();
    float scale;
    {
        float heightScale = float(dest.h) / fh;
        float widthScale = float(dest.w) / (fw * text.length());
        scale = min(heightScale, widthScale);
    }
    int charW = scale * fw;
    int charH = scale * fh;
    dest.y = dest.y + dest.h / 2 - charH / 2;
    if(just == Justify::LEFT_JUST)
    {
        drawString(text, dest.x, dest.y, charW, charH);
    }
    else if(just == Justify::CENTER_JUST)
    {
        int leftStart = dest.x + dest.w / 2 - (text.length() * charW) / 2;
        drawString(text, leftStart, dest.y, charW, charH);
    }
}

void ImmediateDraw::drawStringAuto(std::string& text, Rectangle dest, Color4 color, Justify just)
{
    setColor(color);
    drawStringAuto(text, dest, just);
}

void ImmediateDraw::blit(int texID, int x, int y)
{
    auto& tex = Atlas::textureFromID(texID);
    genericTexturedQuad(tex, {x, y, tex.width, tex.height});
}

void ImmediateDraw::blit(std::string texName, int x, int y)
{
    auto& tex = Atlas::textureFromName(texName);
    genericTexturedQuad(tex, {x, y, tex.width, tex.height});
}

void ImmediateDraw::blit(int texID, int x1, int y1, int x2, int y2)
{
    auto& tex = Atlas::textureFromID(texID);
    genericTexturedQuad(tex, {x1, y1, x2 - x1, y2 - y1});
}

void ImmediateDraw::blit(int texID, Rectangle rect)
{
    auto& tex = Atlas::textureFromID(texID);
    genericTexturedQuad(tex, rect);
}

void ImmediateDraw::blit(std::string texName, Rectangle rect)
{
    blit(Atlas::tileFromName(texName), rect.x, rect.y, rect.x + rect.w, rect.y + rect.h);
}

void ImmediateDraw::blit(std::string texName, int x1, int y1, int x2, int y2)
{
    blit(Atlas::tileFromName(texName), x1, y1, x2, y2);
}

void ImmediateDraw::genericTexturedQuad(const Texture &tex, Rectangle dest)
{
    enableTextures();
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
    if(c != ' ')
    {
        enableTextures();
        blit(Atlas::tileFromChar(c), dest);
    }
}

void ImmediateDraw::drawRect(Color4 color, Rectangle rect)
{
    disableTextures();
    color4b(color.r, color.g, color.b, color.a);
    vertex2i(rect.x, rect.y);
    vertex2i(rect.x + rect.w, rect.y);
    vertex2i(rect.x + rect.w, rect.y + rect.h);
    vertex2i(rect.x, rect.y + rect.h);
}

void ImmediateDraw::drawLineRect(Color4 color, Rectangle rect)
{
    disableTextures();
    color4b(color.r, color.g, color.b, color.a);
    lineVertex2i(rect.x, rect.y);
    lineVertex2i(rect.x + rect.w, rect.y);
    lineVertex2i(rect.x + rect.w, rect.y);
    lineVertex2i(rect.x + rect.w, rect.y + rect.h);
    lineVertex2i(rect.x + rect.w, rect.y + rect.h);
    lineVertex2i(rect.x, rect.y + rect.h);
    lineVertex2i(rect.x, rect.y + rect.h);
    lineVertex2i(rect.x, rect.y);
}

void ImmediateDraw::drawBevelFrame(Color4 light, Color4 dark, Rectangle rect, int borderWidth)
{
    disableTextures();
    color3b(light.r, light.g, light.b);
    //top bevel
    vertex2i(rect.x, rect.y);
    vertex2i(rect.x + rect.w, rect.y);
    vertex2i(rect.x + rect.w - borderWidth, rect.y + borderWidth);
    vertex2i(rect.x + borderWidth, rect.y + borderWidth);
    //left bevel
    vertex2i(rect.x, rect.y);
    vertex2i(rect.x + borderWidth, rect.y + borderWidth);
    vertex2i(rect.x + borderWidth, rect.y + rect.h - borderWidth);
    vertex2i(rect.x, rect.y + rect.h);
    color3b(dark.r, dark.g, dark.b);
    //right bevel
    vertex2i(rect.x + rect.w, rect.y);
    vertex2i(rect.x + rect.w, rect.y + rect.h);
    vertex2i(rect.x + rect.w - borderWidth, rect.y + rect.h - borderWidth);
    vertex2i(rect.x + rect.w - borderWidth, rect.y + borderWidth);
    //bottom bevel
    vertex2i(rect.x + borderWidth, rect.y + rect.h - borderWidth);
    vertex2i(rect.x + rect.w - borderWidth, rect.y + rect.h - borderWidth);
    vertex2i(rect.x + rect.w, rect.y + rect.h);
    vertex2i(rect.x, rect.y + rect.h);
}

void ImmediateDraw::setColor(Color4 color)
{
    color4b(color.r, color.g, color.b, color.a);
}

void ImmediateDraw::enableScissorTest()
{
    //initialize clip marker at current quad index
    //note: client code should set clip rectangle before this function
    if(clipState.numVertices > 0)
    {
        clipMarkers.push_back(clipState);
        clipState.numVertices = 0;
    }
    clipState.enable = true;
}

void ImmediateDraw::disableScissorTest()
{
    //terminate clip marker and add to list
    clipMarkers.push_back(clipState);
    clipState.numVertices = 0;
    clipState.enable = false;
}

void ImmediateDraw::scissorRect(Rectangle rect)
{
    if(rect != clipState.bounds)
    {
        clipMarkers.push_back(clipState);
        clipState.numVertices = 0;
    }
    clipState.bounds = rect;
}
