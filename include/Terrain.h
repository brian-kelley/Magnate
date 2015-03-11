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
    NUM_TYPES
};

typedef struct
{
    GROUND g;
    unsigned char height;
} tNode_t;

/*  -I think only 3 links are needed between nodes
    -origin stores ptr to the node at the top-left corner
    -rendering just involves moving west, then drawing between every node encountered straight north or south
    -When view moves, try to place newly visible nodes in the mem locations of ones that just moved offscreen (for speed i guess)
*/

namespace Terrain
{
    extern long int wseed;
    extern void init(long int seed);
    extern std::map<GROUND, int> terrainTextures;
    extern tNode_t generateNode(double i, double j);
}

#endif
