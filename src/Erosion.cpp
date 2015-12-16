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
    {
        cout << "Did " << i << " runners." << endl;
        runner(*world);
    }
}

void Erosion::deposit(Heightmap &world, Pos2& loc, Height& h)
{
    cout << " Depositing " << h << " height at " << loc << endl;
    int downhill = getDownhill(world, loc);
    if(downhill == NO_DIRECTION)
    {
        cout << "  In a pit." << endl;
        Height locH = world.get(loc);
        //In a pit
        //Find the next lowest neighbor
        while(h > 0)
        {
            //cout << "loc height: " << locH << endl;
            //cout << "neighbor heights: ";
            for(int dir = UP; dir <= RIGHT; dir++)
            {
                //cout << world.get(getTileInDir(loc, dir)) << " ";
            }
            //cout << endl;
            int lowH = SHRT_MAX;
            //Get the lowest neighbor (necessarily higher than loc)
            for(int dir = UP; dir <= RIGHT; dir++)
            {
                Height thisH = world.get(getTileInDir(loc, dir));
                if(thisH < lowH)
                    lowH = thisH;
            }
            if(lowH > locH)
            {
                //Only add height to loc, raise it up to the height of lowest neighbor.
                Height chg = lowH - locH;
                //cout << "locH is " << chg << " units lower than the lowest negihbor." << endl;
                if(chg > h)
                {
                    chg = h;
                    h = 0;
                }
                else
                    h -= chg;
                //cout << "Boosting loc by " << chg << endl;
                world.add(chg, loc);
            }
            else
            {
                //numDest is how many tiles will receive sediment.
                //starts at 1 for loc.
                //increased for every tile that is equal in height to
                Height nextLowest = SHRT_MAX;
                int numDest = 1;
                for(int dir = UP; dir <= RIGHT; dir++)
                {
                    Height neiH = world.get(getTileInDir(loc, dir));
                    if(neiH != locH && neiH < nextLowest)
                        nextLowest = locH;
                    Pos2 nei = getTileInDir(loc, dir);
                    if(world.get(nei) == lowH)
                        numDest++;
                }
                Height totalRaise;
                Height raise;
                //cout << "Spreading " << h << " among " << numDest << " tiles." << endl;
                if(numDest == 5)
                {
                    //unusual case, all sediment was distributed in loc and 4 neighbors with some left over.
                    
                    h = 0;
                    break;
                }
                //total amount of sediment to be distributed
                if(numDest == 1)
                {
                    cout << "1 destination tile." << endl;
                    totalRaise = nextLowest - locH;
                    cout << "Difference between loc and lowest neighbor is " << totalRaise << endl;
                    DBASSERT(totalRaise >= 0);
                    DBASSERT(totalRaise != 0);
                    if(totalRaise > h)
                        totalRaise = h;
                }
                else
                {
                    cout << "Distributing " << h << " among " << numDest << " tiles." << endl;
                    totalRaise = numDest * (nextLowest - locH);
                    if(totalRaise > h)
                        totalRaise = h;
                }
                h -= totalRaise;
                cout << "Total raise is " << totalRaise << endl;
                //cout << "totalRaise is " << totalRaise << endl;
                //amount of height distributed to each tile
                raise = 0.5 + totalRaise / numDest;
                cout << "Raise is " << raise << endl;
                //now go back to the neighbors equal in height to loc, and add raise to them
                for(int dir = UP; dir <= RIGHT; dir++)
                {
                    Pos2 nei = getTileInDir(loc, dir);
                    if(world.get(nei) == locH)
                        world.add(raise, nei);
                }
            }
        }
        DBASSERT(h != 0);
    }
    else
    {
        cout << "  On a hillside." << endl;
        //Not in a pit. Add height only to the downhill tile.
        //Finally, change loc to the downhill tile.
        Pos2 lowNei = getTileInDir(loc, downhill);
        Height raise = world.get(loc) - world.get(lowNei);
        if(raise > h)
        {
            raise = h;
            h = 0;
        }
        else
            h -= raise;
        world.add(raise, lowNei);
        //move loc to the new position
        loc = lowNei;
    }
    cout << " Done depositing." << endl;
}

void Erosion::runner(Heightmap &world)
{
    //Pick a location with height > 0
    Pos2 loc = {0, 0};
    while(world.get(loc) < 1)
    {
        loc.x = gen() % WORLD_SIZE;
        loc.y = gen() % WORLD_SIZE;
    }
    float sedPerSteep = 0.4;         //for each unit of steepness, how much sediment is removed
    //simply pulls some sediment from the current point if steep (up to saturation limit) or deposits if not steep.
    //move runner downhill
    //repeat until sea level reached by runner
    int cap = rainfall->get(loc) / 100;   //capacity for sediment flow, based on amount of rainfall at starting point
    Height sed = world.getSteepness(loc) * sedPerSteep;          //amount of sediment in runner
    //if steepness > 20, must
    const int cutoff = 20;                //steepness above means sediment removed, below means deposited
    //cout << "starting runner at " << loc.x << ", " << loc.y << " with " << sed << " sediment." << endl;
    while(world.get(loc) > 0 && sed != 0)
    {
        //cout << "  starting iteration with " << sed << " sediment left." << endl;
        if(world.getSteepness(loc) > cutoff && getDownhill(world, loc) != NO_DIRECTION)
        {
            //remove sediment: (steepness - cutoff) * lossPerSteepness
            //but don't exceed capacity (cap)
            Height loss = world.getSteepness(loc) * sedPerSteep;
            if(loss + sed > cap)
            {
                //max out capacity
                loss = cap - sed;
                sed = cap;
            }
            else
                sed += loss;
            world.add(-loss, loc);
            int downhill = getDownhill(world, loc);
            if(downhill == NO_DIRECTION)
            {
                //even though ground is steep, all sediment is deposited because runner flowed into a depression
                //note: deposit will set sed to 0
                deposit(world, loc, sed);
            }
            else
            {
                //move runner downhill
                loc = getTileInDir(loc, downhill);
            }
        }
        else
        {
            //deposit sediment: (cutoff - steepness) * lossPerSteepness
            //but sed can't go below 0
            Height deposition = world.getSteepness(loc) * sedPerSteep;
            if(sed < deposition)
            {
                deposition = sed;
                sed = 0;
            }
            else
                sed -= deposition;
            //note: deposit will move loc to a downhill neighbor
            deposit(world, loc, deposition);
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