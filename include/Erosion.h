#ifndef __EROSION_H__
#define __EROSION_H__

#include <iostream>
#include "Heightmap.h"
#include "GlobalConfig.h"
#include "Watershed.h"
#include "DebugTools.h"
#include "Coord.h"

/*
 Erosion simulation
 -start with a large cone
 -add some entropy so the erosion isn't perfectly symmetrical
 -ds to get continuous rainfall function
 -for each timestep (note - only steepness needs to be recalculated):
    -compute steepness function
    -compute vegetation function (+rainfall, -steepness)
    -compute map of height loss (+rainfall, +steepness, -vegetation)
    -compute map of soil capture rate (+vegetation, -steepness)
    -compute distribution of actual soil capture (follow downhill)
    -add height loss map to world
    -add soil capture map to world
 */

class Erosion
{
public:
    Erosion(Heightmap& worldHeights, Heightmap& worldRainfall);
private:
    void simpleRunner();
    int getDownhill(Pos2 loc);
    void fillPit(Pos2 loc, short sed);
    Heightmap& world;
    Heightmap& rainfall;
};

#endif