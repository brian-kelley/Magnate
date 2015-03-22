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

Atlas* RenderRoutines::mainAtlas;

void RenderRoutines::initAtlas(SDL_Renderer* windowRenderer)
{
    int imgFlags = IMG_INIT_PNG;
    if(!(IMG_Init(IMG_INIT_PNG) & imgFlags))
    {
        cout << "Fatal error: View failed to initialize image loading." << endl;
        cout << "Not going to attempt loading main atlas." << endl;
        exit(41);
    }
    else
    {
        mainAtlas = new Atlas("main", windowRenderer);
        if(mainAtlas == nullptr)
        {
            cout << "Fatal error: Failed to create atlas." << endl;
            exit(42);
        }
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
    glColor3f(r, g, b);
    glEnable(GL_BLEND);
    for(int i = 0; i < text.size(); i++)
    {
        if(text[i] != ' ')
        {
            blit(mainAtlas->tileFromChar(text[i]),
                 x + i * constants::FONTW * scale, y,
                 x + (1 + i) * constants::FONTW * scale, y + constants::FONTH * scale);
        }
    }
}

void RenderRoutines::isoBlit(int tex, double i, double j, unsigned char h1, unsigned char h2, unsigned char h3, unsigned char h4)
{
    glEnable(GL_TEXTURE_2D);
    float shading = calcTileShade(h1, h2, h3, h4);
    glColor3f(shading, shading, shading);
    floatRect_t srcRect;
    srcRect.x = mainAtlas->tileX(tex);
    srcRect.y = mainAtlas->tileY(tex);
    srcRect.w = mainAtlas->tileW(tex);
    srcRect.h = mainAtlas->tileH(tex);
    glBegin(GL_QUADS);
    glTexCoord2f(srcRect.x, srcRect.y);
    Point destPt = coord::project3DPoint(i, j, float(h1) / coord::ISO_HEIGHT);
    glVertex2i(destPt.x, destPt.y);
    glTexCoord2f(srcRect.x + srcRect.w, srcRect.y);
    destPt = coord::project3DPoint(i, j + TERRAIN_TILE_SIZE, float(h2) / coord::ISO_HEIGHT);
    glVertex2i(destPt.x, destPt.y);
    glTexCoord2f(srcRect.x + srcRect.w, srcRect.y + srcRect.h);
    destPt = coord::project3DPoint(i + TERRAIN_TILE_SIZE, j + TERRAIN_TILE_SIZE, float(h3) / coord::ISO_HEIGHT);
    glVertex2i(destPt.x, destPt.y);
    glTexCoord2f(srcRect.x, srcRect.y + srcRect.h);
    destPt = coord::project3DPoint(i + TERRAIN_TILE_SIZE, j, float(h4) / coord::ISO_HEIGHT);
    glVertex2i(destPt.x, destPt.y);
    glEnd();
}

void RenderRoutines::blit(int index, int x, int y)
{
    blit(index, x, y, x + mainAtlas->getSize() * mainAtlas->tileW(index), y + mainAtlas->getSize() * mainAtlas->tileH(index));
}

void RenderRoutines::blit(int index, int x1, int y1, int x2, int y2)
{
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glTexCoord2f(mainAtlas->tileX(index), mainAtlas->tileY(index));
    glVertex2i(x1, y1);
    glTexCoord2f(mainAtlas->tileX(index) + mainAtlas->tileW(index),
                 mainAtlas->tileY(index));
    glVertex2i(x2, y1);
    glTexCoord2f(mainAtlas->tileX(index) + mainAtlas->tileW(index),
                 mainAtlas->tileY(index) + mainAtlas->tileH(index));
    glVertex2i(x2, y2);
    glTexCoord2f(mainAtlas->tileX(index),
                 mainAtlas->tileY(index) + mainAtlas->tileH(index));
    glVertex2i(x1, y2);
    glEnd();
}

void RenderRoutines::drawCuboid(Cuboid& c)
{
    int posX = coord::ix(c.getX(), c.getY()) - screenX;
    int posY = coord::jy(c.getX(), c.getY()) + (int) (constants::HMULT * c.getZ()) - screenY;
    //Left wall
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(mainAtlas->tileX(c.getLeft()),
                 mainAtlas->tileY(c.getLeft()));
    glVertex2i(posX + c.draw.x2, posY + c.draw.y2);
    glTexCoord2f(mainAtlas->tileX(c.getLeft()) + mainAtlas->tileW(c.getLeft()),
                 mainAtlas->tileY(c.getLeft()));
    glVertex2i(posX + c.draw.x3, posY + c.draw.y3);
    glTexCoord2f(mainAtlas->tileX(c.getLeft()) + mainAtlas->tileW(c.getLeft()),
                 mainAtlas->tileY(c.getLeft()) + mainAtlas->tileH(c.getLeft()));
    glVertex2i(posX + c.draw.x4, posY + c.draw.y4);
    glTexCoord2f(mainAtlas->tileX(c.getLeft()) + mainAtlas->tileW(c.getLeft()),
                 mainAtlas->tileY(c.getLeft()) + mainAtlas->tileH(c.getLeft()));
    glVertex2i(posX, posY);
    glEnd();
    //Roof
    glBegin(GL_QUADS);
    glTexCoord2f(mainAtlas->tileX(c.getRoof()),
                 mainAtlas->tileY(c.getRoof()));
    glVertex2i(posX + c.draw.x2, posY + c.draw.y2);
    glTexCoord2f(mainAtlas->tileX(c.getRoof()) + mainAtlas->tileW(c.getRoof()),
                 mainAtlas->tileY(c.getRoof()));
    glVertex2i(posX + c.draw.x5, posY + c.draw.y5);
    glTexCoord2f(mainAtlas->tileX(c.getRoof()) + mainAtlas->tileW(c.getRoof()),
                 mainAtlas->tileY(c.getRoof()) + mainAtlas->tileH(c.getRoof()));
    glVertex2i(posX + c.draw.x6, posY + c.draw.y6);
    glTexCoord2f(mainAtlas->tileX(c.getRoof()),
                 mainAtlas->tileY(c.getRoof()) + mainAtlas->tileH(c.getRoof()));
    glVertex2i(posX + c.draw.x3, posY + c.draw.y3);
    glEnd();
    //Right wall
    glColor3f(constants::SHADE, constants::SHADE, constants::SHADE);
    glBegin(GL_QUADS);
    glTexCoord2f(mainAtlas->tileX(c.getRight()),
                 mainAtlas->tileY(c.getRight()));
    glVertex2i(posX + c.draw.x3, posY + c.draw.y3);
    glTexCoord2f(mainAtlas->tileX(c.getRight()) + mainAtlas->tileW(c.getRight()),
                 mainAtlas->tileY(c.getRight()));
    glVertex2i(posX + c.draw.x6, posY + c.draw.y6);
    glTexCoord2f(mainAtlas->tileX(c.getRight()) + mainAtlas->tileW(c.getRight()),
                 mainAtlas->tileY(c.getRight()) + mainAtlas->tileH(c.getRight()));
    glVertex2i(posX + c.draw.x7, posY + c.draw.y7);
    glTexCoord2f(mainAtlas->tileX(c.getRight()),
                 mainAtlas->tileY(c.getRight()) + mainAtlas->tileH(c.getRight()));
    glVertex2i(posX + c.draw.x4, posY + c.draw.y4);
    glEnd();
}

int RenderRoutines::texNumFromStr(std::string name)
{
    return mainAtlas->tileFromName(name);
}

floatRect_t RenderRoutines::getTexCoords(int index)
{
    floatRect_t ret;
    ret.x = mainAtlas->tileX(index);
    ret.y = mainAtlas->tileY(index);
    ret.w = mainAtlas->tileW(index);
    ret.h = mainAtlas->tileH(index);
    return ret;
}

float RenderRoutines::calcTileShade(unsigned char h1, unsigned char h2, unsigned char h3, unsigned char h4)
{
    return 1.0 - 0.5 * (abs(h1 - h2) + abs(h2 - h3) + abs(h3 - h4) + abs(h4 - h1)) / coord::ISO_HEIGHT;
}