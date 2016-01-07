#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include <string>

enum Ground : unsigned char
{
    WATER,
    PLAINS,
    FLOODPLAINS,
    CONIFER_FOREST,
    DECID_FOREST,
    MOUNTAINS,
    DESERT,
    RIVER,    //like water but used for erosion and not necessarily flat
    LAKE,     //like ocean (flat) but with altitude > 0
    BEACH,
    RAINFOREST,
    TUNDRA,
    TAIGA,
    STONE,
    SNOWCAP,
    OUTLET_SEARCHED, //marks tiles visited by lake outlet searching
    LAKE_BOUNDARY,   //marks tiles of lakes under construction
    NUM_TYPES
};

namespace Terrain
{
    std::string getTextureName(Ground g);
}

#endif