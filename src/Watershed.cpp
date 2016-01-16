#include "Watershed.h"

using namespace std;
using namespace Coord;
using namespace GlobalConfig;

Watershed::Watershed(Heightmap& worldMap, Heightmap& biomeMap, short threshold, int numRivers) : world(worldMap), biomes(biomeMap)
{
    Pos2 loc(0, 0);
    int tries = 0;
    while(world.get(loc) < threshold || biomes.get(loc) == LAKE || biomes.get(loc) == RIVER || biomes.get(loc) == STONE)
    {
        if(tries++ > 1000)
        {
            cout << "Error: Timeout to find high elevation headwaters." << endl;
            return;
        }
        loc.x = RandomUtils::gen() % GlobalConfig::WORLD_SIZE;
        loc.y = RandomUtils::gen() % GlobalConfig::WORLD_SIZE;
    }
    while(true) //Flow downhill until reach ocean or y = 0 (only stopping conditions)
    {
        biomes.set(RIVER, loc);
        int downhill = getNonLakeDownhillDir(loc);
        if(downhill != NO_DIRECTION);
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
        if(biomes.get(loc) == WATER || biomes.get(loc) <= 0)
            break;
    }
}

void Watershed::beginFlow(Pos2 pos)
{
    generalFlow(pos);
}

void Watershed::generalFlow(Pos2 pos)
{
    queue<Pos2> q;
    q.push(pos);
    while(q.size() > 0)
    {
        Pos2 proc = q.front();
        q.pop();
        processTile(proc, q);
    }
}

Pos2 Watershed::getLowestNeighbor(Pos2 pos)
{
    Pos2 bestPos = {-1, -1};
    short bestH = SHRT_MAX;
    for(int dir = UP; dir <= RIGHT; dir++)
    {
        Pos2 nei = getTileInDir(pos, dir);
        if(world.get(nei) < bestH)
        {
            bestH = world.get(nei);
            bestPos = nei;
        }
    }
    return bestPos;
}

void Watershed::processTile(Pos2 pos, std::queue<Pos2>& q)
{
    Pos2 lowestNei = getLowestNeighbor(pos);
    if(world.get(lowestNei) <= 0)        //Flowed into ocean, done
        return;
    if(lowestNei.x == -1)
        return;     //should never get here
    if(world.get(lowestNei) < world.get(pos))
    {
        //Have the steepest downhill tile, flow to it and return (caller will
        biomes.set(RIVER, lowestNei);
        q.push(lowestNei);
        return;
    }
    auto posH = world.get(pos);
    int newFloods[4];       //Array of the directions to be stored
    int numNewFloods = 0;   //Number of directions in newFloods
    for(int dir = UP; dir <= RIGHT; dir++)
    {
        Pos2 nei = getTileInDir(pos, dir);
        if(world.get(nei) == posH && biomes.get(nei) != LAKE)
        {
            newFloods[numNewFloods] = dir;
            numNewFloods++;
        }
    }
    if(numNewFloods != 0)
    {
        for(int i = 0; i < numNewFloods; i++)
        {
            Pos2 proc = getTileInDir(pos, newFloods[i]);
            biomes.set(LAKE, proc);
            q.push(proc);
        }
        return;
    }
    lowestNei = getLowestNeighbor(pos);
    if(lowestNei.x != -1)
    {
        world.set(world.get(lowestNei), pos);
        biomes.set(LAKE, pos);
        //Tile is now as high as at least one of its neighbors, so process it again
        q.push(pos);
    }
}

void Watershed::riverRandomWalk(Pos2 p1, Pos2 p2)
{
    Pos2 iter = p1;
    while(iter != p2)
    {
        int xdiff = abs(p2.x - iter.x);
        int ydiff = abs(p2.y - iter.y);
        int choice = RandomUtils::gen() % (xdiff + ydiff);
        if(xdiff == 0)
        {
            //Finish drawing the route
            for(int i = min(p2.x, iter.x); i <= max(p2.x, iter.x); i++)
            {
                biomes.set(RIVER, i, p2.y);
            }
            return;
        }
        else if(ydiff == 0)
        {
            for(int i = min(p2.y, iter.y); i <= max(p2.y, iter.y); i++)
            {
                biomes.set(RIVER, p2.x, i);
            }
            return;
        }
        if(choice <= xdiff)
        {
            //move in x towards p2
            if(iter.x < p2.x)
                iter.x++;
            else
                iter.x--;
        }
        else
        {
            //move in y towards p2
            if(iter.y < p2.y)
                iter.y++;
            else
                iter.y--;
        }
        biomes.set(RIVER, iter);
    }
}

Pos2 Watershed::getLandTile()
{
    Pos2 rv;
    do
    {
        rv.x = RandomUtils::gen() % WORLD_SIZE;
        rv.y = RandomUtils::gen() % WORLD_SIZE;
    }
    while(world.get(rv) <= 0);
    return rv;
}

Pos2 Watershed::flowDownhill(Pos2 loc)
{
    while(true)
    {
        Pos2 lowNei = getLowestNeighbor(loc);
        if(world.get(lowNei) < world.get(loc))
            loc = lowNei;
        else
            return loc;
    }
}

int Watershed::getNonLakeDownhillDir(Pos2 loc)
{
    int poss[4];
    int iter = 0;
    auto h = world.get(loc);
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
    //cout << "Forming lake at " << loc.x << ", " << loc.y << endl;
    //Expanding boundary approach
    vector<Pos2> boundary;
    vector<Pos2> outlets;
    auto flood = world.get(loc);
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
            //cout << "Finding equal-height tiles" << endl;
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