//
//  Chunk.h
//  MagIndev
//
//  Created by Brian Kelley on 10/22/14295.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#ifndef __MagIndev__Chunk__
#define __MagIndev__Chunk__

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <boost/multi_array.hpp>
#include "Terrain.h"
#include "Constants.h"

typedef unsigned char Height;

typedef struct
{
    GROUND g;
    Height height;
} tNode_t;

class Chunk
{
public:
    //Chunks are always aligned with the boundary i and j values CHUNK_SIZE * TERRAIN_TILE_SIZE
    //so constructor should take integers even though tiles exist in double, double coordinates
    Chunk();
    Chunk(int i, int j);
    ~Chunk();
    double getIOffset();
    double getJOffset();
    int getI();
    int getJ();
    void setI(int i);
    void setJ(int j);
    //array of nodes in heightmap within this chunk's area
    tNode_t mesh[constants::CHUNK_SIZE][constants::CHUNK_SIZE];
private:
    int i;
    int j;
};

#endif /* defined(__MagIndev__Chunk__) */
