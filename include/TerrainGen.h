#ifndef __TERRAIN_GEN_INCLUDED__
#define __TERRAIN_GEN_INCLUDED__

#include <iostream>
#define ROUGHNESS 1
#include "Constants.h"
#include "Chunk.h"
#include "World.h"

class World;
class Chunk;    //have to forward-declare World and Chunk because of mutual includes

namespace TerrainGen
{
    void generate(World& world);
    void diamondSquare(World& world);
    void fillSquare(World& world, int x, int y, int size);
    void fillDiamond(World& world, int x, int y, int size);
    //Tests whether the mesh
    bool inMesh(int x, int y);
    Height getHeight(int avg, int size);
}

#endif