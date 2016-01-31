#ifndef __TERRAIN_GEN_INCLUDED__
#define __TERRAIN_GEN_INCLUDED__

#include <iostream>
#include <queue>
#include <boost/graph/adjacency_list.hpp>
#include "Heightmap.h"
#include "Coord.h"
#include "World.h"
#include "Watershed.h"
#include "Erosion.h"
#include "GlobalConfig.h"

class TerrainGen
{
public:
    TerrainGen(Heightmap& worldHeights, Heightmap& worldBiomes);
private:
    void oldGeneration();
    void diamondSquare();
    void pyramidGen();
    void defuzz();       //remove non-water tiles that are surrounded by water on at least 3 sides (adjacent)
    void flattenWater(); //force lake/ocean tiles to be flat (same height)
    //Tests whether the mesh
    void scatterVolcanos();
    void addVolcano(int x, int y, short height, int radius); //create a cone-shaped hill (or cone-shaped pit if height < 0)
    void clearAll();  //fill world with water at sea level
    void sphereMask();
    void clampSeaLevel();
    void defaultGen();
    void combinedGen();
    void flat();
    void erosionGen();
    void shelfMask();  //simulate continental shelf by lowering ground near world boundary. Shaped like beveled square.
    void smooth(int iters = 1);
    void scatterCentralVolcanoes();
    void verticalNormalize();
    void stretchToFill();
    short getAverageHeight();   //Get average height of NON-OCEAN tiles
    short getMaxHeight();
    float getLandArea();   //# non-water tiles as proportion
    void addWatershed(int numRivers, float cutoff);
    void scaleHeight(int target, int maxH);
    void unsmooth(short maxH);
    void roughCone();
    void getSlope();
    void getCanal();
    void simpleBiomes();
    //Biome-related code
    enum Latitude
    {
        POLAR,
        MODERATE,
        TROPICAL
    };
    void assignBiomes(Heightmap& rainfall);
    void biomeSmooth(int iters = 1);
    Ground decideGround(bool rain, bool highAlt, Latitude lat);
    //access world's heightmaps
    Heightmap& world;
    Heightmap& biomes;
};

#endif