#include "Erosion.h"

using namespace std;
using namespace Coord;
using namespace RandomUtils;
using namespace GlobalConfig;

Erosion::Erosion(Heightmap& worldHeights) : world(worldHeights), erosionMap(WORLD_SIZE, WORLD_SIZE), hitMap(WORLD_SIZE, WORLD_SIZE)
{
    PRINT("Starting erosion...");
    //TODO: How many runners are needed?
    //How is that affected by world size?
    erosionMap.set(0);
    const int n = 100000;
    const int step = n / 10;
    for(int i = 0; i < n; i++)
    {
        simpleRunner();
        if(i % step == 0)
            cout << "i = " << i << endl;
    }
    erosionMap.normalize(maxChange);
    world += erosionMap;
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            if(world.get(i, j) < 0)
                world.set(0, i, j);
        }
    }
    PRINT("Done with erosion");
}

void Erosion::simpleRunner()
{
    Pos2 loc;
    do
    {
        loc.x = gen() % WORLD_SIZE;
        loc.y = gen() % WORLD_SIZE;
    }
    while(world.get(loc) <= 0);
    while(world.get(loc) > 0)
    {
        int downhill = getDownhill(loc);
        if(downhill == NO_DIRECTION)
        {
            erosionMap.add(1, loc);
            break;
        }
        else
        {
            erosionMap.add(-1, loc);
            loc = getTileInDir(loc, downhill);
        }
        hitMap.add(1, loc);
        if(hitMap.get(loc) == maxHits)
            break;
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