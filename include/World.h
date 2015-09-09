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
#include <random>
#include <fstream>
#include "Chunk.h"
#include "Constants.h"
#include "Terrain.h"
#include "TerrainGen.h"
#include "Minimap.h"
#include "RandomUtils.h"

namespace World
{
    //saveName = name of folder in saves,
    void init(std::string saveName, bool generate);
    void dispose(); //free memory
    void generate();
    int getWorldLength();
    //slow, worldwide tile access (use for world generation)
    void setHeight(Height height, int wi, int wj);
    void setHeight(Height height, Pos2 loc);
    void setGround(GROUND ground, int wi, int wj);
    void setGround(GROUND ground, Pos2 loc);
    Height getHeight(int wi, int wj);
    Height getHeight(Pos2 loc);
    GROUND getGround(int wi, int wj);
    GROUND getGround(Pos2 loc);
    bool tileInWorld(int x, int y);
    //Length of world in either direction, in chunks
    void readWorld();
    void writeWorld();
    extern Chunk* chunks[constants::WORLD_CHUNKS][constants::WORLD_CHUNKS];
    extern std::string currentSaveName;
    extern unsigned long long int seed;
}

#endif
