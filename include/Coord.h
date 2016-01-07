//
//  Coord.h
//  Magnate
//
//  Created by Brian Kelley on 10/15/14288.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#ifndef __COORD_H__
#define __COORD_H__

#include <iostream>
#include <algorithm>
#include "Constants.h"
#include "GlmHeaders.h"
#include "LinAlg.h"
#include "GenTypes.h"



bool operator==(const Pos2& p1, const Pos2& p2);
bool operator!=(const Pos2& p1, const Pos2& p2);
std::ostream& operator<<(std::ostream& os, const Pos2& loc);

typedef glm::vec3 Pos3;

struct Plane
{
    Plane() : a(1), b(0), c(0), d(0) {}
    Plane(float a, float b, float c, float d) : a(a), b(b), c(c), d(d) {}
    float a;
    float b;
    float c;
    float d;
};

enum Direction
{
    UP,    //-y
    DOWN,  //+y
    LEFT,  //-x
    RIGHT, //+x
    NO_DIRECTION
};

namespace Coord
{
    void initCoord();
    bool rectInside(intRect_t* small, intRect_t* big);
    glm::vec3 tileToWorld(int x, unsigned short height, int z);
    Pos2 worldToTile(glm::vec4 worldPos);
    extern glm::mat4 view3;
    extern glm::mat4 proj3;
    //Component vectors for constructing worldspace coords from tile indices and heightmap heights
    const glm::vec4 tileI = {constants::TERRAIN_TILE_SIZE, 0, 0, 0};
    const glm::vec4 tileJ = {0, constants::TERRAIN_Y_SCALE, 0, 0};
    const glm::vec4 tileK = {0, 0, constants::TERRAIN_TILE_SIZE, 0};
    Pos2 getTileInDir(Pos2 loc, int dir);
    extern glm::mat4 tileToWorldMat;
    extern glm::mat4 worldToTileMat;
};
#endif