//
//  RenderRoutines.h
//  Magnate
//
//  Created by Brian Kelley on 3/5/1564.
//
//

#ifndef Magnate_RenderRoutines_h
#define Magnate_RenderRoutines_h

#include <iostream>
#include <string>
#include "Constants.h"
#include "Coord.h"
#include "Building.h"
#include "Atlas.h"
#include "SdlHeaders.h"

namespace RenderRoutines
{
    void drawString(std::string text, int x, int y);
    void drawString(std::string text, int x, int y, float scale);
    void drawString(std::string text, int x, int y, float scale, float r, float g, float b);
    void blit(int tex, int x, int y);
    void blit(int tex, int x1, int y1, int x2, int y2);
    void drawCuboid(Cuboid& c);
    int texNumFromStr(std::string name);
    floatRect_t getTexCoords(int index);

}

#endif
