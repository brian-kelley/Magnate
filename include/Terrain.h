//
//  Terrain.h
//  Magnate
//
//  Created by Brian Kelley on 3/2/1561.
//
//

#ifndef Magnate_Terrain_h
#define Magnate_Terrain_h

#include <map>
#include <string>
#include "Constants.h"
#include "Coord.h"
#include "RenderRoutines.h"

//Don't manually assign int values to any of these!
enum GROUND : unsigned char
{
    WATER,
    PLAINS,
    FOREST,
    MOUNTAINS,
    DESERT,
    NUM_TYPES
};

typedef struct
{
    GROUND g;
    unsigned char height;
} tNode_t;

namespace Terrain
{
    extern long int wseed;
    extern void init(long int seed);
    extern std::map<GROUND, int> terrainTextures;
    extern tNode_t generateNode(double i, double j);
}

#endif
