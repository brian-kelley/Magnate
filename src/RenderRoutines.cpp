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
    color3f(r, g, b);
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
    texCoord2f(mainAtlas->tileX(index), mainAtlas->tileY(index));
    vertex2i(x1, y1);
    texCoord2f(mainAtlas->tileX(index) + mainAtlas->tileW(index),
                 mainAtlas->tileY(index));
    vertex2i(x2, y1);
    texCoord2f(mainAtlas->tileX(index) + mainAtlas->tileW(index),
                 mainAtlas->tileY(index) + mainAtlas->tileH(index));
    vertex2i(x2, y2);
    texCoord2f(mainAtlas->tileX(index),
                 mainAtlas->tileY(index) + mainAtlas->tileH(index));
    vertex2i(x1, y2);
}

void RenderRoutines::drawCuboid(Cuboid& c)
{
    //Note: Assume roof texture "this side up" would point NW in the world
    switch(viewDirection)
    {
        case NORTH:
            drawCuboidNorth(c);
            break;
        case EAST:
            drawCuboidEast(c);
            break;
        case WEST:
            drawCuboidWest(c);
            break;
        case SOUTH:
            drawCuboidSouth(c);
            break;
    }
}

void RenderRoutines::drawCuboidNorth(Cuboid &c)
{
    color3f(1, 1, 1);   //No shading
    int swWall = c.getSEWall(); //Texture ID for southwest wall
    int seWall = c.getSWWall(); //Tex id for SE
    int roof = c.getRoof();
    int pxHeight = c.getHeight() * coord::ISO_HEIGHT;   //screen "height" of box
    Point westCorner = coord::project3DPoint(c.getX(), c.getY(), c.getZ());
    Point southCorner = coord::project3DPoint(c.getX() + c.getLength(), c.getY(), c.getZ());
    Point eastCorner = coord::project3DPoint(c.getX() + c.getLength(), c.getY() + c.getWidth(), c.getZ());
    //Draw SW wall
    texCoord2f(mainAtlas->tileX(swWall), mainAtlas->tileY(swWall) + mainAtlas->tileH(swWall));
    vertex2i(westCorner.x, westCorner.y);
    texCoord2f(mainAtlas->tileX(swWall), mainAtlas->tileY(swWall));
    vertex2i(westCorner.x, westCorner.y - pxHeight);
    texCoord2f(mainAtlas->tileX(swWall) + mainAtlas->tileW(swWall), mainAtlas->tileY(swWall));
    vertex2i(southCorner.x, southCorner.y - pxHeight);
    texCoord2f(mainAtlas->tileX(swWall) + mainAtlas->tileW(swWall), mainAtlas->tileY(swWall) + mainAtlas->tileH(swWall));
    vertex2i(southCorner.x, southCorner.y);
    //Shade the SE wall
    color3f(SHADE, SHADE, SHADE);
    //Draw SE wall
    texCoord2f(mainAtlas->tileX(seWall), mainAtlas->tileY(seWall) + mainAtlas->tileH(seWall));
    vertex2i(southCorner.x, southCorner.y);
    texCoord2f(mainAtlas->tileX(seWall), mainAtlas->tileY(seWall));
    vertex2i(southCorner.x, southCorner.y - pxHeight);
    texCoord2f(mainAtlas->tileX(seWall) + mainAtlas->tileW(seWall), mainAtlas->tileY(seWall));
    vertex2i(eastCorner.x, eastCorner.y - pxHeight);
    texCoord2f(mainAtlas->tileX(seWall) + mainAtlas->tileW(seWall), mainAtlas->tileY(seWall) + mainAtlas->tileH(seWall));
    vertex2i(eastCorner.x, eastCorner.y);
    //Draw roof
    color3f(1, 1, 1);
    texCoord2f(mainAtlas->tileX(roof), mainAtlas->tileY(roof));
    vertex2i(westCorner.x, westCorner.y - pxHeight);
    texCoord2f(mainAtlas->tileX(roof) + mainAtlas->tileW(roof), mainAtlas->tileY(roof));
    vertex2i(westCorner.x + (eastCorner.x - southCorner.x), eastCorner.y - (southCorner.y - westCorner.y) - pxHeight); //north point is a bit tricky but doesn't need project3dpoint
    texCoord2f(mainAtlas->tileX(roof) + mainAtlas->tileW(roof), mainAtlas->tileY(roof) + mainAtlas->tileH(roof));
    vertex2i(eastCorner.x, eastCorner.y - pxHeight);
    texCoord2f(mainAtlas->tileX(roof), mainAtlas->tileY(roof) + mainAtlas->tileH(roof));
    vertex2i(southCorner.x, southCorner.y - pxHeight);
}

void RenderRoutines::drawCuboidWest(Cuboid &c)
{
    //Facing west means that left = SE and right = NE
    color3f(SHADE, SHADE, SHADE);   //Both walls are in shadow
    int seWall = c.getSEWall();
    int neWall = c.getNEWall();
    int roof = c.getRoof();
    int pxHeight = c.getHeight() * coord::ISO_HEIGHT;   //screen "height" of box
    Point southCorner = coord::project3DPoint(c.getX() + c.getLength(), c.getY(), c.getZ());
    Point eastCorner = coord::project3DPoint(c.getX() + c.getLength(), c.getY() + c.getWidth(), c.getZ());
    Point northCorner = coord::project3DPoint(c.getX(), c.getY() + c.getWidth(), c.getZ());
    //Draw SE wall
    texCoord2f(mainAtlas->tileX(seWall), mainAtlas->tileY(seWall) + mainAtlas->tileH(seWall));
    vertex2i(southCorner.x, southCorner.y);
    texCoord2f(mainAtlas->tileX(seWall), mainAtlas->tileY(seWall));
    vertex2i(southCorner.x, southCorner.y - pxHeight);
    texCoord2f(mainAtlas->tileX(seWall) + mainAtlas->tileW(seWall), mainAtlas->tileY(seWall));
    vertex2i(eastCorner.x, eastCorner.y - pxHeight);
    texCoord2f(mainAtlas->tileX(seWall) + mainAtlas->tileW(seWall), mainAtlas->tileY(seWall) + mainAtlas->tileH(seWall));
    vertex2i(eastCorner.x, eastCorner.y);
    //Maintain the dark shading
    //Draw NE wall
    texCoord2f(mainAtlas->tileX(neWall), mainAtlas->tileY(neWall) + mainAtlas->tileH(neWall));
    vertex2i(eastCorner.x, eastCorner.y);
    texCoord2f(mainAtlas->tileX(neWall), mainAtlas->tileY(neWall));
    vertex2i(eastCorner.x, eastCorner.y - pxHeight);
    texCoord2f(mainAtlas->tileX(neWall) + mainAtlas->tileW(neWall), mainAtlas->tileY(neWall));
    vertex2i(northCorner.x, northCorner.y - pxHeight);
    texCoord2f(mainAtlas->tileX(neWall) + mainAtlas->tileW(neWall), mainAtlas->tileY(neWall) + mainAtlas->tileH(neWall));
    vertex2i(northCorner.x, northCorner.y);
    //Draw roof - upright image points to upper-right on the screen
    color3f(1, 1, 1);
    texCoord2f(mainAtlas->tileX(roof), mainAtlas->tileY(roof));
    vertex2i(southCorner.x + (northCorner.x - eastCorner.x), southCorner.y - (eastCorner.y - northCorner.y) - pxHeight);
    texCoord2f(mainAtlas->tileX(roof) + mainAtlas->tileW(roof), mainAtlas->tileY(roof));
    vertex2i(northCorner.x, northCorner.y - pxHeight);
    texCoord2f(mainAtlas->tileX(roof) + mainAtlas->tileW(roof), mainAtlas->tileY(roof) + mainAtlas->tileH(roof));
    vertex2i(eastCorner.x, eastCorner.y - pxHeight);
    texCoord2f(mainAtlas->tileX(roof), mainAtlas->tileY(roof) + mainAtlas->tileH(roof));
    vertex2i(southCorner.x, southCorner.y - pxHeight);
}

void RenderRoutines::drawCuboidEast(Cuboid &c)
{
    int nwWall = c.getNWWall(); //Texture ID for southwest wall
    int swWall = c.getSWWall(); //Tex id for SE
    int roof = c.getRoof();
    int pxHeight = c.getHeight() * coord::ISO_HEIGHT;   //screen "height" of box
    Point northCorner = coord::project3DPoint(c.getX(), c.getY() + c.getWidth(), c.getZ());
    Point westCorner = coord::project3DPoint(c.getX(), c.getY(), c.getZ());
    Point southCorner = coord::project3DPoint(c.getX() + c.getLength(), c.getY(), c.getZ());
    //Draw left (NW) wall, brightly lit
    color3f(1, 1, 1);
    texCoord2f(mainAtlas->tileX(nwWall), mainAtlas->tileY(nwWall) + mainAtlas->tileH(nwWall));
    vertex2i(northCorner.x, northCorner.y);
    texCoord2f(mainAtlas->tileX(nwWall), mainAtlas->tileY(nwWall));
    vertex2i(northCorner.x, northCorner.y - pxHeight);
    texCoord2f(mainAtlas->tileX(nwWall) + mainAtlas->tileW(nwWall), mainAtlas->tileY(nwWall));
    vertex2i(westCorner.x, westCorner.y - pxHeight);
    texCoord2f(mainAtlas->tileX(nwWall) + mainAtlas->tileW(nwWall), mainAtlas->tileY(nwWall) + mainAtlas->tileH(nwWall));
    vertex2i(westCorner.x, westCorner.y);
    //SW wall also brightly shaded
    texCoord2f(mainAtlas->tileX(swWall), mainAtlas->tileY(swWall) + mainAtlas->tileH(swWall));
    vertex2i(westCorner.x, westCorner.y);
    texCoord2f(mainAtlas->tileX(swWall), mainAtlas->tileY(swWall));
    vertex2i(westCorner.x, westCorner.y - pxHeight);
    texCoord2f(mainAtlas->tileX(swWall) + mainAtlas->tileW(swWall), mainAtlas->tileY(swWall));
    vertex2i(southCorner.x, southCorner.y - pxHeight);
    texCoord2f(mainAtlas->tileX(swWall) + mainAtlas->tileW(swWall), mainAtlas->tileY(swWall) + mainAtlas->tileH(swWall));
    vertex2i(southCorner.x, southCorner.y);
    //Draw roof
    texCoord2f(mainAtlas->tileX(roof) + mainAtlas->tileW(roof), mainAtlas->tileY(roof));
    vertex2i(northCorner.x, northCorner.y - pxHeight);
    texCoord2f(mainAtlas->tileX(roof) + mainAtlas->tileW(roof), mainAtlas->tileY(roof) + mainAtlas->tileH(roof));
    vertex2i(northCorner.x + (southCorner.x - westCorner.x), northCorner.y + (southCorner.y - westCorner.y) - pxHeight);
    texCoord2f(mainAtlas->tileX(roof), mainAtlas->tileY(roof) + mainAtlas->tileH(roof));
    vertex2i(southCorner.x, southCorner.y - pxHeight);
    texCoord2f(mainAtlas->tileX(roof), mainAtlas->tileY(roof));
    vertex2i(westCorner.x, westCorner.y - pxHeight);
}

void RenderRoutines::drawCuboidSouth(Cuboid &c)
{
    int nwWall = c.getSEWall(); //Texture ID for southwest wall
    int neWall = c.getSWWall(); //Tex id for SE
    int roof = c.getRoof();
    int pxHeight = c.getHeight() * coord::ISO_HEIGHT;   //screen "height" of box
    Point eastCorner = coord::project3DPoint(c.getX() + c.getLength(), c.getY() + c.getWidth(), c.getZ());
    Point northCorner = coord::project3DPoint(c.getX(), c.getY() + c.getWidth(), c.getZ());
    Point westCorner = coord::project3DPoint(c.getX(), c.getY(), c.getZ());
    //Draw NE wall
    color3f(SHADE, SHADE, SHADE);
    texCoord2f(mainAtlas->tileX(neWall), mainAtlas->tileY(neWall) + mainAtlas->tileH(neWall));
    vertex2i(eastCorner.x, eastCorner.y);
    texCoord2f(mainAtlas->tileX(neWall), mainAtlas->tileY(neWall));
    vertex2i(eastCorner.x, eastCorner.y - pxHeight);
    texCoord2f(mainAtlas->tileX(neWall) + mainAtlas->tileW(neWall), mainAtlas->tileY(neWall));
    vertex2i(northCorner.x, northCorner.y - pxHeight);
    texCoord2f(mainAtlas->tileX(neWall) + mainAtlas->tileW(neWall), mainAtlas->tileY(neWall) + mainAtlas->tileH(neWall));
    vertex2i(northCorner.x, northCorner.y);
    color3f(1, 1, 1);
    //Draw NW wall
    texCoord2f(mainAtlas->tileX(nwWall), mainAtlas->tileY(nwWall) + mainAtlas->tileH(nwWall));
    vertex2i(northCorner.x, northCorner.y);
    texCoord2f(mainAtlas->tileX(nwWall), mainAtlas->tileY(nwWall));
    vertex2i(northCorner.x, northCorner.y - pxHeight);
    texCoord2f(mainAtlas->tileX(nwWall) + mainAtlas->tileW(nwWall), mainAtlas->tileY(nwWall));
    vertex2i(westCorner.x, westCorner.y - pxHeight);
    texCoord2f(mainAtlas->tileX(nwWall) + mainAtlas->tileW(nwWall), mainAtlas->tileY(nwWall) + mainAtlas->tileH(nwWall));
    vertex2i(westCorner.x, westCorner.y);
    //Draw roof
    color3f(1, 1, 1);
    texCoord2f(mainAtlas->tileX(roof) + mainAtlas->tileW(roof), mainAtlas->tileY(roof));
    vertex2i(northCorner.x, northCorner.y - pxHeight);
    texCoord2f(mainAtlas->tileX(roof), mainAtlas->tileY(roof));
    vertex2i(westCorner.x, westCorner.y - pxHeight);
    texCoord2f(mainAtlas->tileX(roof), mainAtlas->tileY(roof) + mainAtlas->tileH(roof));
    vertex2i(eastCorner.x + (westCorner.x - northCorner.x), eastCorner.y + (westCorner.y - northCorner.y) - pxHeight);
    texCoord2f(mainAtlas->tileX(roof) + mainAtlas->tileW(roof), mainAtlas->tileY(roof) + mainAtlas->tileH(roof));
    vertex2i(eastCorner.x, eastCorner.y - pxHeight);
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
    return 1.0 - 0.3 * (abs(h1 - h2) + abs(h2 - h3) + abs(h3 - h4) + abs(h4 - h1)) / coord::ISO_HEIGHT;
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