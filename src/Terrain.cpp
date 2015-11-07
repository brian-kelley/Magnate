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
using namespace Coord;
using namespace RenderRoutines;

int Terrain::terrainTextures[Ground::NUM_TYPES];

void Terrain::init()
{
    terrainTextures[WATER] = texNumFromStr("water");
    terrainTextures[PLAINS] = texNumFromStr("grass");
    terrainTextures[FLOODPLAINS] = texNumFromStr("floodplains");
    terrainTextures[CONIFER_FOREST] = texNumFromStr("forest");
    terrainTextures[DECID_FOREST] = texNumFromStr("forest");
    terrainTextures[MOUNTAINS] = texNumFromStr("mountains");
    terrainTextures[STONE] = texNumFromStr("mountains");
    terrainTextures[SNOWCAP] = texNumFromStr("snow");
    terrainTextures[TAIGA] = texNumFromStr("taiga");
    terrainTextures[TUNDRA] = texNumFromStr("tundra");
    terrainTextures[DESERT] = texNumFromStr("sand");
    terrainTextures[BEACH] = texNumFromStr("sand");
    terrainTextures[RIVER] = texNumFromStr("water");
    terrainTextures[LAKE] = texNumFromStr("water");
    terrainTextures[RAINFOREST] = texNumFromStr("rainforest");
    terrainTextures[OUTLET_SEARCHED] = texNumFromStr("forest");
    terrainTextures[LAKE_BOUNDARY] = texNumFromStr("test1");
}