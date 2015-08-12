//
//  Coord.h
//  Magnate
//
//  Created by Brian Kelley on 10/15/14288.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#ifndef __Magnate__Coord__
#define __Magnate__Coord__

#include "Constants.h"
#include <SDL2/SDL.h>

typedef struct
{
    short x;
    short y;
} Pos2;

typedef struct
{
    float x;
    float y;
    float z;
} Pos3;

namespace Coord
{
    bool rectInside(intRect_t* small, intRect_t* big);
    Pos3 tileToWorld(int x, unsigned short height, int z);
};
#endif
