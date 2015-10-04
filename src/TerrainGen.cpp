#include "TerrainGen.h"

using namespace std;
using namespace constants;

void TerrainGen::generate()
{
    clock_t start = clock();
    defaultGen();
    cout << "Generation took " << (double(clock()) - start) / CLOCKS_PER_SEC << " seconds." << endl;
}

void TerrainGen::defaultGen()
{
    clearAll();
    diamondSquare();
    scatterVolcanos();
    pyramidMask();
    clampSeaLevel();
    for(int i = 0; i < 30; i++)
    {
        addRiver();
    }
    consolidateLakes();
    defuzz();
    defuzz();
    flattenWater();
}

void TerrainGen::tester()
{
    clearAll();
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            int height = 2000 - (abs(i - WORLD_SIZE / 2) + abs(j - WORLD_SIZE / 2));
            if(height > 0)
            {
                World::setHeight(height, i, j);
                World::setGround(DESERT, i, j);
            }
        }
    }
    for(int i = 0; i < 400; i++)
    {
        int volcanoSize = RandomUtils::gen() % 40 + 20;
        addVolcano(150 + RandomUtils::gen() % (WORLD_SIZE - 150), 150 + RandomUtils::gen() % (WORLD_SIZE - 150), -volcanoSize * 3, volcanoSize);
    }
    clampSeaLevel();
    for(int i = 0; i < 50; i++)
        addRiver();
}

//Generates height based on average surrounding height, and how fine grid is
Height TerrainGen::getHeight(int avg, int size)
{
    int range = double(size) / (WORLD_SIZE - 1) * 500 * ROUGHNESS;
    if(range == 0)
        return avg;
    range = range > 255 ? 255 : range;
    int result = (avg + (random() % range) - (range / 2));
    if(result < 0)
        result = 0;
    if(result > 255)
        result = 255;
    //cout << "Height is " << result << " for size " << size << " and avg " << avg << " and range " << range << endl;
    return result;
}

void TerrainGen::diamondSquare()
{
    World::setHeight(50, 0, 0);
    World::setHeight(50, WORLD_SIZE - 1, 0);
    World::setHeight(50, 0, WORLD_SIZE - 1);
    World::setHeight(50, WORLD_SIZE - 1, WORLD_SIZE - 1);
    //First diamond (puts pt at very center of chunk)
    World::setHeight(50000, WORLD_SIZE / 2, WORLD_SIZE / 2);
    //repeatedly go over mesh and do these steps, making grid progressively finer
    //(squares first, then diamonds)
    //Note: if squares then diamonds, same size can be used each iteration
    int size = WORLD_SIZE;
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
            if(World::getGround(i, j) == WATER || World::getGround(i, j) == LAKE)
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
                    World::setHeight(World::getHeight(i, j) + change, i, j);
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
    for(int i = 0; i < 25; i++)
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
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            int height = World::getHeight(i, j);
            int centerDist = abs(WORLD_SIZE / 2 - i) + abs(WORLD_SIZE / 2 - j);
            height -= centerDist / 5;
            if(height < 0)
            {
                height = 0;
                World::setGround(WATER, i, j);
            }
            World::setHeight(height, i, j);
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
    const Height minSpringAlt = 400;
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
        Pos2 nei = TerrainGen::getTileInDir(loc, dir);
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
            if(World::getGround(loc) == LAKE || World::getGround(loc) == RIVER)
                return;
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
                    Pos2 nei = TerrainGen::getTileInDir(proc, dir);
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
                    Pos2 nei = TerrainGen::getTileInDir(proc, dir);
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
                Pos2 nei = TerrainGen::getTileInDir(proc, dir);
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
        return outlet;
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

Pos2 TerrainGen::getTileInDir(Pos2 loc, int dir)
{
    switch(dir)
    {
        case UP:
            loc.y--;
            return loc;
        case DOWN:
            loc.y++;
            return loc;
        case LEFT:
            loc.x--;
            return loc;
        case RIGHT:
            loc.x++;
            return loc;
        default:
            return {-1, -1};
    }
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