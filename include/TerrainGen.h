#ifndef __TERRAIN_GEN_INCLUDED__
#define __TERRAIN_GEN_INCLUDED__

#include "Constants.h"
#include <iostream>
#define ROUGHNESS 1

namespace TerrainGen
{
    void generate(mesh_t& dest, int i, int j);
    void diamondSquare(mesh_t& dest);
    void fillSquare(mesh_t& dest, int x, int y, int size);
    void fillDiamond(mesh_t& dest, int x, int y, int size);
    void diamond(mesh_t& dest, int x, int y, int size);
    void square(mesh_t& dest, int x, int y, int size);
    //Tests whether the mesh
    bool inMesh(int x, int y);
    unsigned char getHeight(int avg, int size);
}

#endif