#ifndef __BUILDING_H__
#define __BUILDING_H__

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
};

#endif