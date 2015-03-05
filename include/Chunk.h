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
    static const int CHUNK_SIZE = 64;
    Chunk(int, int);
    ~Chunk();
    void generateTerrain(Chunk* left, Chunk* top, Chunk* right, Chunk* bottom);
    GROUND ground[CHUNK_SIZE][CHUNK_SIZE];
    void setTerrain(int x, int y, GROUND gtype);
private:
    int x;
    int y;
};

#endif /* defined(__MagIndev__Chunk__) */