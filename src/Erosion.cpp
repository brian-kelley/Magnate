#include "Erosion.h"

using namespace std;
using namespace Coord;
using namespace RandomUtils;
using namespace GlobalConfig;

Erosion::Erosion(Heightmap& worldHeights, vector<Pos2>& focusLocs) : world(worldHeights), hitMap(WORLD_SIZE, WORLD_SIZE)
{
    //TODO: How many runners are needed?
    //How is that affected by world size?
    hitMap.set(0);
    for(int i = 0; i < focusLocs.size(); i++)
    {
        for(int j = 0; j < 20000 / focusLocs.size(); j++)
        {
            simpleRunner(focusLocs[i]);
        }
    }
    world.smooth();
}

void Erosion::simpleRunner(Pos2 focus)
{
    Pos2 loc;
    do
    {
        loc.x = focus.x + (gen() % focusRad) - (focusRad / 2);
        loc.y = focus.y + (gen() % focusRad) - (focusRad / 2);
    }
    while(world.get(loc) <= 0);
    while(world.get(loc) > 0)
    {
        hitMap.add(1, loc);
        int downhill = getDownhill(loc);
        if(downhill == NO_DIRECTION)
        {
            world.add(1, loc);
            break;
        }
        else
        {
            world.add(-1, loc);
            loc = getTileInDir(loc, downhill);
            if(hitMap.get(loc) > maxHits)
                break;
        }
    }
}

int Erosion::getDownhill(Pos2 loc)
{
    short locH = world.get(loc);
    short minH = SHRT_MAX;
    int minDir = NO_DIRECTION;
    for(int dir = UP; dir <= RIGHT; dir++)
    {
        short neiH = world.get(getTileInDir(loc, dir));
        if(neiH < locH)
        {
            minH = neiH;
            minDir = dir;
        }
    }
    return minDir;
}