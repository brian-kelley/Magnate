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
using namespace Camera;

mat4 Coord::view3;
mat4 Coord::proj3;
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

FrustumCorners Coord::getFrustumCorners()
{
    FrustumCorners corners;
    mat4 projInv = glm::inverse(proj3);
    mat4 viewInv = glm::inverse(view3);
    float back = 1;
    float front = -1;
    float left = -1;
    float right = 1;
    float up = 1;
    float down = -1;
    vec4 lub = {left, up, back, 1};
    vec4 rub = {right, up, back, 1};
    vec4 rdb = {right, down, back, 1};
    vec4 ldb = {left, down, back, 1};
    vec4 luf = {left, up, front, 1};
    vec4 ruf = {right, up, front, 1};
    vec4 rdf = {right, down, front, 1};
    vec4 ldf = {left, down, front, 1};
    lub = projInv * lub;
    rub = projInv * rub;
    rdb = projInv * rdb;
    ldb = projInv * ldb;
    luf = projInv * luf;
    ruf = projInv * ruf;
    rdf = projInv * rdf;
    ldf = projInv * ldf;
    lub /= lub.w;
    rub /= rub.w;
    rdb /= rdb.w;
    ldb /= ldb.w;
    luf /= luf.w;
    ruf /= ruf.w;
    rdf /= rdf.w;
    ldf /= ldf.w;
    lub = viewInv * lub;
    rub = viewInv * rub;
    rdb = viewInv * rdb;
    ldb = viewInv * ldb;
    luf = viewInv * luf;
    ruf = viewInv * ruf;
    rdf = viewInv * rdf;
    ldf = viewInv * ldf;
    //frustum corner points are now world space
    //draw line between front and back point pairs and find where y = 0
    vec4 luVec = normalize(luf - lub);
    vec4 ruVec = normalize(ruf - rub);
    vec4 rdVec = normalize(rdf - rdb);
    vec4 ldVec = normalize(ldf - ldb);
    float luMult = -lub.y / luVec.y;
    float ruMult = -rub.y / ruVec.y;
    float rdMult = -rdb.y / rdVec.y;
    float ldMult = -ldb.y / ldVec.y;
    corners.upperLeft = {camPos.x + luMult * luVec.x, 0, camPos.z + luMult * luVec.z, 1};
    corners.upperRight = {camPos.x + ruMult * ruVec.x, 0, camPos.z + ruMult * ruVec.z, 1};
    corners.lowerRight = {camPos.x + rdMult * rdVec.x, 0, camPos.z + rdMult * rdVec.z, 1};
    corners.lowerLeft = {camPos.x + ldMult * ldVec.x, 0, camPos.z + ldMult * ldVec.z, 1};
    return corners;
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

SmallVec::SmallVec() {}

SmallVec::SmallVec(char x, char y)
{
    this->x = x;
    this->y = y;
}

SmallVec::SmallVec(int direction, int mag)
{
    mag = std::max(mag, CHAR_MIN);
    mag = std::min(mag, CHAR_MAX);
    x = 0;
    y = 0;
    switch(direction)
    {
        case UP:
            y = -mag;
            break;
        case LEFT:
            x = -mag;
            break;
        case DOWN:
            y = mag;
            break;
        case RIGHT:
            x = mag;
    }
}

SmallVec::SmallVec(int d1, int m1, int d2, int m2)
{
    //Handle potential overflow from summing m1 and m2 in the same component
    if(d1 == d2)
    {
        m1 += m2;
        m2 = 0;
    }
    int xt = 0;
    int yt = 0;
    switch(d1)
    {
        case UP:
            yt -= m1;
            break;
        case LEFT:
            xt -= m1;
            break;
        case DOWN:
            yt += m1;
            break;
        case RIGHT:
            xt += m1;
    }
    switch(d2)
    {
        case UP:
            yt -= m2;
            break;
        case LEFT:
            xt -= m2;
            break;
        case DOWN:
            yt += m2;
            break;
        case RIGHT:
            xt += m2;
    }
    xt = std::max(CHAR_MIN, xt);
    xt = std::min(CHAR_MAX, xt);
    yt = std::max(CHAR_MIN, yt);
    yt = std::min(CHAR_MAX, yt);
    x = xt;
    y = yt;
}