#ifndef __IMMEDIATE_DRAW_H__
#define __IMMEDIATE_DRAW_H__

#include <iostream>
#include <vector>
#include <string>
#include "GenTypes.h"
#include "SdlHeaders.h"
#include "DebugTools.h"
#include "Atlas.h"
#include "VBO.h"

enum struct Justify
{
    LEFT_JUST,
    CENTER_JUST
};

class ImmediateDraw
{
public:
    ImmediateDraw(int quadInit, int lineInit);
    void beginFrame();
    void color3f(float r, float g, float b);
    void color3b(u8 r, u8 g, u8 b);
    void color4f(float r, float g, float b, float a);
    void color4b(u8 r, u8 g, u8 b, u8 a);
    void vertex2i(short x, short y);
    void lineVertex2i(short x, short y);
    void texCoord2f(float u, float v);
    void texCoord2i(short u, short v);
    void drawString(std::string text, int x, int y, int w, int h);  //(x,y) is the upper-left of the string, and w/h are for each glyph.
    void drawStringScaled(std::string text, Rectangle dest);
    void drawStringAuto(std::string text, Rectangle dest, Justify just = Justify::CENTER_JUST);  //chooses one scaling factor and justifies
    void blit(int texID, int x, int y);   //blit at actual size
    void blit(std::string texName, int x, int y);
    void blit(int texID, int x1, int y1, int x2, int y2); //blit, but specify dest rect
    void blit(std::string texName, int x1, int y1, int x2, int y2);
    void enableTextures();
    void disableTextures();
    void draw();
private:
    VBO vbo;
    std::vector<Vertex2D> quadVertices;
    std::vector<Vertex2D> lineVertices;
    std::vector<ClipMarker> clipMarkers;
    int quadIndex;
    int lineIndex;
    Vertex2D state;
    bool texturesEnabled;
    //utility functions
    void genericTexturedQuad(const Texture& tex, Rectangle dest);
    void genericBlitChar(char c, Rectangle dest);
};

#endif