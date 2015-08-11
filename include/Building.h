//
//  Building.h
//  Magnate
//
//  Created by Brian Kelley on 10/15/14288.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#ifndef __Magnate__Building__
#define __Magnate__Building__

#include <vector>
#include "Coord.h"
#include "Atlas.h"

struct Cuboid
{
    float x;
    float y;
    float z;
    float xsize;
    float ysize;
    float zsize;
    int wallTex1;
    int wallTex2;
    int wallTex3;
    int wallTex4;
    int roofTex;
};

enum BUILDINGS
{
    //Put factory/building types here
};

class Building
{
public:
    Building();
    ~Building() {};
    int numCuboids();
    Cuboid& getCuboid(int index);
protected:
private:
    std::vector<Cuboid> boxes;
    void addCuboid(Cuboid c);
    BUILDINGS bldgType;
};

#endif