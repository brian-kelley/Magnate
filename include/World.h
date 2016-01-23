#ifndef __WORLD_H__
#define __WORLD_H__

#include <vector>
#include <iostream>
#include <random>
#include <cstdio>
#include "Terrain.h"
#include "TerrainGen.h"
#include "FileIO.h"
#include "Topo.h"
#include "RandomUtils.h"
#include "Heightmap.h"

class World
{
public:
    static void initDebug();    //quickly jump into game with fresh world
    static void init(std::string saveName, bool generate);
    static void read();
    static void write();
    static void update();       //do all game logic/simulation
    static const Heightmap& getHeights();
    static const Heightmap& getBiomes();
    static bool isDrawing();
private:
    static bool drawing;
    static Heightmap height;
    static Heightmap biomes;
    static std::string saveName;
    static unsigned seed;
};

#endif
