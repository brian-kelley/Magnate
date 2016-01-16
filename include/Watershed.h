#ifndef __WATERSHED_H__
#define __WATERSHED_H__

#include <iostream>
#include <vector>
#include <queue>
#include "Coord.h"
#include "Heightmap.h"
#include "Terrain.h"
#include "GlobalConfig.h"

class Watershed
{
public:
    Watershed(Heightmap& worldMap, Heightmap& biomeMap, short threshold, int numRivers);
private:
    void beginFlow(Pos2 pos);
    void generalFlow(Pos2 pos);
    void processTile(Pos2 pos, std::queue<Pos2>& q);
    Pos2 getLowestNeighbor(Pos2 pos);
    void riverRandomWalk(Pos2 p1, Pos2 p2);
    Pos2 getLandTile();
    Pos2 flowDownhill(Pos2 loc);
    Pos2 formLake(Pos2 loc);
    int getNonLakeDownhillDir(Pos2 loc);
    int getNonLakeEqualDir(Pos2 loc, short flood);
    bool surroundedByLake(Pos2 loc);
    short getLowestAdjacent(Pos2 loc); //Height of lowest neighbor
    bool hasLakeNeighbor(Pos2 loc);
    bool hasOutlet(Pos2 loc);
    bool hasDownhillFlow(Pos2 loc);
    Heightmap& world;
    Heightmap& biomes;
};

#endif