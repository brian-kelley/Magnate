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
    terrainTextures[WATER] = texNumFromStr("water");
    terrainTextures[PLAINS] = texNumFromStr("grass");
    terrainTextures[FOREST] = texNumFromStr("forest");
    terrainTextures[MOUNTAINS] = texNumFromStr("mountains");
    terrainTextures[DESERT] = texNumFromStr("sand");
}

tNode_t Terrain::generateNode(double i, double j)
{
    tNode_t node;
    node.g = (GROUND) ((abs(int(i / 2))) % GROUND::NUM_TYPES);
    node.height = 100 + rand() % 12;
    node.g = WATER;
    node.height = 0;
    return node;
}