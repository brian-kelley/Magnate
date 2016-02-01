#ifndef __WATERSHED_H__
#define __WATERSHED_H__

#include <iostream>
#include <vector>
#include <queue>
#include "GlobalConfig.h"
#include "World.h"
#include "Coord.h"
#include "Heightmap.h"

typedef short Height;

struct Gradient
{
    Gradient() : dx(0), dy(0) {}
    Gradient(short dx, short dy)
    {
        this->dx = dx;
        this->dy = dy;
    }
    short dx;
    short dy;
};

struct Watershed
{
    Watershed(int numRivers, short minH);
    Heightmap& world;
    Heightmap& biomes;
    void addRiver(Height threshold);
    void beginFlow(Pos2 pos);
    void generalFlow(Pos2 pos);
    void processTile(Pos2 pos, std::queue<Pos2>& q);
    Pos2 getLowestNeighbor(Pos2 pos);
    void riverRandomWalk(Pos2 p1, Pos2 p2);
    void rainForTime(int sec); //repeatedly drop water for the specified time
    void dropWater(Pos2 loc);
    void doErosion();
    Pos2 getLandTile();
    Gradient getTileGradient(Pos2 loc); //loc is upper-left
    void processStuckFlow(Pos2 loc, Gradient* gradBuf);
    Pos2 flowDownhill(Pos2 loc);
    Pos2 formLake(Pos2 loc);
    //Utility functions for lake/river generation
    int getNonLakeDownhillDir(Pos2 loc);
    int getNonLakeEqualDir(Pos2 loc, Height flood);
    bool surroundedByLake(Pos2 loc);
    Height getLowestAdjacent(Pos2 loc); //Height of lowest neighbor
    bool hasLakeNeighbor(Pos2 loc);
    bool hasOutlet(Pos2 loc);
    bool hasDownhillFlow(Pos2 loc);
};

#endif