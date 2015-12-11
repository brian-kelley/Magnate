#include "VecField.h"

VecField::VecField(int x, int y)
{
    data = new SmallVec[x * y];
    w = x;
    h = y;
}

SmallVec VecField::get(int x, int y)
{
    return data[x + y * w];
}

SmallVec VecField::get(Pos2 loc)
{
    return data[loc.x + loc.y * w];
}

void VecField::set(SmallVec v, int x, int y)
{
    data[x + y * w] = v;
}

void VecField::set(SmallVec v, Pos2 loc)
{
    data[loc.x + loc.y * w] = v;
}