#ifndef __TERRAIN_GEN_INCLUDED__
#define __TERRAIN_GEN_INCLUDED__

#include <iostream>
#define ROUGHNESS 10
#include "Constants.h"
#include "Chunk.h"
#include "World.h"

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
    void addVolcano(int x, int y, short height, int radius); //create a cone-shaped hill (or cone-shaped pit if height < 0)
    void erodeStep(); //do one timestep's worth of erosion
    void clearAll();  //fill world with water at sea level
}

#endif