#ifndef __VECFIELD_H__
#define __VECFIELD_H__

#include <iostream>
#include <string>
#include "Constants.h"
#include "DebugTools.h"
#include "Coord.h"

class VecField
{
public:
    VecField(int x, int y);
    SmallVec get(int x, int y);
    SmallVec get(Pos2 loc);
    void set(SmallVec v, int x, int y);
    void set(SmallVec v, Pos2 loc);
private:
    int w;
    int h;
    SmallVec* data;
};

#endif