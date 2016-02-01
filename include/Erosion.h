#ifndef __EROSION_H__
#define __EROSION_H__

#include <iostream>
#include <vector>
#include "Heightmap.h"
#include "GlobalConfig.h"
#include "Watershed.h"
#include "DebugTools.h"
#include "Coord.h"
#include "World.h"
#include "GenTypes.h"

class Erosion
{
public:
    Erosion(Heightmap& worldHeights, std::vector<Pos2>& focusLocs);
private:
    void simpleRunner(Pos2 focus);
    int getDownhill(Pos2 loc);
    Heightmap& world;
    Heightmap hitMap;
    static constexpr int focusRad = 100;
    static constexpr int maxHits = 80;
};

#endif