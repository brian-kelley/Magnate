#ifndef __TERRAIN_GEN_INCLUDED__
#define __TERRAIN_GEN_INCLUDED__

#include <iostream>
#include <queue>
#include "Coord.h"
#include "World.h"
#include "Watershed.h"
#include "GlobalConfig.h"
#include "Erosion.h"

struct TerrainGen
{
    TerrainGen();
    Heightmap& world;
    Heightmap& biomes;
    void generate();
    void defuzz();       //remove non-water tiles that are surrounded by water on at least 3 sides (adjacent)
    void flattenWater(); //force water tiles to have constant height at all 4 corners
    //Tests whether the mesh
    bool inMesh(int x, int y);
    void scatterVolcanos();
    void addVolcano(int x, int y, short height, int radius); //create a cone-shaped hill (or cone-shaped pit if height < 0)
    void erode(int numTimesteps); //do one timestep's worth of erosion
    void clearAll();  //fill world with water at sea level
    void pyramidMask(); //lower elevation corresponding to distance from world center, so that around the edges is only water
    void sphereMask();
    void clampSeaLevel();
    /*
     max of height values of four corners of tile
     */
    short maxHeightOfTile(Pos2 loc);
    void defaultGen();
    void shelfMask();  //simulate continental shelf by lowering ground near world boundary. Shaped like beveled square.
    void smooth(int iters = 1);
    void scatterCentralVolcanoes();
    void addEntropy();
    void verticalNormalize();
    void stretchToFill();
    void combinedGen();
    short getAverageHeight();   //Get average height of non-ocean tiles
    short getMaxHeight();
    float getLandArea();           //# non-water tiles as proportion
    void addWatershed(float cutoff, short maxH, short avgH);
    void placeRivers(float headAlt, int num);
    int floodToHeight(Pos2 lastRiver, short flood, bool markBoundary = false);
    short getFloodHeight(Pos2 lastRiver);
    bool findOutlet(Pos2 lastRiver, Pos2& result);
    void fillToHeight(Pos2 pos, short flood);
    void removeLake(Pos2 pos);
    void riverHeightAdjust();
    void scaleHeight(int target, int maxH);
    void unsmooth();
    //Biome-related code
    enum Latitude
    {
        POLAR,
        MODERATE,
        TROPICAL
    };
    void assignBiomes();
    Ground decideGround(bool rain, bool highAlt, Latitude lat);
};

#endif