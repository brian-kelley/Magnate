#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include <string>

enum Ground : unsigned char
{
    WATER,
    PLAINS,
    DECID_FOREST,
    BOREAL_FOREST,
    MOUNTAINS,      //Exposed granite or some other stone
    SAVANNA,
    DESERT,
    RIVER,    //like water but used for erosion and not necessarily flat
    LAKE,     //like ocean (flat) but with altitude > 0
    BEACH,
    RAINFOREST,
    TUNDRA,
    SNOWCAP,        //Like mountains but mostly covered with snow
    //Utilties
    OUTLET_SEARCHED, //marks tiles visited by lake outlet searching
    LAKE_BOUNDARY,   //marks tiles of lakes under construction
    OCEAN_SEARCHED,  //marks real ocean tiles
    NUM_TYPES
};

namespace Terrain
{
    std::string getTextureName(Ground g);
    std::string getName(Ground g);
}

#endif
