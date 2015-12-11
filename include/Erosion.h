#ifndef __EROSION_H__
#define __EROSION_H__

#include "VecField.h"
#include "Heightmap.h"
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
    Heightmap erosion();    //run the simulation, return heightmap
    void timestep(Heightmap& world, Heightmap& rainfall);
    Heightmap getConeWithEntropy();
    Heightmap getRainfall();
    Heightmap getVegetation(Heightmap& rainfall, Heightmap& steepness);
    Heightmap getSteepness(Heightmap& world);
    Heightmap getHeightLoss(Heightmap& rainfall, Heightmap& vegetation, Heightmap& steepness);
    Heightmap getSoilCaptureRate(Heightmap& vegetation, Heightmap& steepness);
    void getActualSoilCapture(Heightmap& world, Heightmap heightLoss, Heightmap& captureRate);
    VecField getDownhillField(Heightmap& world);
    void addChanges(Heightmap& world, Heightmap& loss, Heightmap& capture);
    void flow(Heightmap& hm, VecField& vf);
    void normalize(Heightmap& hm, int range = 1000);  //normalize field to [0, 1000)
    extern float NORM_TO_H;         //how to scale 0-1000 values to heights
};

#endif