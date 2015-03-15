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
#include "Terrain.h"

class Chunk
{
public:
    static const int CHUNK_SIZE = 128;
    //Chunks are always aligned with the boundary i and j values CHUNK_SIZE * TERRAIN_TILE_SIZE
    //so constructor should take integers even though tiles exist in double, double coordinates
    Chunk(int i, int j);
    ~Chunk();
    tNode_t mesh[CHUNK_SIZE][CHUNK_SIZE];
    double getIOffset();
    double getJOffset();
    int getI();
    int getJ();
private:
    int i;
    int j;
};

#endif /* defined(__MagIndev__Chunk__) */
