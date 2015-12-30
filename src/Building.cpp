//
//  Building.cpp
//  Magnate
//
//  Created by Brian Kelley on 10/15/14288.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "Building.h"

using namespace std;

Building::Building()
{
    //default constrcutor just tests out the rendering
    int defaultWall = 0;
    int defaultRoof = 0;
    Cuboid c = {10, 2, -10, 1, 1, 1, defaultWall, defaultWall, defaultWall, defaultWall, defaultRoof};
    boxes.push_back(c);
}

void Building::addCuboid(Cuboid c)
{
    boxes.push_back(c);
}

Cuboid& Building::getCuboid(int index)
{
    return boxes[index];
}

int Building::numCuboids()
{
    return (int) this->boxes.size();
}