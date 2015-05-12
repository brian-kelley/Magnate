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
#include <vector>
#include <iostream>
#include <fstream>
#include "Chunk.h"
#include "Constants.h"
#include "Terrain.h"
#include "TerrainGen.h"

class World
{
public:
    //saveName = name of folder in saves,
    World(std::string saveName, bool generate);
    ~World();
    static std::string currentSaveName;
    void generate();
    int getWorldLength();
    Chunk* chunks[constants::WORLD_CHUNKS][constants::WORLD_CHUNKS];
    //slow, worldwide tile access (use for world generation)
    void setHeight(Height height, int wi, int wj);
    void setGround(GROUND ground, int wi, int wj);
    Height getHeight(int wi, int wj);
    GROUND getGround(int wi, int wj);
private:
    //Length of world in either direction, in chunks
    //data
    int seed;
    void readWorld();
    void writeWorld();
};

#endif