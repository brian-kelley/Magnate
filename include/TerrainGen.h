#ifndef __TERRAIN_GEN_INCLUDED__
#define __TERRAIN_GEN_INCLUDED__

#include <iostream>
#include <queue>
#define ROUGHNESS 2.0
#include "Constants.h"
#include "Coord.h"
#include "Chunk.h"
#include "World.h"
#include "Watershed.h"

namespace TerrainGen
{
    void generate();
    void diamondSquare();
    void defuzz();       //remove non-water tiles that are surrounded by water on at least 3 sides (adjacent)
    void flattenWater(); //force water tiles to have constant height at all 4 corners
    void fillSquare(int x, int y, int size);
    void fillDiamond(int x, int y, int size);
    //Tests whether the mesh
    bool inMesh(int x, int y);
    Height getHeight(int avg, int size);
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
    Height maxHeightOfTile(Pos2 loc);
    void tester();
    void defaultGen();
    void shelfMask();  //simulate continental shelf by lowering ground near world boundary. Shaped like beveled square.
    void smooth(int iters = 1);
    void fancySmooth();
    void scatterCentralVolcanoes();
    void addEntropy();
    void verticalNormalize();
    void stretchToFill();
    Height* getHeightBuffer(); //Return a copy of world's heightmap
    void combinedGen();
    void addBuffer(Height* buf); //Add buffer over world's current heightmap
    Height getAverageHeight();   //Get average height of non-ocean tiles
    Height getMaxHeight();
    float getLandArea();           //# non-water tiles as proportion
    void addWatershed(float cutoff, Height maxH, Height avgH);
    void placeRivers(float headAlt, int num);
    int floodToHeight(Pos2 lastRiver, Height flood, bool markBoundary = false);
    Height getFloodHeight(Pos2 lastRiver);
    bool findOutlet(Pos2 lastRiver, Pos2& result);
    void fillToHeight(Pos2 pos, Height flood);
    void removeLake(Pos2 pos);
    void riverHeightAdjust();
    void scaleHeight(int target, int maxH);
    void unsmooth(Height maxH);
    //Biome-related code
    typedef std::vector<std::vector<bool>> RainMap;
    enum Latitude
    {
        POLAR,
        MODERATE,
        TROPICAL
    };
    void assignBiomes();
    void addRainCircle(Pos2 loc, int r, RainMap& rmap);
    bool isCoastal(Pos2 loc);
    void addRandomRain(RainMap& rmap);
    void addCoastalRain(RainMap& rmap);
    void addRiverRain(RainMap& rmap);
    void addTropicalRain(int bandWidth, RainMap& rmap);
    Ground decideGround(bool rain, bool highAlt, Latitude lat);
}

#endif