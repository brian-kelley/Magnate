//
//  Coord.cpp
//  Magnate
//
//  Created by Brian Kelley on 10/15/14288.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "Coord.h"

using namespace constants;

double coord::ISO_LENGTH = DEFAULT_ISO_LENGTH;
double coord::ISO_WIDTH = DEFAULT_ISO_WIDTH;
double coord::ISO_HEIGHT = DEFAULT_ISO_HEIGHT;

int coord::ix(double i, double j)
{
    switch(viewDirection)
    {
        case NORTH:
            return ISO_LENGTH / 2.0 * (i + j);
        case WEST:
            return ISO_LENGTH / 2.0 * (-i + j);
        case EAST:
            return ISO_LENGTH / 2.0 * (i - j);
        case SOUTH:
            return ISO_LENGTH / 2.0 * (-i - j);
    }
}

int coord::jy(double i, double j)
{
    switch(viewDirection)
    {
        case NORTH:
            return ISO_WIDTH / 2.0 * (i - j);
        case WEST:
            return ISO_WIDTH / 2.0 * (i + j);
        case EAST:
            return ISO_WIDTH / 2.0 * (-i - j);
        case SOUTH:
            return ISO_WIDTH / 2.0 * (-i + j);
    }
}

double coord::xi(int x, int y)
{
    switch(viewDirection)
    {
        case NORTH:
            return x / double(ISO_LENGTH) + y / double(ISO_WIDTH);
        case WEST:
            return -x / double(ISO_LENGTH) + y / double(ISO_WIDTH);
        case EAST:
            return x / double(ISO_LENGTH) - y / double(ISO_WIDTH);
        case SOUTH:
            return -x / double(ISO_LENGTH) - y / double(ISO_WIDTH);
    }
}

double coord::yj(int x, int y)
{
    switch(viewDirection)
    {
        case NORTH:
            return x / double(ISO_LENGTH) - y / double(ISO_WIDTH);
        case WEST:
            return x / double(ISO_LENGTH) + y / double(ISO_WIDTH);
        case EAST:
            return -x / double(ISO_LENGTH) - y / double(ISO_WIDTH);
        case SOUTH:
            return -x / double(ISO_LENGTH) + y / double(ISO_WIDTH);
    }
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
    return ret;
}

void coord::updateScale()
{
    ISO_LENGTH = DEFAULT_ISO_LENGTH * worldScale;
    ISO_WIDTH = DEFAULT_ISO_WIDTH * worldScale;
    ISO_HEIGHT = DEFAULT_ISO_HEIGHT * worldScale;
}