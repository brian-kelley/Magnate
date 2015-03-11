//
//  World.h
//  MagIndev
//
//  Created by Brian Kelley on 10/21/14294.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#ifndef __MagIndev__World__
#define __MagIndev__World__

#include <boost/filesystem.hpp>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <fstream>
#include "Chunk.h"
#include "Constants.h"
#include "Terrain.h"

class World
{
public:
    //saveName = name of folder in saves,
    World(std::string saveName, bool generate);
    ~World();
    static std::string currentSaveName;
    bool isValidTile(int x, int y);
    GROUND getTerrainType(int x, int y);
    int getTerrainHeight(int x, int y);
private:
    //data
    uint64_t seed;
    //methods
    void generate();
    void readWorld();
    void writeWorld();
};

#endif
