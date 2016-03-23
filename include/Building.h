#ifndef __BUILDING_H__
#define __BUILDING_H__

#include <vector>
#include "GenTypes.h"
#include "Coord.h"
#include "Atlas.h"

struct BuildingModel
{
    std::vector<int> model;
};

struct Building
{
    Building();
};

enum struct BldgType
{
    testBox
};

class TrainTerminal : public Building
{
public:
    
};

#endif