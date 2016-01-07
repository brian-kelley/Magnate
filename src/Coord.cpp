//
//  Coord.cpp
//  Magnate
//
//  Created by Brian Kelley on 10/15/14288.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "Coord.h"

using namespace constants;
using namespace glm;
using namespace std;

mat4 Coord::tileToWorldMat;
mat4 Coord::worldToTileMat;

void Coord::initCoord()
{
    tileToWorldMat[0] = tileI;
    tileToWorldMat[1] = tileJ;
    tileToWorldMat[2] = tileK;
    worldToTileMat = inverse(tileToWorldMat);
}

bool Coord::rectInside(intRect_t* small, intRect_t* big)
{
    if(big->x <= small->x && big->x + big->w > small->x + small->w
       && big->y <= small->y && big->y + big->h > small->y + small->h)
    {
        return true;
    }
    return false;
}

vec3 Coord::tileToWorld(int x, unsigned short height, int z)
{
    vec4 tileVec = {x, height, z, 1};
    return vec3(tileToWorldMat * tileVec);
}

Pos2 Coord::worldToTile(vec4 worldPos)
{
    vec4 tileVec = worldToTileMat * worldPos;
    return {short(tileVec.x), short(tileVec.z)};
}

Pos2 Coord::getTileInDir(Pos2 loc, int dir)
{
    switch(dir)
    {
        case UP:
            loc.y--;
            return loc;
        case DOWN:
            loc.y++;
            return loc;
        case LEFT:
            loc.x--;
            return loc;
        case RIGHT:
            loc.x++;
            return loc;
        default:
            return {-1, -1};
    }
}

bool operator==(const Pos2& p1, const Pos2& p2)
{
    if(p1.x == p2.x && p1.y == p2.y)
        return true;
    return false;
}

bool operator!=(const Pos2& p1, const Pos2& p2)
{
    return !(p1 == p2);
}

ostream& operator<<(ostream& os, const Pos2& loc)
{
    os << "(" << loc.x << ", " << loc.y << ")";
    return os;
}
