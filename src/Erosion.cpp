#include "Erosion.h"

using namespace std;
using namespace constants;
using namespace Coord;
using namespace RandomUtils;

Heightmap* Erosion::world;
Heightmap* Erosion::rainfall;

void Erosion::erosion(Heightmap &init, Heightmap &rainmap)
{
    world = &init;
    rainfall = &rainmap;
    rainfall->diamondSquare(2, 0, 0, true);
    //TODO: How many runners are needed?
    //How is that affected by world size?
    for(int i = 0; i < 1000; i++)
        simpleRunner(*world);
}

void Erosion::simpleRunner(Heightmap &world)
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
        int downhill = getDownhill(world, loc);
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

int Erosion::getDownhill(Heightmap &world, Pos2 loc)
{
    Height locH = world.get(loc);
    Height minH = SHRT_MAX;
    int minDir = NO_DIRECTION;
    for(int dir = UP; dir <= RIGHT; dir++)
    {
        Height neiH = world.get(getTileInDir(loc, dir));
        if(neiH < locH)
        {
            minH = neiH;
            minDir = dir;
        }
    }
    return minDir;
}

void Erosion::fillPit(Heightmap &world, Pos2 loc, Height sed)
{
    //while sediment remains, raise loc & all equal neighbors to the height of the next lowest neighbor
    while(sed > 0)
    {
        Height locH = world.get(loc);
        Height lowNei = SHRT_MAX;
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