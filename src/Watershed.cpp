#include "Watershed.h"

using namespace std;
using namespace constants;
using namespace Coord;

void Watershed::addRiver(Height minHeight)
{
    Pos2 loc(0, 0);
    int tries = 0;
    while(World::getHeight(loc) < minHeight || World::getGround(loc) == LAKE || World::getGround(loc) == RIVER || World::getGround(loc) == STONE)
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
        World::setGround(RIVER, loc);
        int downhill = getNonLakeDownhillDir(loc);
        if(downhill != NO_DIRECTION);
        {
            //Check if just flowed into a lake that already flows out to sea
            Pos2 next = getTileInDir(loc, downhill);
            if(World::getGround(next) == LAKE || World::getGround(next) == RIVER)
                return;
        }
        //As long as there is nowhere to flow, form lake
        while(downhill == NO_DIRECTION)
        {
            loc = formLake(loc);
            if(loc.x == -1 || World::getGround(loc) == WATER || World::getHeight(loc) <= 0)
                return;
            
            downhill = getNonLakeDownhillDir(loc);
        }
        loc = getTileInDir(loc, downhill);
        //Decide whether to end
        if(World::getGround(loc) == WATER || World::getHeight(loc) <= 0)
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
    Height bestH = HEIGHT_MAX;
    for(int dir = UP; dir <= RIGHT; dir++)
    {
        Pos2 nei = getTileInDir(pos, dir);
        if(World::getHeight(nei) < bestH)
        {
            bestH = World::getHeight(nei);
            bestPos = nei;
        }
    }
    return bestPos;
}

void Watershed::processTile(Pos2 pos, std::queue<Pos2>& q)
{
    Pos2 lowestNei = getLowestNeighbor(pos);
    if(World::getHeight(lowestNei) <= 0)        //Flowed into ocean, done
        return;
    if(lowestNei.x == -1)
        return;     //should never get here
    if(World::getHeight(lowestNei) < World::getHeight(pos))
    {
        //Have the steepest downhill tile, flow to it and return (caller will
        World::setGround(RIVER, lowestNei);
        q.push(lowestNei);
        return;
    }
    Height posH = World::getHeight(pos);
    int newFloods[4];       //Array of the directions to be stored
    int numNewFloods = 0;   //Number of directions in newFloods
    for(int dir = UP; dir <= RIGHT; dir++)
    {
        Pos2 nei = getTileInDir(pos, dir);
        if(World::getHeight(nei) == posH && World::getGround(nei) != LAKE)
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
            World::setGround(LAKE, proc);
            q.push(proc);
        }
        return;
    }
    lowestNei = getLowestNeighbor(pos);
    if(lowestNei.x != -1)
    {
        World::setHeight(World::getHeight(lowestNei), pos);
        World::setGround(LAKE, pos);
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
                World::setGround(RIVER, i, p2.y);
            }
            return;
        }
        else if(ydiff == 0)
        {
            for(int i = min(p2.y, iter.y); i <= max(p2.y, iter.y); i++)
            {
                World::setGround(RIVER, p2.x, i);
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
        World::setGround(RIVER, iter);
    }
}

void Watershed::dropWater(Pos2 loc)
{
    bool doErode = false;
    //Simulate some water starting at loc, flowing downhill untill it can no longer flow downhill or it reaches the ocean
    while(true)
    {
        Pos2 lowestNei = getLowestNeighbor(loc);
        if(World::getGround(lowestNei) == WATER || World::getHeight(lowestNei) <= 0)
            break;
        if(World::getHeight(lowestNei) < World::getHeight(loc))
        {
            //Simulate the (occasional) carrying of some soil from the uphill position downhill
            if(doErode && RandomUtils::gen() % 2 == 0)
            {
                World::chgHeight(-1, loc);
                World::chgHeight(1, lowestNei);
            }
            loc = lowestNei;
        }
        else if(World::getHeight(lowestNei) == World::getHeight(loc))
        {
            //Height equal
            World::chgHeight(1, loc);
            break;
        }
        else
        {
            World::setHeight(World::getHeight(loc), lowestNei);
            World::setGround(LAKE, lowestNei);
            break;
        }
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
    while(World::getHeight(rv) <= 0);
    return rv;
}

void Watershed::rainForTime(int sec)
{
    clock_t start = clock();
    do
    {
        for(int i = 0; i < 1000; i++)
        {
            Pos2 target;
            do
            {
                target.x = RandomUtils::gen() % WORLD_SIZE;
                target.y = RandomUtils::gen() % WORLD_SIZE;
            }
            while(World::getGround(target) == WATER);
            dropWater(target);
        }
    }
    while((clock() - start) / CLOCKS_PER_SEC < sec);
}

Gradient Watershed::getTileGradient(Pos2 loc)
{
    Gradient grad;
    //dx is change in height with respect to x, or avg height difference between node at x = k+1 and node at x = k
    Height h1 = World::getHeight(loc.x, loc.y);
    Height h2 = World::getHeight(loc.x + 1, loc.y);
    Height h3 = World::getHeight(loc.x + 1, loc.y + 1);
    Height h4 = World::getHeight(loc.x, loc.y + 1);
    grad.dx = ((h2 - h1) + (h3 - h4)) / 2;
    grad.dy = ((h4 - h1) + (h3 - h2)) / 2;
    return grad;
}

void Watershed::doErosion()
{
    Gradient* gradBuf = new Gradient[WORLD_SIZE * WORLD_SIZE];
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            gradBuf[i + j * WORLD_SIZE] = getTileGradient(Pos2(i, j));
        }
    }
    delete[] gradBuf;
}

Pos2 Watershed::flowDownhill(Pos2 loc)
{
    while(true)
    {
        Pos2 lowNei = getLowestNeighbor(loc);
        if(World::getHeight(lowNei) < World::getHeight(loc))
            loc = lowNei;
        else
            return loc;
    }
}

int Watershed::getNonLakeDownhillDir(Pos2 loc)
{
    int poss[4];
    int iter = 0;
    Height h = World::getHeight(loc);
    for(int dir = UP; dir <= RIGHT; dir++)
    {
        Pos2 nei = getTileInDir(loc, dir);
        if(World::getHeight(nei) < h && World::getGround(nei) != LAKE_BOUNDARY)
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

int Watershed::getNonLakeEqualDir(Pos2 loc, Height flood)
{
    for(int dir = UP; dir <= RIGHT; dir++)
    {
        Pos2 nei = getTileInDir(loc, dir);
        if(World::getHeight(nei) == flood && World::getGround(nei) != LAKE_BOUNDARY)
            return dir;
    }
    return NO_DIRECTION;
}

bool Watershed::surroundedByLake(Pos2 loc)
{
    for(int dir = UP; dir <= RIGHT; dir++)
    {
        if(World::getGround(getTileInDir(loc, dir)) != LAKE_BOUNDARY)
            return false;
    }
    return true;
}

bool Watershed::hasLakeNeighbor(Pos2 loc)
{
    for(int dir = UP; dir <= RIGHT; dir++)
    {
        if(World::getGround(getTileInDir(loc, dir)) == LAKE)
            return true;
    }
    return false;
}

bool Watershed::hasDownhillFlow(Pos2 loc)
{
    if(World::getGround(loc) != RIVER)
        return false;
    for(int dir = UP; dir <= RIGHT; dir++)
    {
        Pos2 nei = getTileInDir(loc, dir);
        if(World::getHeight(nei) < World::getHeight(loc) && World::getGround(nei) == RIVER)
           return true;
    }
    return false;
}

bool Watershed::hasOutlet(Pos2 loc)
{
    if(World::getGround(loc) != LAKE)
        return false;
    queue<Pos2> q;
    q.push(loc);
    World::setGround(OUTLET_SEARCHED, loc);
    bool found = false;
    while(q.size() > 0)
    {
        Pos2 proc = q.front();
        q.pop();
        if(World::getGround(proc) == LAKE)
        {
            World::setGround(OUTLET_SEARCHED, proc);
            //Check for river flow downhill
            //if found remember to return true
            //if not found mark as visited and continue iteration
            for(int dir = UP; dir <= RIGHT; dir++)
            {
                Pos2 nei = getTileInDir(proc, dir);
                if(World::getGround(nei) == RIVER && getNonLakeDownhillDir(nei) != NO_DIRECTION)
                    found = true;
                else if(World::getGround(nei) == LAKE)
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
    World::setGround(LAKE, loc);
    while(q.size() > 0)
    {
        Pos2 proc = q.front();
        q.pop();
        if(World::getGround(proc) == OUTLET_SEARCHED)
        {
            World::setGround(LAKE, proc);
            for(int dir = UP; dir <= RIGHT; dir++)
            {
                Pos2 nei = getTileInDir(proc, dir);
                if(World::getGround(proc) == OUTLET_SEARCHED)
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
    Height flood = World::getHeight(loc);
    World::setGround(LAKE_BOUNDARY, loc);
    boundary.push_back(loc);
    int iteration = 0;
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
                    World::setGround(LAKE_BOUNDARY, nei);
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
        flood = HEIGHT_MAX;
        for(Pos2 iter : boundary)
        {
            for(int dir = UP; dir <= RIGHT; dir++)
            {
                Pos2 nei = getTileInDir(iter, dir);
                if(World::getGround(nei) != LAKE_BOUNDARY && World::getHeight(nei) < flood)
                    flood = World::getHeight(nei);
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
        if(World::getGround(proc) == LAKE_BOUNDARY || World::getHeight(proc) != flood)
        {
            World::setHeight(flood, proc);
            World::setGround(LAKE, proc);
            //If proc has a neighbor that is a river that flows downhill...
            for(int dir = UP; dir <= RIGHT; dir++)
            {
                Pos2 nei = getTileInDir(proc, dir);
                if(World::getGround(nei) == LAKE_BOUNDARY)
                    fillq.push(nei);
            }
        }
    }
    if(alreadyHasOutlet)    //Don't continue flow
        return {-1, -1};
    else
        return outlets[RandomUtils::gen() % outlets.size()];
}