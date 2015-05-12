//
//  Chunk.cpp
//  MagIndev
//
//  Created by Brian Kelley on 10/22/14295.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "Chunk.h"

using namespace std;
using namespace constants;

Chunk::Chunk()
{
    i = 0;
    j = 0;
}

//Constructor is where terrain mesh generation happens i guess (diamond-square)
Chunk::Chunk(int i, int j)
{
    this->i = i;
    this->j = j;
}

Chunk::~Chunk() {}

//add these offsets to tile coords within the chunk to get absolute coordinates in world
double Chunk::getIOffset()
{
    return TERRAIN_TILE_SIZE * CHUNK_SIZE * i;
}

double Chunk::getJOffset()
{
    return TERRAIN_TILE_SIZE * CHUNK_SIZE * j;
}

int Chunk::getI()
{
    return i;
}

int Chunk::getJ()
{
    return j;
}