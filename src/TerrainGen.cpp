#include "TerrainGen.h"

using namespace std;
using namespace constants;
using namespace Coord;

void TerrainGen::generate()
{
    clock_t start = clock();
    RandomUtils::seed(time(NULL));
    clearAll(); //prevent invalid height/ground in world
    defaultGen();
    cout << "Generation took " << (double(clock()) - start) / CLOCKS_PER_SEC << " seconds." << endl;
}

void TerrainGen::defaultGen()
{
    diamondSquare();
    scatterCentralVolcanoes();
    shelfMask();
    clampSeaLevel();
    //stretchToFill();
    smooth();
    for(int i = 0; i < 20; i++)
    {
        //addRiver();
    }
    defuzz();
    //consolidateLakes();
    flattenWater();
}

void TerrainGen::tester()
{
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            World::setHeight(2 * (WORLD_SIZE / 2 - max(abs(i - WORLD_SIZE / 2), abs(j - WORLD_SIZE / 2))), i, j);
            World::setGround(DESERT, i, j);
        }
    }
}

//Generates height based on average surrounding height, and how fine grid is
Height TerrainGen::getHeight(int avg, int size)
{
    int range = double(size) * ROUGHNESS;
    if(range == 0)
        return avg;
    range = range > HEIGHT_MAX ? HEIGHT_MAX : range;
    int result = (avg + (RandomUtils::gen() % range) - (range / 2));
    return result;
}

void TerrainGen::diamondSquare()
{
    const Height seedH = 100;
    //World corners
    World::setHeight(10, 0, 0);
    World::setHeight(10, WORLD_SIZE - 1, 0);
    World::setHeight(10, 0, WORLD_SIZE - 1);
    World::setHeight(10, WORLD_SIZE - 1, WORLD_SIZE - 1);
    //World edge midpoints
    World::setHeight(10, WORLD_SIZE / 2, 0);
    World::setHeight(10, 0, WORLD_SIZE);
    World::setHeight(10, WORLD_SIZE / 2, WORLD_SIZE - 1);
    World::setHeight(10, WORLD_SIZE - 1, WORLD_SIZE / 2);
    //Center tile
    World::setHeight(seedH, WORLD_SIZE / 2, WORLD_SIZE / 2);
    //repeatedly go over mesh and do these steps, making grid progressively finer
    //(squares first, then diamonds)
    //Note: if squares then diamonds, same size can be used each iteration
    int size = WORLD_SIZE / 2;
    while(size > 0)
    {
        //Fill in the diamonds
        //Iterate over all grid points, check if needs a diamond fill-in
        for(int i = 0; i < WORLD_SIZE; i += size)
        {
            for(int j = 0; j < WORLD_SIZE; j += size)
            {
                if((i + j) % (size * 2) != 0)
                {
                    fillDiamond(i, j, size * 2);
                }
            }
        }
        for(int i = size / 2; i < WORLD_SIZE; i += size)
        {
            for(int j = size / 2; j < WORLD_SIZE; j += size)
            {
                fillSquare(i, j, size);
            }
        }
        size /= 2;
    }
}

bool TerrainGen::inMesh(int x, int y)
{
    if(x > 0 && x < WORLD_SIZE && y > 0 && y < WORLD_SIZE)
        return true;
    else
        return false;
}

void TerrainGen::fillDiamond(int x, int y, int size)
{
    //place point at center of a diamond (square corner)
    //size = total width/height of the diamond being filled in
    int sum = 0;
    int n = 0;
    //cout << "Filling in diamond @ " << x << "," << y << " with size " << size << endl;
    if(inMesh(x - size / 2, y))
    {
        n++;
        sum += World::getHeight(x - size / 2, y);
    }
    if(inMesh(x + size / 2, y))
    {
        n++;
        sum += World::getHeight(x + size / 2, y);
    }
    if(inMesh(x, y - size / 2))
    {
        n++;
        sum += World::getHeight(x, y - size / 2);
    }
    if(inMesh(x, y + size / 2))
    {
        n++;
        sum += World::getHeight(x, y + size / 2);
    }
    if(n == 0)
        return;
    World::setHeight(getHeight(sum / n, size), x, y);
    World::setGround(DESERT, x, y);
}

void TerrainGen::fillSquare(int x, int y, int size)
{
    int sum = 0;
    int n = 0;
    //cout << "Filling in square @ " << x << "," << y << " with size " << size << endl;
    //sometimes (x,y) is right on boundary of chunk so we have to check this
    //size is the side length of square being filled in
    if(inMesh(x - size / 2, y - size / 2))
    {
        n++;
        sum += World::getHeight(x - size / 2, y - size / 2);
    }
    if(inMesh(x - size / 2, y + size / 2))
    {
        n++;
        sum += World::getHeight(x - size / 2, y + size / 2);
    }
    if(inMesh(x + size / 2, y + size / 2))
    {
        n++;
        sum += World::getHeight(x + size / 2, y + size / 2);
    }
    if(inMesh(x + size / 2, y - size / 2))
    {
        n++;
        sum += World::getHeight(x + size / 2, y - size / 2);
    }
    //Put in height for center of square
    if(n == 0)
        return;
    World::setHeight(getHeight(sum / n, size), x, y);
    World::setGround(DESERT, x, y);
}

void TerrainGen::defuzz()
{
    for(short i = 0; i < WORLD_SIZE; i++)
    {
        for(short j = 0; j < WORLD_SIZE; j++)
        {
            if(World::getGround(i, j) != WATER)
            {
                int waterNeighbors = 0;
                for(int k = UP; k <= RIGHT; k++)
                {
                    if(World::getGround(getTileInDir({i, j}, k)) == WATER)
                        waterNeighbors++;
                }
                if(waterNeighbors >= 3)
                {
                    World::setGround(WATER, i, j);
                }
            }
            else
            {
                int waterNeighbors = 0;
                for(int k = UP; k <= RIGHT; k++)
                {
                    if(World::getGround(getTileInDir({i, j}, k)) != WATER)
                        waterNeighbors++;
                }
                if(waterNeighbors >= 3)
                {
                    World::setHeight(1, i, j);
                    World::setGround(DESERT, i, j);
                }
            }
        }
    }
}

void TerrainGen::flattenWater()
{
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            if(World::getGround(i, j) == WATER)
            {
                Height minHeight = World::getHeight(i, j);
                bool needsUpdate = false;
                if(World::getHeight(i + 1, j) < minHeight)
                {
                    minHeight = World::getHeight(i + 1, j);
                    needsUpdate = true;
                }
                if(World::getHeight(i + 1, j + 1) < minHeight)
                {
                    minHeight = World::getHeight(i + 1, j + 1);
                    needsUpdate = true;
                }
                if(World::getHeight(i, j + 1) < minHeight)
                {
                    minHeight = World::getHeight(i, j + 1);
                    needsUpdate = true;
                }
                if(needsUpdate)
                {
                    World::setHeight(minHeight, i, j);
                    World::setHeight(minHeight, i + 1, j);
                    World::setHeight(minHeight, i + 1, j + 1);
                    World::setHeight(minHeight, i, j + 1);
                }
            }
        }
    }
}

void TerrainGen::addVolcano(int x, int y, short height, int radius)
{
    DBASSERT(radius > 0);
    int xmin = x - radius;
    int xmax = x + radius;
    int ymin = y - radius;
    int ymax = y + radius;
    //cone height is height / (float(dist((i,j) <-> (x,y))) / radius). Or 0 if this would be <0.
    for(int i = xmin; i <= xmax; i++)
    {
        for(int j = ymin; j < ymax; j++)
        {
            float dist = sqrt((i - x) * (i - x) + (j - y) * (j - y));
            float rad = radius;
            if(dist < rad)
            {
                int change = height - (float(height) * (dist / rad));
                if(World::tileInWorld(i, j))
                {
                    World::setHeight(World::getHeight(i, j) + change, i, j);
                    World::setGround(DESERT, i, j);
                }
            }
        }
    }
}

void TerrainGen::clearAll()
{
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            World::setGround(WATER, i, j);
            World::setHeight(0, i, j);
        }
    }
}

void TerrainGen::scatterVolcanos()
{
    for(int i = 0; i < 20; i++)
    {
        int x = RandomUtils::gen() % WORLD_SIZE;
        int y = RandomUtils::gen() % WORLD_SIZE;
        int height = 100 + RandomUtils::gen() % 500;
        float heightMult = TERRAIN_TILE_SIZE / TERRAIN_Y_SCALE;
        int radius = (height * (RandomUtils::gen() % 5 + 5)) / heightMult;
        addVolcano(x, y, height, radius);
    }
}

void TerrainGen::pyramidMask()
{
    Height reduction = -HEIGHT_MAX;
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        if(World::getHeight(i, 0) > reduction)
            reduction = World::getHeight(i, 0);
        if(World::getHeight(0, i) > reduction)
            reduction = World::getHeight(0, i);
        if(World::getHeight(i, WORLD_SIZE - 1) > reduction)
            reduction = World::getHeight(i, WORLD_SIZE - 1);
        if(World::getHeight(WORLD_SIZE - 1, i) > reduction)
            reduction = World::getHeight(WORLD_SIZE - 1, i);
    }
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            double dist = double(max(abs(i - WORLD_SIZE / 2), abs(j - WORLD_SIZE / 2))) / (WORLD_SIZE / 2);
            World::setHeight(World::getHeight(i, j) - dist * reduction, i, j);
            //World::setHeight(World::getHeight(i, j) - reduction, i, j);
        }
    }
}

void TerrainGen::clampSeaLevel()
{
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            if(World::getHeight(i, j) < 0)
            {
                World::setGround(WATER, i, j);
                World::setHeight(0, i, j);
            }
        }
    }
}

void TerrainGen::erode(int numTimesteps)
{
    
}

void TerrainGen::addRiver()
{
    //Let the headwater tile be a random tile above a certain height
    //ideally pick so only a handful of random samples are typically needed,
    //but is still at high enough altitude to make interesting long rivers
    const Height minSpringAlt = 200;
    Pos2 tilePos;
    do
    {
        tilePos.x = RandomUtils::gen() % WORLD_SIZE;
        tilePos.y = RandomUtils::gen() % WORLD_SIZE;
    }
    while(World::getHeight(tilePos.x, tilePos.y) < minSpringAlt);
    World::setGround(RIVER, tilePos.x, tilePos.y);
    flowRiverFromPoint(tilePos);
}

int getDownhill(Pos2 loc)
{
    int cand[4];
    int iter = 0;
    for(int dir = 0; dir <= RIGHT; dir++)
    {
        Pos2 nei = getTileInDir(loc, dir);
        if(World::getHeight(nei) < World::getHeight(loc))
        {
            if(World::getGround(nei) == LAKE || World::getGround(nei) == RIVER)
                return dir;
            cand[iter] = dir;
            iter++;
        }
    }
    if(iter == 0)
        return NO_DIRECTION;
    return cand[RandomUtils::gen() % iter];
}

void TerrainGen::flowRiverFromPoint(Pos2 loc)
{
    while(true)
    {
        int dir = getDownhill(loc);
        if(dir == NO_DIRECTION)
        {
            loc = formLake(loc);
            if(loc.x == -1 && loc.y == -1)
                return;
            World::setGround(RIVER, loc);
        }
        else
        {
            loc = getTileInDir(loc, dir);
            /*
            if(World::getGround(loc) == LAKE || World::getGround(loc) == RIVER)
                return;
             */
            World::setGround(RIVER, loc);
        }
        if(World::getHeight(loc) <= 0)
            break;
    }
}

void fillToHeight(Pos2 pos, Height flood)
{
    if(World::getGround(pos) != LAKE)
    {
        if(World::getHeight(pos) <= flood)
        {
            World::setGround(LAKE, pos);
            World::setHeight(flood, pos);
            for(int dir = UP; dir <= RIGHT; dir++)
            {
                fillToHeight(pos, flood);
            }
        }
    }
}

bool findOutlet(Pos2 lastRiver, Pos2& result)
{
    Height h = World::getHeight(lastRiver);
    queue<Pos2> q;
    q.push(lastRiver);
    vector<Pos2> outlets;
    while(q.size() > 0)
    {
        Pos2 proc = q.front();
        q.pop();
        if(World::getGround(proc) != OUTLET_SEARCHED)
        {
            World::setGround(OUTLET_SEARCHED, proc);
            if(World::getHeight(proc) < h)
                outlets.push_back(proc);
            else if(World::getHeight(proc) == h) //only chain new calls if proc is already part of lake
            {
                for(int dir = UP; dir <= RIGHT; dir++)
                {
                    Pos2 nei = getTileInDir(proc, dir);
                    if(World::getGround(nei) != OUTLET_SEARCHED)
                        q.push(nei);
                }
            }
        }
    }
    if(outlets.size() == 0)
        return false;
    result = outlets[RandomUtils::gen() % outlets.size()];
    return true;
}

Height getFloodHeight(Pos2 lastRiver)
{
    Height oldFlood = World::getHeight(lastRiver);
    Height newFlood = HEIGHT_MAX;
    queue<Pos2> q;
    q.push(lastRiver);
    while(q.size() > 0)
    {
        Pos2 proc = q.front();
        q.pop();
        if(World::getGround(proc) != FLOODING)
        {
            World::setGround(FLOODING, proc); //mark as visited by fill
            Height procH = World::getHeight(proc);
            if(procH > oldFlood && procH < newFlood)
                newFlood = procH;
            if(procH == oldFlood)
            {
                for(int dir = UP; dir <= RIGHT; dir++)
                {
                    Pos2 nei = getTileInDir(proc, dir);
                    if(World::getGround(nei) != FLOODING)
                        q.push(nei);
                }
            }
        }
    }
    return newFlood;
}

void floodToHeight(Pos2 lastRiver, Height flood)
{
    queue<Pos2> q;
    q.push(lastRiver);
    while(q.size() > 0)
    {
        Pos2 proc = q.front();
        q.pop();
        if(World::getGround(proc) != LAKE)
        {
            World::setGround(LAKE, proc); //mark as visited by fill
            World::setHeight(flood, proc);
            for(int dir = UP; dir <= RIGHT; dir++)
            {
                Pos2 nei = getTileInDir(proc, dir);
                if((World::getGround(nei) == FLOODING || World::getGround(nei) == OUTLET_SEARCHED) && World::getHeight(nei) <= flood)
                    q.push(nei);
            }
        }
    }
}

Pos2 TerrainGen::formLake(Pos2 lastRiver)
{
    Pos2 outlet;
    Height flood = World::getHeight(lastRiver);
    floodToHeight(lastRiver, flood);
    if(findOutlet(lastRiver, outlet))
    {
        floodToHeight(lastRiver, flood);
        return outlet;
    }
    for(int iter = 0; true; iter++)
    {
        flood = getFloodHeight(lastRiver);
        floodToHeight(lastRiver, flood);
        if(findOutlet(lastRiver, outlet))
            break;
    }
    floodToHeight(lastRiver, flood);
    return outlet;
}

Height TerrainGen::maxHeightOfTile(Pos2 loc)
{
    Height h = World::getHeight(loc.x, loc.y);
    h = max(h, World::getHeight(loc.x + 1, loc.y));
    h = max(h, World::getHeight(loc.x + 1, loc.y + 1));
    h = max(h, World::getHeight(loc.x, loc.y + 1));
    return h;
}

int TerrainGen::getNonLakeDownhill(Pos2 loc)
{
    Height cutoff = World::getHeight(loc);
    Direction cand[4];
    int iter = 0;
    for(int i = UP; i <= RIGHT; i++)
    {
        Pos2 search = getTileInDir(loc, i);
        if(World::getGround(search) != FLOODING && World::getGround(search) != LAKE && World::getHeight(search) < cutoff)
        {
            cand[iter] = (Direction) i;
            iter++;
        }
    }
    if(iter == 0)
        return NO_DIRECTION;
    int result = RandomUtils::gen() % iter;
    return cand[result];
}

void TerrainGen::consolidateLakes()
{
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            if(World::getGround(i, j) == OUTLET_SEARCHED)
                World::setGround(LAKE, i, j);
        }
    }
}

void TerrainGen::fillRiverGaps()
{
    Pos2 loc;
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        loc.x = i;
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            loc.y = j;
            GROUND g = World::getGround(loc);
            if(g != LAKE && g != RIVER)
            {
                int lakeNeighbors = 0;
                int riverNeighbors = 0;
                for(int dir = UP; dir <= RIGHT; dir++)
                {
                    GROUND ng = World::getGround(getTileInDir(loc, dir));
                    if(ng == RIVER)
                        riverNeighbors++;
                    else if(ng == LAKE)
                        lakeNeighbors++;
                }
                if(lakeNeighbors > 0 && riverNeighbors > 0)
                    World::setGround(RIVER, loc);
                else if(lakeNeighbors >= 3)
                    World::setGround(LAKE, loc);
            }
        }
    }
}

void TerrainGen::shelfMask()
{
    Height slopeDrop = World::getHeight(0, 0);
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        if(World::getHeight(0, i) > slopeDrop)
            slopeDrop = World::getHeight(0, i);
        if(World::getHeight(i, 0) > slopeDrop)
            slopeDrop = World::getHeight(i, 0);
        if(World::getHeight(WORLD_SIZE - 1, i) > slopeDrop)
            slopeDrop = World::getHeight(WORLD_SIZE - 1, i);
        if(World::getHeight(i, WORLD_SIZE - 1) > slopeDrop)
            slopeDrop = World::getHeight(i, WORLD_SIZE - 1);
    }
    //slopeDrop += 3;
    const int slopeLen = 400;
    const float slope = float(slopeDrop) / slopeLen;
    for(int i = 0; i < WORLD_SIZE - 1; i++) // i = x for top bevel
    {
        for(int j = 0; j < slopeLen; j++) //j = y for top bevel
        {
            if(i >= j && i < WORLD_SIZE - j)
            {
                //i, j gives point in the upper bevel edge
                Height drop = (slopeLen - j) * slope;
                World::chgHeight(-drop, i, j);
                World::chgHeight(-drop, WORLD_SIZE - j, i);
                World::chgHeight(-drop, WORLD_SIZE - i, WORLD_SIZE - j);
                World::chgHeight(-drop, j, WORLD_SIZE - i);
                if(j == slopeLen - 1)
                {
                    World::setGround(MOUNTAINS, i, j);
                    World::setGround(MOUNTAINS, WORLD_SIZE - j, i);
                    World::setGround(MOUNTAINS, WORLD_SIZE - i, WORLD_SIZE - j);
                    World::setGround(MOUNTAINS, j, WORLD_SIZE - i);
                }
            }
        }
    }
}

void TerrainGen::smooth(int iters)
{
    Height* buf = new Height[WORLD_SIZE * WORLD_SIZE];
    for(int it = 0; it < iters; it++)
    {
        for(int i = 1; i < WORLD_SIZE - 1; i++)
        {
            for(int j = 1; j < WORLD_SIZE - 1; j++)
            {
                int sum = 0;
                sum += World::getHeight(i, j);
                sum += World::getHeight(i + 1, j);
                sum += World::getHeight(i - 1, j);
                sum += World::getHeight(i, j + 1);
                sum += World::getHeight(i, j - 1);
                buf[i * WORLD_SIZE + j] = sum / 5;
            }
        }
        for(int i = 1; i < WORLD_SIZE - 1; i++)
        {
            for(int j = 1; j < WORLD_SIZE - 1; j++)
            {
                World::setHeight(buf[i * WORLD_SIZE + j], i, j);
            }
        }
    }
    delete[] buf;
}

void TerrainGen::scatterCentralVolcanoes()
{
    const int numVolcanoes = 8;
    const int minEdgeDist = 300;
    const int minDiam = 300;
    const int maxDiam = 600;
    const float minSlope = 0.5;
    const float maxSlope = 0.8;
    for(int i = 0; i < numVolcanoes; i++)
    {
        int x = RandomUtils::gen() % (WORLD_SIZE - minEdgeDist * 2) + minEdgeDist;
        int y = RandomUtils::gen() % (WORLD_SIZE - minEdgeDist * 2) + minEdgeDist;
        int diam = minDiam + RandomUtils::gen() % (maxDiam - minDiam);
        float slope = minSlope + (RandomUtils::gen() % 1000) * (maxSlope - minSlope) / 1000;
        addVolcano(x, y, diam * slope / 2, diam / 2);
        for(int i = -5; i < 5; i++)
        {
            for(int j = -5; j < 5; j++)
            {
                World::setGround(MOUNTAINS, i + x, j + y);
            }
        }
    }
}

void TerrainGen::addEntropy()
{
    for(int i = 0; i < WORLD_SIZE * WORLD_SIZE / 100; i++)
    {
        World::chgHeight(-2, RandomUtils::gen() % WORLD_SIZE, RandomUtils::gen() % WORLD_SIZE);
    }
}

void TerrainGen::verticalNormalize()
{
    Height reduction = World::getHeight(0, 0);
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        if(World::getHeight(0, i) > reduction)
            reduction = World::getHeight(0, i);
        if(World::getHeight(i, 0) > reduction)
            reduction = World::getHeight(i, 0);
        if(World::getHeight(WORLD_SIZE - 1, i) > reduction)
            reduction = World::getHeight(WORLD_SIZE - 1, i);
        if(World::getHeight(i, WORLD_SIZE - 1) > reduction)
            reduction = World::getHeight(i, WORLD_SIZE - 1);
    }
    reduction += 3;
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            World::chgHeight(-reduction, i, j);
        }
    }
}

void TerrainGen::stretchToFill()
{
    int minx = WORLD_SIZE / 2;
    int maxx = minx;
    int miny = minx;
    int maxy = minx;
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            if(World::getGround(i, j) != WATER)
            {
                if(i < minx)
                    minx = i;
                if(i > maxx)
                    maxx = i;
                if(j < miny)
                    miny = j;
                if(j > maxy)
                    maxy = j;
            }
        }
    }
    maxx++; //account for the visible south and east edges of tiles being
    maxy++; //one greater in x and y than the tile's coords
    minx -= 10; //give 10 tiles of space on all sides
    maxx += 10;
    miny -= 10;
    maxy += 10;
    if(minx < 0)
        minx = 0;
    if(maxx >= WORLD_SIZE)
        maxx = WORLD_SIZE - 1;
    if(miny < 0)
        miny = 0;
    if(maxy >= WORLD_SIZE)
        maxy = WORLD_SIZE - 1;
    Height* buf = new Height[WORLD_SIZE * WORLD_SIZE];
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            buf[WORLD_SIZE * i + j] = World::getHeight(minx + float(i) / WORLD_SIZE * (maxx - minx), miny + float(j) / WORLD_SIZE * (maxy - miny));
        }
    }
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            World::setHeight(buf[WORLD_SIZE * i + j], i, j);
            if(World::getHeight(i, j) <= 0)
                World::setGround(WATER, i, j);
            else
                World::setGround(DESERT, i, j);
        }
    }
    delete[] buf;
}