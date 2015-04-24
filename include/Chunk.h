//
//  Chunk.h
//  MagIndev
//
//  Created by Brian Kelley on 10/22/14295.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#ifndef __MagIndev__Chunk__
#define __MagIndev__Chunk__
#define CHUNK_SIZE 129

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <boost/multi_array.hpp>
#include "Terrain.h"
#include "TerrainGen.h"

class Chunk
{
public:
    //Chunks are always aligned with the boundary i and j values CHUNK_SIZE * TERRAIN_TILE_SIZE
    //so constructor should take integers even though tiles exist in double, double coordinates
    Chunk(int i, int j);
    ~Chunk();
    double getIOffset();
    double getJOffset();
    int getI();
    int getJ();
    tNode_t mesh[CHUNK_SIZE][CHUNK_SIZE];
private:
    int i;
    int j;
};

#endif /* defined(__MagIndev__Chunk__) */
