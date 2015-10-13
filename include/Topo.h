#ifndef __TOPO_H__
#define __TOPO_H__

#include "Atlas.h"
#include "Constants.h"
#include "Coord.h"
#include "Chunk.h"
#include "Renderer.h"
#include "TerrainGen.h"
#include "World.h"
#include <vector>
#include <iostream>

namespace Topo
{
    void generateTopo();       //build the map and store in texture on GPU
    void drawTopo();           //draw full-screen topo map
}

#endif