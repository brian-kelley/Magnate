#ifndef __TERRAIN_GEN_INCLUDED__
#define __TERRAIN_GEN_INCLUDED__

#include <iostream>
#include <queue>
#define ROUGHNESS 0.7
#include "Constants.h"
#include "Coord.h"
#include "Chunk.h"
#include "World.h"

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
    void addRiver(Height minHeadwaterAlt);
    void flowRiverFromPoint(Pos2 loc);
    /*
     create a flat pool at the point, and return the location of where river continues
     */
    //void floodStep(Pos2 pos);
    //bool findOutlet(Pos2 pos, Pos2& outlet);
    Pos2 formLake(Pos2 lastRiver);
    Pos2 formLakeOld(Pos2 lastRiver);
    /*
     test whether there is a downhill route from loc, that doesn't go directly to a lake tile. Return a Direction or NO_DIRECTION
     */
    int getNonLakeDownhill(Pos2 loc);
    /*
     max of height values of four corners of tile
     */
    Height maxHeightOfTile(Pos2 loc);
    //Does the tile have 4 lake tiles around it?
    bool surroundedByLake(Pos2 loc);
    /*
     flood fill all lake tiles connected to loc to height flood
     */
    void setLakeTilesFlooding(Pos2 loc);
    void buildFlatLake(Height flood, Pos2 pos);
    bool isOutlet(Pos2 pos, Pos2& result);
    bool isDownhillChain(int num, Pos2 pos, bool flatOK);
    void tester();
    void defaultGen();
    void consolidateLakes();
    void fillRiverGaps();
    void shelfMask();  //simulate continental shelf by lowering ground near world boundary. Shaped like beveled square.
    void smooth(int iters = 1);
    void scatterCentralVolcanoes();
    void addEntropy();
    void verticalNormalize();
    void stretchToFill();
    Height* getHeightBuffer(); //Return a copy of world's heightmap
    void combinedGen();
    void addBuffer(Height* buf); //Add buffer over world's current heightmap
    Height getAverageHeight();   //Get average height of non-lake tiles
    float getLandArea();           //# non-water tiles as proportion
}

#endif