#include "Watershed.h"

using namespace std;
using namespace GlobalConfig;
using namespace Coord;

Watershed::Watershed(Heightmap& worldHeights, Heightmap& worldBiomes, int numRivers, short minH) : world(worldHeights), biomes(worldBiomes)
{
    for(int i = 0; i < numRivers; i++)
        addRiver(minH);
}

void Watershed::addRiver(short minHeight)
{
    Pos2 loc(0, 0);
    int tries = 0;
    while(world.get(loc) < minHeight || biomes.get(loc) == LAKE || biomes.get(loc) == RIVER)
    {
        if(tries++ > 1000)
        {
            cout << "Error: Timeout to find high elevation headwaters." << endl;
            return;
        }
        loc.x = RandomUtils::gen() % WORLD_SIZE;
        loc.y = RandomUtils::gen() % WORLD_SIZE;
    }
    while(true) //Flow downhill until reach ocean or y = 0 (only stopping conditions)
    {
        biomes.set(RIVER, loc);
        int downhill = getNonLakeDownhillDir(loc);
        if(downhill != NO_DIRECTION)
        {
            //Check if just flowed into a lake that already flows out to sea
            Pos2 next = getTileInDir(loc, downhill);
            if(biomes.get(next) == LAKE || biomes.get(next) == RIVER)
                return;
        }
        //As long as there is nowhere to flow, form lake
        while(downhill == NO_DIRECTION)
        {
            loc = formLake(loc);
            if(loc.x == -1 || biomes.get(loc) == WATER || world.get(loc) <= 0)
                return;
            
            downhill = getNonLakeDownhillDir(loc);
        }
        loc = getTileInDir(loc, downhill);
        //Decide whether to end
        if(biomes.get(loc) == WATER || world.get(loc) <= 0)
            break;
    }
}

int Watershed::getNonLakeDownhillDir(Pos2 loc)
{
    int poss[4];
    int iter = 0;
    short h = world.get(loc);
    for(int dir = UP; dir <= RIGHT; dir++)
    {
        Pos2 nei = getTileInDir(loc, dir);
        if(world.get(nei) < h && biomes.get(nei) != LAKE_BOUNDARY)
        {
            poss[iter] = dir;
            iter++;
        }
    }
    if(iter == 0)
        return NO_DIRECTION;
    else
        return poss[RandomUtils::gen() % iter];
}

int Watershed::getNonLakeEqualDir(Pos2 loc, short flood)
{
    for(int dir = UP; dir <= RIGHT; dir++)
    {
        Pos2 nei = getTileInDir(loc, dir);
        if(world.get(nei) == flood && biomes.get(nei) != LAKE_BOUNDARY)
            return dir;
    }
    return NO_DIRECTION;
}

bool Watershed::surroundedByLake(Pos2 loc)
{
    for(int dir = UP; dir <= RIGHT; dir++)
    {
        if(biomes.get(getTileInDir(loc, dir)) != LAKE_BOUNDARY)
            return false;
    }
    return true;
}

bool Watershed::hasLakeNeighbor(Pos2 loc)
{
    for(int dir = UP; dir <= RIGHT; dir++)
    {
        if(biomes.get(getTileInDir(loc, dir)) == LAKE)
            return true;
    }
    return false;
}

bool Watershed::hasDownhillFlow(Pos2 loc)
{
    if(biomes.get(loc) != RIVER)
        return false;
    for(int dir = UP; dir <= RIGHT; dir++)
    {
        Pos2 nei = getTileInDir(loc, dir);
        if(world.get(nei) < world.get(loc) && biomes.get(nei) == RIVER)
           return true;
    }
    return false;
}

bool Watershed::hasOutlet(Pos2 loc)
{
    if(biomes.get(loc) != LAKE)
        return false;
    queue<Pos2> q;
    q.push(loc);
    biomes.set(OUTLET_SEARCHED, loc);
    bool found = false;
    while(q.size() > 0)
    {
        Pos2 proc = q.front();
        q.pop();
        if(biomes.get(proc) == LAKE)
        {
            biomes.set(OUTLET_SEARCHED, proc);
            //Check for river flow downhill
            //if found remember to return true
            //if not found mark as visited and continue iteration
            for(int dir = UP; dir <= RIGHT; dir++)
            {
                Pos2 nei = getTileInDir(proc, dir);
                if(biomes.get(nei) == RIVER && getNonLakeDownhillDir(nei) != NO_DIRECTION)
                    found = true;
                else if(biomes.get(nei) == LAKE)
                    q.push(nei);
            }
        }
        if(found)
            break;
    }
    //Now restore all OUTLET_SEARCHED tiles to LAKE
    while(q.size())
        q.pop();
    q.push(loc);
    biomes.set(LAKE, loc);
    while(q.size() > 0)
    {
        Pos2 proc = q.front();
        q.pop();
        if(biomes.get(proc) == OUTLET_SEARCHED)
        {
            biomes.set(LAKE, proc);
            for(int dir = UP; dir <= RIGHT; dir++)
            {
                Pos2 nei = getTileInDir(proc, dir);
                if(biomes.get(proc) == OUTLET_SEARCHED)
                    q.push(nei);
            }
        }
    }
    return found;
}

Pos2 Watershed::formLake(Pos2 loc)
{
    vector<Pos2> boundary;
    vector<Pos2> outlets;
    short flood = world.get(loc);
    biomes.set(LAKE_BOUNDARY, loc);
    boundary.push_back(loc);
    while(outlets.size() == 0)
    {
        //cout << "Have flood height " << flood << endl;
        //cout << "Iter " << iteration++ << endl;
        vector<Pos2> newBound;
        newBound.reserve(boundary.size());
        int updates = 1;
        while(updates > 0)
        {
            newBound.clear();
            updates = 0;
            for(Pos2 iter : boundary)
            {
                if(getNonLakeDownhillDir(iter) != NO_DIRECTION)
                    outlets.push_back(iter);
                //Otherwise, add all tiles that are equal in height, and mark them as lakes
                int equalDir = getNonLakeEqualDir(iter, flood);
                while(equalDir != NO_DIRECTION)
                {
                    Pos2 nei = getTileInDir(iter, equalDir);
                    biomes.set(LAKE_BOUNDARY, nei);
                    newBound.push_back(nei);
                    updates++;
                    equalDir = getNonLakeEqualDir(iter, flood);
                }
                if(!surroundedByLake(iter))
                    newBound.push_back(iter);
            }
            boundary = newBound;
        }
        if(outlets.size() > 0)
            break;
        //Now find flood height for next iteration
        flood = SHRT_MAX;
        for(Pos2 iter : boundary)
        {
            for(int dir = UP; dir <= RIGHT; dir++)
            {
                Pos2 nei = getTileInDir(iter, dir);
                if(biomes.get(nei) != LAKE_BOUNDARY && world.get(nei) < flood)
                    flood = world.get(nei);
            }
        }
    }
    queue<Pos2> fillq;
    fillq.push(loc);
    bool alreadyHasOutlet = false;
    while(fillq.size() > 0)
    {
        Pos2 proc = fillq.front();
        fillq.pop();
        if(biomes.get(proc) == LAKE_BOUNDARY || world.get(proc) != flood)
        {
            world.set(flood, proc);
            biomes.set(LAKE, proc);
            //If proc has a neighbor that is a river that flows downhill...
            for(int dir = UP; dir <= RIGHT; dir++)
            {
                Pos2 nei = getTileInDir(proc, dir);
                if(biomes.get(nei) == LAKE_BOUNDARY)
                    fillq.push(nei);
            }
        }
    }
    if(alreadyHasOutlet)    //Don't continue flow
        return {-1, -1};
    else
        return outlets[RandomUtils::gen() % outlets.size()];
}
