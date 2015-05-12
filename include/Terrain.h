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

namespace Terrain
{
    extern void init();
    extern std::map<GROUND, int> terrainTextures;
}

#endif
