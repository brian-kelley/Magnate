#ifndef __TERRAIN_GEN_INCLUDED__
#define __TERRAIN_GEN_INCLUDED__

#include <iostream>
#include <queue>
#include <boost/graph/adjacency_list.hpp>

#include "Heightmap.h"
#include "Constants.h"
#include "Coord.h"
#include "World.h"
#include "Watershed.h"
#include "Erosion.h"

namespace TerrainGen
{
    void generate(Heightmap& world, Heightmap& biomes);
    void oldGeneration(Heightmap& world);
    void diamondSquare(Heightmap& world);
    void defuzz(Heightmap& world);       //remove non-water tiles that are surrounded by water on at least 3 sides (adjacent)
    void flattenWater(Heightmap& world); //force water tiles to have constant height at all 4 corners
    //Tests whether the mesh
    void scatterVolcanos(Heightmap& world);
    void addVolcano(Heightmap& world, int x, int y, short height, int radius); //create a cone-shaped hill (or cone-shaped pit if height < 0)
    void clearAll(Heightmap& world);  //fill world with water at sea level
    void sphereMask(Heightmap& world);
    void clampSeaLevel(Heightmap& world);
    void defaultGen(Heightmap& world);
    void combinedGen(Heightmap& world);
    void erosionGen();
    void shelfMask();  //simulate continental shelf by lowering ground near world boundary. Shaped like beveled square.
    void smooth(Heightmap& world, int iters = 1);
    void scatterCentralVolcanoes(Heightmap& world);
    void verticalNormalize();
    void stretchToFill(Heightmap& world);
    Height getAverageHeight(Heightmap& world);   //Get average height of non-ocean tiles
    Height getMaxHeight(Heightmap& world);
    float getLandArea(Heightmap& world);           //# non-water tiles as proportion
    void addWatershed(Heightmap& world, Heightmap& biomes, int numRivers, float cutoff);
    void addPeaks();
    void placeRivers(float headAlt, int num);
    int floodToHeight(Pos2 lastRiver, Height flood, bool markBoundary = false);
    Height getFloodHeight(Pos2 lastRiver);
    bool findOutlet(Pos2 lastRiver, Pos2& result);
    void fillToHeight(Pos2 pos, Height flood);
    void removeLake(Pos2 pos);
    void scaleHeight(int target, int maxH);
    void unsmooth(Height maxH);
    void roughCone(Heightmap& world);
    //Biome-related code
    enum Latitude
    {
        POLAR,
        MODERATE,
        TROPICAL
    };
    void assignBiomes(Heightmap& world, Heightmap& biomes);
    void biomeSmooth(int iters = 1);
    Ground decideGround(bool rain, bool highAlt, Latitude lat);
}

#endif