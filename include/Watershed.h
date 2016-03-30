#ifndef __WATERSHED_H__
#define __WATERSHED_H__

#include <iostream>
#include <vector>
#include <queue>
#include "GlobalConfig.h"
#include "Coord.h"
#include "Heightmap.h"
#include "Terrain.h"

struct Watershed
{
    Watershed(Heightmap& worldHeights, Heightmap& worldBiomes, int numRivers, short minH);
    Heightmap& world;
    Heightmap& biomes;
    void addRiver(short threshold);
    Pos2 formLake(Pos2 loc);
    //Utility functions for lake/river generation
    int getNonLakeDownhillDir(Pos2 loc);
    int getNonLakeEqualDir(Pos2 loc, short flood);
    bool surroundedByLake(Pos2 loc);
    short getLowestAdjacent(Pos2 loc); //Height of lowest neighbor
    bool hasLakeNeighbor(Pos2 loc);
    bool hasOutlet(Pos2 loc);
    bool hasDownhillFlow(Pos2 loc);
};

#endif
