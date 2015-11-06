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

map<Ground, int> Terrain::terrainTextures;

void Terrain::init()
{
    terrainTextures[WATER] = texNumFromStr("water");
    terrainTextures[PLAINS] = texNumFromStr("grass");
    terrainTextures[FOREST] = texNumFromStr("forest");
    terrainTextures[MOUNTAINS] = texNumFromStr("mountains");
    terrainTextures[DESERT] = texNumFromStr("sand");
    terrainTextures[RIVER] = texNumFromStr("river");
    terrainTextures[LAKE] = texNumFromStr("lake");
    terrainTextures[FLOODING] = texNumFromStr("mountains");
    terrainTextures[OUTLET_SEARCHED] = texNumFromStr("forest");
    terrainTextures[LAKE_BOUNDARY] = texNumFromStr("test1");
    terrainTextures[VISITED] = texNumFromStr("test1");
}