#ifndef __EROSION_H__
#define __EROSION_H__

#include <iostream>
#include <vector>
#include "Heightmap.h"
#include "GlobalConfig.h"
#include "Watershed.h"
#include "DebugTools.h"
#include "Coord.h"
#include "GenTypes.h"

class Erosion
{
public:
    Erosion(Heightmap& worldHeights);
private:
    void simpleRunner();
    int getDownhill(Pos2 loc);
    void normalizeAndAdd();
    Heightmap& world;
    Heightmap erosionMap;
    Heightmap hitMap;
    static constexpr int maxHits = 80;
    static constexpr int maxChange = 50;
};

#endif
