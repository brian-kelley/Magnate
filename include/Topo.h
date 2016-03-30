#ifndef __TOPO_H__
#define __TOPO_H__

#include "Atlas.h"
#include "Coord.h"
#include "Heightmap.h"
#include "GlobalConfig.h"
#include <vector>
#include <iostream>

namespace Topo
{
    void generateTopo(Heightmap& heights);
}

#endif
