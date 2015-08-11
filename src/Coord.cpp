//
//  Coord.cpp
//  Magnate
//
//  Created by Brian Kelley on 10/15/14288.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "Coord.h"

using namespace constants;

bool Coord::rectInside(intRect_t* small, intRect_t* big)
{
    if(big->x <= small->x && big->x + big->w > small->x + small->w
       && big->y <= small->y && big->y + big->h > small->y + small->h)
    {
        return true;
    }
    return false;
}

Pos3 Coord::tileToWorld(int x, unsigned short height, int z)
{
    Pos3 pos;
    pos.x = x * TERRAIN_TILE_SIZE;
    pos.y = height * TERRAIN_Y_SCALE * TERRAIN_TILE_SIZE;
    pos.z = z * TERRAIN_TILE_SIZE;
    return pos;
}