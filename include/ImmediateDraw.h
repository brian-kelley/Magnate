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
    //Simpler, abstracted drawing functions
    void setColor(Color4 color);
    void drawRect(Color4 color, Rectangle rect);
    void drawLineRect(Color4 color, Rectangle rect);
    void drawBevelFrame(Color4 light, Color4 dark, Rectangle rect, int borderWidth);
    void drawString(std::string& text, int x, int y, int w, int h);  //(x,y) is the upper-left of the string, and w/h are for each glyph.
    void drawStringScaled(std::string& text, Rectangle dest);
    void drawStringAuto(std::string& text, Rectangle dest, Justify just = Justify::CENTER_JUST);  //chooses one scaling factor and justifies
    void drawStringAuto(std::string& text, Rectangle dest, Color4 color, Justify just = Justify::CENTER_JUST);
    //Low-level drawing functions
    void enableScissorTest();
    void disableScissorTest();
    void scissorRect(Rectangle rect);
    void color3f(float r, float g, float b);
    void color3b(u8 r, u8 g, u8 b);
    void color4f(float r, float g, float b, float a);
    void color4b(u8 r, u8 g, u8 b, u8 a);
    void vertex2i(short x, short y);
    void lineVertex2i(short x, short y);
    void texCoord2f(float u, float v);
    void texCoord2i(short u, short v);
    void blit(int texID, int x, int y);   //blit at actual size
    void blit(std::string texName, int x, int y);
    void blit(int texID, int x1, int y1, int x2, int y2); //blit, but specify dest rect
    void blit(std::string texName, int x1, int y1, int x2, int y2);
    void blit(std::string texName, Rectangle rect);
    void enableTextures();
    void disableTextures();
    void draw();    //called by owner in its draw function
private:
    VBO vbo;
    std::vector<Vertex2D> quadVertices;
    std::vector<Vertex2D> lineVertices;
    std::vector<ClipMarker> clipMarkers;
    int quadIndex;
    int lineIndex;
    Vertex2D state;
    ClipMarker clipState;
    bool texturesEnabled;
    //utility functions
    void genericTexturedQuad(const Texture& tex, Rectangle dest);
    void genericBlitChar(char c, Rectangle dest);
};

#endif