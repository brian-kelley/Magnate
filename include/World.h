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
#include "TriMesh.h"

class World
{
public:
    static void initDebug();    //quickly jump into game with fresh world
    static void init(std::string saveName);
    static void initCached(std::string name);
    static void saveAndExit();
    static void read();
    static void write();
    static void update();       //do all game logic/simulation
    static Heightmap& getHeights();
    static Heightmap& getBiomes();
    static bool isDrawing();
    static Broadcaster<bool> worldLoaded;
    static void readTerrainCache(std::string name);
    static void writeTerrainCache(std::string name);
private:
    static bool drawing;
    static Heightmap height;
    static Heightmap biomes;
    static std::vector<Building> buildings;
    static std::string saveName;
    static unsigned seed;
};

#endif
