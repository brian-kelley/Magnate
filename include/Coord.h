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

typedef struct
{
    short x;
    short y;
} Pos2;

typedef glm::vec3 Pos3;

typedef struct
{
    float a;
    float b;
    float c;
    float d;
} Plane;

typedef struct
{
    glm::vec4 upperLeft;
    glm::vec4 upperRight;
    glm::vec4 lowerRight;
    glm::vec4 lowerLeft;
} FrustumCorners;

namespace Coord
{
    void initCoord();
    bool rectInside(intRect_t* small, intRect_t* big);
    glm::vec3 tileToWorld(int x, unsigned short height, int z);
    Pos2 worldToTile(glm::vec4 worldPos);
    FrustumCorners getFrustumCorners();
    extern glm::mat4 view3;
    extern glm::mat4 proj3;
    extern glm::vec3 camPos;
    extern glm::vec3 camUp;
    extern glm::vec3 camDir;
    extern float camAngle;
    const float camPitch = M_PI_2 * 0.7; //angle of depression from straight ahead (pi/2 is straight down)
    //Component vectors for constructing world positions from tile indices and heightmap heights
    const glm::vec4 tileI = {constants::TERRAIN_TILE_SIZE, 0, 0, 0};
    const glm::vec4 tileJ = {0, constants::TERRAIN_Y_SCALE, 0, 0};
    const glm::vec4 tileK = {0, 0, constants::TERRAIN_TILE_SIZE, 0};
    extern glm::mat4 tileToWorldMat;
    extern glm::mat4 worldToTileMat;
};
#endif
