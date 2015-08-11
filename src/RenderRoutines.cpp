//
//  RenderRoutines.cpp
//  Magnate
//
//  Created by Brian Kelley on 3/5/1564.
//
//

#include "RenderRoutines.h"
#include <stdio.h>

using namespace std;
using namespace constants;
using namespace RenderRoutines;
using namespace Renderer;

void RenderRoutines::initAtlas(SDL_Renderer* windowRenderer)
{
    int imgFlags = IMG_INIT_PNG;
    if(!(IMG_Init(IMG_INIT_PNG) & imgFlags))
    {
        cout << "Fatal error: View failed to initialize image loading." << endl;
        cout << "Not going to attempt loading main atlas." << endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        Atlas::init("main", windowRenderer);
    }
}

void RenderRoutines::drawString(string text, int x, int y)
{
    drawString(text, x, y, 1, 0, 0, 0);
}

void RenderRoutines::drawString(string text, int x, int y, float scale)
{
    drawString(text, x, y, scale, 0, 0, 0);
}

void RenderRoutines::drawString(string text, int x, int y, float scale, float r, float g, float b)
{
    color3f(r, g, b);
    glEnable(GL_BLEND);
    for(int i = 0; i < text.size(); i++)
    {
        if(text[i] != ' ')
        {
            blit(Atlas::tileFromChar(text[i]),
                 x + i * constants::FONTW * scale, y,
                 x + (1 + i) * constants::FONTW * scale, y + constants::FONTH * scale);
        }
    }
}

void RenderRoutines::isoBlit(int tex, double i, double j, unsigned char h1, unsigned char h2, unsigned char h3, unsigned char h4)
{
}

void RenderRoutines::blit(int index, int x, int y)
{
    blit(index, x, y, x + Atlas::tileW(index), y + Atlas::tileH(index));
}

void RenderRoutines::blit(int index, int x1, int y1, int x2, int y2)
{
    texCoord2i(Atlas::tileX(index), Atlas::tileY(index));
    vertex2i(x1, y1);
    texCoord2i(Atlas::tileX(index) + Atlas::tileW(index),
                 Atlas::tileY(index));
    vertex2i(x2, y1);
    texCoord2i(Atlas::tileX(index) + Atlas::tileW(index),
                 Atlas::tileY(index) + Atlas::tileH(index));
    vertex2i(x2, y2);
    texCoord2i(Atlas::tileX(index),
                 Atlas::tileY(index) + Atlas::tileH(index));
    vertex2i(x1, y2);
}

void RenderRoutines::drawCuboid(Cuboid& c)
{
    //TODO - add buildings to the building VBO only when needed
}

int RenderRoutines::texNumFromStr(std::string name)
{
    return Atlas::tileFromName(name);
}

floatRect_t RenderRoutines::getTexCoords(int index)
{
    floatRect_t ret;
    ret.x = Atlas::tileX(index);
    ret.y = Atlas::tileY(index);
    ret.w = Atlas::tileW(index);
    ret.h = Atlas::tileH(index);
    return ret;
}

//Draw a line using a quad
void RenderRoutines::drawLine(int x1, int y1, int x2, int y2)
{
    disableTexture();
    if(abs(x2 - x1) < abs(y2 - y1))
    {
        // dx/dy small
        vertex2i(x1, y1);
        vertex2i(x1 + 1, y1);
        vertex2i(x2 + 1, y2);
        vertex2i(x2, y2);
    }
    else
    {
        // dy/dx <= 1
        vertex2i(x1, y1);
        vertex2i(x1, y1 + 1);
        vertex2i(x2, y2 + 1);
        vertex2i(x2, y2);
    }
}