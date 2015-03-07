//
//  Coord.cpp
//  Magnate
//
//  Created by Brian Kelley on 10/15/14288.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "Coord.h"

int coord::ix(double i, double j)
{
    return (int) (ISO_HEIGHT * i + ISO_HEIGHT * j);
}

int coord::jy(double i, double j)
{
    return (int) (ISO_HEIGHT / 2 * j - 32 * i);
}

double coord::xi(int x, int y)
{
    return x / double(ISO_WIDTH) - y / double(ISO_HEIGHT);
}

double coord::yj(int x, int y)
{
    return x / double(ISO_WIDTH) + y / double(ISO_HEIGHT);
}

bool coord::rectInside(intRect_t* small, intRect_t* big)
{
    if(big->x <= small->x && big->x + big->w > small->x + small->w
       && big->y <= small->y && big->y + big->h > small->y + small->h)
    {
        return true;
    }
    return false;
}

SDL_Point coord::project3DPoint(double i, double j, double h)
{
    SDL_Point ret;
    ret.x = ix(i, j);
    ret.y = jy(i, j);
    ret.y -= h * ISO_HEIGHT;
    return ret;
}