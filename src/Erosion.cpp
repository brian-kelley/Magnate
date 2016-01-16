#include "Erosion.h"

using namespace std;
using namespace Coord;
using namespace RandomUtils;

Erosion::Erosion(Heightmap& worldHeights, Heightmap& worldRainfall) : world(worldHeights), rainfall(worldRainfall)
{
    rainfall.diamondSquare(2, 0, 0, true);
    //TODO: How many runners are needed?
    //How is that affected by world size?
    for(int i = 0; i < 1000; i++)
        simpleRunner();
}

void Erosion::simpleRunner()
{
    Pos2 loc;
    do
    {
        loc.x = gen() % GlobalConfig::WORLD_SIZE;
        loc.y = gen() % GlobalConfig::WORLD_SIZE;
    }
    while(world.get(loc) <= 0);
    while(world.get(loc) > 0)
    {
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

void Erosion::fillPit(Pos2 loc, short sed)
{
    //while sediment remains, raise loc & all equal neighbors to the height of the next lowest neighbor
    while(sed > 0)
    {
        short locH = world.get(loc);
        short lowNei = SHRT_MAX;
        for(int dir = UP; dir <= RIGHT; dir++)
        {
            Pos2 nei = getTileInDir(loc, dir);
            if(world.get(nei) < lowNei && world.get(nei) > locH)
            {
                lowNei = world.get(nei);
            }
        }
        //cout << endl;
        //cout << "next lowest neighbor height is " << lowNei << endl;
        if(lowNei == SHRT_MAX)
        {
            //loc and all neighbors raised to the max, do nothing
            return;
        }
        int numRaise = 1;   //# of tiles to be raised, including loc
        int change = lowNei - locH; //maximum height change of each tile
        for(int dir = UP; dir <= RIGHT; dir++)
        {
            if(world.get(getTileInDir(loc, dir)) == lowNei)
                numRaise++;
        }
        if(change * numRaise > sed)
            change = 0.5 + float(sed) / numRaise;
        if(change == 0)
            break;
        //deduct sediment
        sed -= change * numRaise;
        //apply raise
        for(int dir = UP; dir <= RIGHT; dir++)
        {
            Pos2 nei = getTileInDir(loc, dir);
            if(world.get(nei) == lowNei)
                world.add(change, nei);
        }
        world.add(change, loc);
        if(sed <= 0)
            return;
    }
}