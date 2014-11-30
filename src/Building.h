//
//  Building.h
//  Magnate
//
//  Created by Brian Kelley on 10/15/14288.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#ifndef __Magnate__Building__
#define __Magnate__Building__

#include "Cuboid.h"
#include "Coord.h"
#include <vector>

class Building
{
public:
    Building();
    virtual ~Building();
    unsigned long numCuboids();
    Cuboid& getCuboidRef(int index);
protected:
private:
    std::vector<Cuboid>* boxes;
    void addCuboid(Cuboid* c);
    bool isFront(Cuboid* c1, Cuboid* c2);
};

#endif