//
//  RenderRoutines.h
//  Magnate
//
//  Created by Brian Kelley on 3/5/1564.
//
//

#ifndef Magnate_RenderRoutines_h
#define Magnate_RenderRoutines_h

#include <gl.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include "Constants.h"
#include "Coord.h"
#include "Atlas.h"
#include "Cuboid.h"

namespace RenderRoutines
{
    extern Atlas* mainAtlas;
    extern void initAtlas(SDL_Renderer* windowRenderer);
    extern void drawString(std::string text, int x, int y);
    extern void drawString(std::string text, int x, int y, float scale);
    extern void drawString(std::string text, int x, int y, float scale, float r, float g, float b);
    extern void isoBlit(int tex, int i, int j, int h1, int h2, int h3, int h4); //Used for drawing terrain mesh in 3D space isometrically
    extern void blit(int tex, int x, int y);
    extern void blit(int tex, int x1, int y1, int x2, int y2);
    extern void drawCuboid(Cuboid& c);
    extern int texNumFromStr(std::string name);
}

#endif
