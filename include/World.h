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

//2 bytes per node seems pretty good for the terrain i guess
typedef struct
{
    //0 (sea level?) to 255 (mountain peaks?)
    unsigned char height;
    //ground type of tile with node as upper-left corner
    GROUND g;
} terrainMeshNode_t;

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
    terrainMeshNode_t terrain[WORLD_SIZE][WORLD_SIZE];
    //methods
    void generate();
    void populateTerrain();
    void readWorld();
    void writeWorld();
};

#endif
