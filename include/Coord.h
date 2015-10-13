//
//  Coord.h
//  Magnate
//
//  Created by Brian Kelley on 10/15/14288.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#ifndef __Magnate__Coord__
#define __Magnate__Coord__

#include <iostream>
#include "Constants.h"
#include "GlmHeaders.h"
#include "LinAlg.h"
#include "Camera.h"

struct Pos2
{
    Pos2() : x(0), y(0) {}
    Pos2(short x, short y) : x(x), y(y) {}
    short x;
    short y;
};

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

struct FrustumCorners
{
    FrustumCorners() {}
    FrustumCorners(glm::vec4 upperLeft, glm::vec4 upperRight, glm::vec4 lowerRight, glm::vec4 lowerLeft) : upperLeft(upperLeft), upperRight(upperRight), lowerRight(lowerRight), lowerLeft(lowerLeft) {}
    glm::vec4 upperLeft;
    glm::vec4 upperRight;
    glm::vec4 lowerRight;
    glm::vec4 lowerLeft;
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
    FrustumCorners getFrustumCorners();
    extern glm::mat4 view3;
    extern glm::mat4 proj3;
    //Component vectors for constructing world positions from tile indices and heightmap heights
    const glm::vec4 tileI = {constants::TERRAIN_TILE_SIZE, 0, 0, 0};
    const glm::vec4 tileJ = {0, constants::TERRAIN_Y_SCALE, 0, 0};
    const glm::vec4 tileK = {0, 0, constants::TERRAIN_TILE_SIZE, 0};
    Pos2 getTileInDir(Pos2 loc, int dir);
    extern glm::mat4 tileToWorldMat;
    extern glm::mat4 worldToTileMat;
};
#endif