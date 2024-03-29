#ifndef __TERRAIN_GEN_H__
#define __TERRAIN_GEN_H__

#include <iostream>
#include <queue>
#include "GlobalConfig.h"
#include "Coord.h"
#include "Watershed.h"
#include "Heightmap.h"
#include "Erosion.h"

struct TerrainGen
{
    TerrainGen(Heightmap& heights, Heightmap& biomes);
    Heightmap& world;
    Heightmap& biomes;
    std::vector<Pos2> focusLocs;
    Heightmap rainfall;
    void generate();
    void defuzz();       //remove non-water tiles that are surrounded by water on at least 3 sides (adjacent)
    void flattenWater(); //force water tiles to have constant height at all 4 corners
    short getHeight(int avg, int size);
    void scatterVolcanos();
    void addVolcano(int x, int y, short height, int radius); //create a cone-shaped hill (or cone-shaped pit if height < 0)
    void clearAll();  //fill world with water at sea level
    void sphereMask();
    void clampSeaLevel();
    void simpleBiomes();
    void assignBiomes();
    short maxHeightOfTile(Pos2 loc);
    void getDSLayer();
    void scatterCentralVolcanoes();
    void addEntropy();
    void verticalNormalize();
    void stretchToFill();
    Ground getGround(int temperature, int wetness, int altitude);
    short getAverageHeight();   //Get average height of non-ocean tiles
    short getMaxHeight();
    float getLandArea();           //# non-water tiles as proportion
    void addWatershed(float cutoff, short maxH, short avgH);
    void scaleHeight(int target, int maxH);
    void unsmooth();
    void correctOceans();          //make sure that all y=0 tiles connect directly to the ocean
    void logisticHeightDist();
    void analyzeBiomes();
};

#endif
