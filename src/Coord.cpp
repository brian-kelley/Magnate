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
    return ISO_LENGTH / 2 * (i + j);
}

int coord::jy(double i, double j)
{
    return ISO_WIDTH / 2 * (i - j);
}

double coord::xi(int x, int y)
{
    return x / ISO_LENGTH + y / ISO_WIDTH;
}

double coord::yj(int x, int y)
{
    return x / ISO_LENGTH - y / ISO_WIDTH;
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

Point coord::project3DPoint(double i, double j, double h)
{
    Point ret;
    ret.x = ix(i, j) - constants::screenX;
    ret.y = jy(i, j) - constants::screenY;
    ret.y -= h * ISO_HEIGHT;
    //printf("(%.2f, %.2f, %.2f) mapped to (%d, %d)\n\n", i, j, h, ret.x, ret.y);
    return ret;
}
/*
 length / 2 * (i + j) = x
 width / 2 * (i - j) = y
 i + j = 2x / length
 i - j = 2y / width
 2i = 2x / length + 2y / width
 i = x / length + y / width
 j = 2x / length - i
 j = 2x / length - x / length - y / width
 j = x / length - y / width
*/