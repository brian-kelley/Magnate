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
#include <cstdio>
#include "Constants.h"
#include "Terrain.h"
#include "TerrainGen.h"
#include "Minimap.h"
#include "Topo.h"
#include "RandomUtils.h"
#include "Heightmap.h"

class World
{
public:
    World();
    World(std::string saveName, bool generate);
    void read();
    void write();
    static World* instance;
private:
    Heightmap height;
    Heightmap ground;
    std::string saveName;
    unsigned int seed;
};

#endif
