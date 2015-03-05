//
//  Chunk.cpp
//  MagIndev
//
//  Created by Brian Kelley on 10/22/14295.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "Chunk.h"

using namespace std;

Chunk::Chunk(int x, int y)
{
    this->x = x;
    this->y = y;
}

Chunk::~Chunk()
{
    
}

void Chunk::setTerrain(int x, int y, GROUND gtype)
{
    if(x < CHUNK_SIZE && y < CHUNK_SIZE)
    {
        ground[y][x] = gtype;
    }
    else
    {
        cout << "Chunk coords out of bounds." << endl;
    }
}