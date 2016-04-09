#ifndef __WORLD_H__
#define __WORLD_H__

#include <vector>
#include <iostream>
#include <random>
#include <cstdio>
#include "Terrain.h"
#include "TerrainGen.h"
#include "FileIO.h"
#include "RandomUtils.h"
#include "Heightmap.h"
#include "Broadcaster.h"
#include "Building.h"
#include "Mesh.h"

#define DEBUG_SAVE_NAME ("debug_world")

namespace World
{
    void init(std::string saveName = DEBUG_SAVE_NAME);
    void initCached(std::string name);
    void saveAndExit();
    void read();
    void write();
    void update();       //do all game logic/simulation
    Heightmap& getHeights();
    Heightmap& getBiomes();
    void readTerrainCache(std::string name);
    void writeTerrainCache(std::string name);
    extern bool drawing;
    extern Heightmap height;
    extern Heightmap biomes;
    extern Mesh mesh;
    extern std::vector<Building> buildings;
    extern std::string saveName;
    extern unsigned seed;
    extern Broadcaster<bool> worldLoaded;
};

#endif
