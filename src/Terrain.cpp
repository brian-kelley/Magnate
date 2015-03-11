//
//  Terrain.cpp
//  Magnate
//
//  Created by Brian Kelley on 3/2/1561.
//
//


#include "Terrain.h"
#include <stdio.h>
#include <stdlib.h>

using namespace std;
using namespace constants;
using namespace coord;
using namespace RenderRoutines;

long int Terrain::wseed;
map<GROUND, int> Terrain::terrainTextures;

void Terrain::init(long int seed)
{
    wseed = seed;
    //Manually map ground types to their textures
    terrainTextures[WATER] = texNumFromStr("water");
    terrainTextures[PLAINS] = texNumFromStr("grass");
    terrainTextures[FOREST] = texNumFromStr("forest");
    terrainTextures[MOUNTAINS] = texNumFromStr("mountains");
}

tNode_t Terrain::generateNode(double i, double j)
{
    tNode_t node;
    node.g = (GROUND) (rand() % GROUND::NUM_TYPES);
    node.height = rand() % 0xFF;
    return node;
}
