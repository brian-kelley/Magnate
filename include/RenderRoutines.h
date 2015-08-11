//
//  RenderRoutines.h
//  Magnate
//
//  Created by Brian Kelley on 3/5/1564.
//
//

#ifndef Magnate_RenderRoutines_h
#define Magnate_RenderRoutines_h

#include "gl.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include "Constants.h"
#include "Coord.h"
#include "Building.h"
#include "Atlas.h"
#include "Renderer.h"

namespace RenderRoutines
{
    void initAtlas(SDL_Renderer* windowRenderer);
    void drawString(std::string text, int x, int y);
    void drawString(std::string text, int x, int y, float scale);
    void drawString(std::string text, int x, int y, float scale, float r, float g, float b);
    void isoBlit(int tex, double i, double j, unsigned char h1, unsigned char h2, unsigned char h3, unsigned char h4); //Used for drawing terrain mesh in 3D space isometrically
    void blit(int tex, int x, int y);
    void blit(int tex, int x1, int y1, int x2, int y2);
    void drawCuboid(Cuboid& c);
    int texNumFromStr(std::string name);
    floatRect_t getTexCoords(int index);
    void drawLine(int x1, int y1, int x2, int y2);
}

#endif
