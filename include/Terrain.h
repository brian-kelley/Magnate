//
//  Terrain.h
//  Magnate
//
//  Created by Brian Kelley on 3/2/1561.
//
//

#ifndef Magnate_Terrain_h
#define Magnate_Terrain_h

#include "Terrain.h"

//Don't manually assign int values to any of these!
enum GROUND : unsigned char
{
    WATER,
    PASTURE,
    STONE,
    MARSH,
    SNOW,
    FOREST,
    JUNGLE,
    MOUNTAINS,
    DESERT,
    NUM_TYPES
};

#endif