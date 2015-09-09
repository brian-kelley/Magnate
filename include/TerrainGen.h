#ifndef __TERRAIN_GEN_INCLUDED__
#define __TERRAIN_GEN_INCLUDED__

#include <iostream>
#include <queue>
#define ROUGHNESS 10
#include "Constants.h"
#include "Chunk.h"
#include "World.h"

static enum Direction
{
    UP,    //-y
    LEFT,  //-x
    DOWN,  //+y
    RIGHT, //+x
    NO_DIRECTION
};

namespace TerrainGen
{
    void generate();
    void diamondSquare();
    void defuzz();       //remove non-water tiles that are surrounded by water on at least 3 sides (adjacent)
    void flattenWater(); //force water tiles to have constant height at all 4 corners
    void fillSquare(int x, int y, int size);
    void fillDiamond(int x, int y, int size);
    //Tests whether the mesh
    bool inMesh(int x, int y);
    Height getHeight(int avg, int size);
    void scatterVolcanos();
    void addVolcano(int x, int y, short height, int radius); //create a cone-shaped hill (or cone-shaped pit if height < 0)
    void erode(int numTimesteps); //do one timestep's worth of erosion
    void clearAll();  //fill world with water at sea level
    void pyramidMask(); //lower elevation corresponding to distance from world center, so that around the edges is only water
    void clampSeaLevel();
    void addRiver();
    void flowRiverFromPoint(Pos2 loc);
    Pos2 formLake(Pos2 lastRiver); //create a flat pool at the point, and return the location of where river continues
    bool hasNonLakeDownhill(Pos2 loc); //test whether there is a downhill route from loc, that doesn't go into a lake
    Height maxHeightOfTile(Pos2 loc);  //max of height values of four corners of tile
    void recurseLakeFill(Pos2 loc, Height threshold, std::vector<Pos2>& lakeTiles);
    bool surroundedByLake(Pos2 loc);
    bool processLakeBoundaryTile(std::list<Pos2>& boundList, Pos2 bound, Pos2 check, Height floodH); //given bound in lake boundary and check is a neighbor to bound, and floodH is the next flood height, test whether to add check to lake. Return true if next river flows out of check.
    void recurseSetLakeHeight(Height h, Pos2 pos);
}

#endif