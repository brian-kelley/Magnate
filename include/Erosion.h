#ifndef __EROSION_H__
#define __EROSION_H__

#include "VecField.h"
#include "Heightmap.h"
#include "Watershed.h"
#include "Constants.h"
#include "DebugTools.h"
#include "Coord.h"
#include <iostream>

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

namespace Erosion
{
    void erosion(Heightmap& init, Heightmap& rainfall);    //run the simulation, return heightmap
    void simpleRunner(Heightmap& world);
    void deposit(Heightmap& world, Pos2& loc, Height& h);    //deposit height anywhere. Different behavior if in a pit or on a slope.
    int getDownhill(Heightmap& world, Pos2 loc);
    void fillPit(Heightmap& world, Pos2 loc, Height sed);
    extern Heightmap* world;
    extern Heightmap* rainfall;
};

#endif