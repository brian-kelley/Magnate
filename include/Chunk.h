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

typedef short Height;

class Chunk
{
public:
    //Chunks are always aligned with the boundary i and j values CHUNK_SIZE * TERRAIN_TILE_SIZE
    //so constructor should take integers even though tiles exist in double, double coordinates
    Chunk();
    ~Chunk();
    //array of nodes in heightmap within this chunk's area
    GROUND terrain[constants::CHUNK_SIZE][constants::CHUNK_SIZE];
    Height mesh[constants::CHUNK_SIZE][constants::CHUNK_SIZE];
};

#endif /* defined(__MagIndev__Chunk__) */
