#include "TerrainGen.h"

using namespace std;
using namespace constants;

void TerrainGen::generate()
{
    clearAll();
    diamondSquare();
    scatterVolcanos();
    pyramidMask();
    erode(2);
    for(int i = 0; i < 2; i++)
        addRiver();
    clampSeaLevel();
    defuzz();
    defuzz(); //two passes seems optimal (leaves a handful of tiles, but acceptable)
    flattenWater();
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
    {
        return true;
    }
    else
    {
        return false;
    }
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
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            if(World::getGround(i, j) != WATER)
            {
                int waterNeighbors = 0;
                if(World::getGround(i + 1, j) == WATER)
                    waterNeighbors++;
                if(World::getGround(i - 1, j) == WATER)
                    waterNeighbors++;
                if(World::getGround(i, j + 1) == WATER)
                    waterNeighbors++;
                if(World::getGround(i, j - 1) == WATER)
                    waterNeighbors++;
                if(waterNeighbors >= 3)
                {
                    World::setGround(WATER, i, j);
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
            int change = height - (float(height) * (dist / rad));
            if(change > 0)
            {
                if(World::tileInWorld(i, j))
                {
                    World::setHeight(World::getHeight(i, j) + change, i, j);
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
#define heightInSed(i, j) (sed[(i) * WORLD_SIZE + (j)])
    Height* sed = new Height[WORLD_SIZE * WORLD_SIZE]; //sed holds net height change at each corner. Distribution of sediment based on x and z components of downhill vector at the corner.
    for(int step = 0; step < numTimesteps; step++)
    {
        for(int i = 0; i < WORLD_SIZE; i++)
        {
            for(int j = 0; j < WORLD_SIZE; j++)
            {
                //Zero out sed
                heightInSed(i, j) = 0;
            }
        }
        for(int i = 0; i < WORLD_SIZE; i++)
        {
            for(int j = 0; j < WORLD_SIZE; j++)
            {
                float downX = World::getHeight(i + 1, j) - World::getHeight(i - 1, j);
                float downY = World::getHeight(i, j + 1) - World::getHeight(i, j - 1);
                Height loss = sqrt(downX * downX + downY * downY) * 0.2; //todo: decide scaling factor
                if(loss == 0)
                    continue;
                float angle;
                if(downX == 0)
                {
                    if(downY < 0)
                        angle = -M_PI_2;
                    else
                        angle = M_PI_2;
                }
                else
                    angle = atan(downY / downX);
                //cout << angle << endl;
                sed[i * WORLD_SIZE + j] -= loss;
                //Decide where that sediment will flow
                int segment = (angle - M_PI / 8) / (M_PI / 4);
                switch(segment)
                {
                    case 0:
                        heightInSed(i + 1, j + 1) += loss;
                        break;
                    case 1:
                        heightInSed(i, j + 1) += loss;
                        break;
                    case 2:
                        heightInSed(i - 1, j + 1) += loss;
                        break;
                    case 3:
                        heightInSed(i - 1, j) += loss;
                        break;
                    case 4:
                        heightInSed(i - 1, j - 1) += loss;
                        break;
                    case 5:
                        heightInSed(i, j - 1) += loss;
                        break;
                    case 6:
                        heightInSed(i + 1, j - 1) += loss;
                        break;
                    case 7:
                        heightInSed(i + 1, j) += loss;
                        break;
                }
            }
        }
        //apply sed changes to world
        for(int i = 0; i < WORLD_SIZE; i++)
        {
            for(int j = 0; j < WORLD_SIZE; j++)
            {
                World::setHeight(World::getHeight(i, j) + heightInSed(i, j), i, j);
            }
        }
    }
    delete[] sed;
#undef heightInSed
}

void TerrainGen::addRiver()
{
    cout << "Starting river generation." << endl;
    //Let the headwater tile be a random tile above a certain height
    //ideally pick so only a handful of random samples are typically needed
    const Height minSpringAlt = 200;
    Pos2 tilePos;
    do
    {
        tilePos.x = RandomUtils::gen() % WORLD_SIZE;
        tilePos.y = RandomUtils::gen() % WORLD_SIZE;
    }
    while(World::getHeight(tilePos.x, tilePos.y) < minSpringAlt);
    PRINT("Starting a river at " << tilePos.x << ", " << tilePos.y)
    World::setGround(RIVER, tilePos.x, tilePos.y);
    flowRiverFromPoint(tilePos);
}

void TerrainGen::flowRiverFromPoint(Pos2 loc)
{
    //now work downhill, forming river's path
    int dir;
    const int xDir = 0;
    const int yDir = 1;
    while(true)
    {
        Height tileH = World::getHeight(loc.x, loc.y);
        Height x1 = World::getHeight(loc.x + 1, loc.y);
        Height x2 = World::getHeight(loc.x - 1, loc.y);
        Height y1 = World::getHeight(loc.x, loc.y + 1);
        Height y2 = World::getHeight(loc.x, loc.y - 1);
        int xSlope = x1 - tileH;
        if(abs(xSlope) < (tileH - x2))
            xSlope = tileH - x2;
        int ySlope = y1 - tileH;
        if(abs(ySlope) < (tileH - y2))
            ySlope = tileH - y2;
        //Decide if river should end (true if can't go downhill in any direction)
        if(x1 >= tileH && x2 >= tileH && y1 >= tileH && y2 >= tileH)
        {
            loc = formLake(loc);
            //reassign local variables and continue
            tileH = World::getHeight(loc);
            x1 = World::getHeight(loc.x + 1, loc.y);
            x2 = World::getHeight(loc.x - 1, loc.y);
            y1 = World::getHeight(loc.x, loc.y + 1);
            y2 = World::getHeight(loc.x, loc.y - 1);
            xSlope = x1 - tileH;
            if(abs(xSlope) < (tileH - x2))
                xSlope = tileH - x2;
            ySlope = y1 - tileH;
            if(abs(ySlope) < (tileH - y2))
                ySlope = tileH - y2;
        }
        //Now choose direction of flow
        if(abs(xSlope) == abs(ySlope))     //there is a tie, pick x or y randomly
            dir = (RandomUtils::gen() >> 4) & 1;
        else if(abs(xSlope) < abs(ySlope)) //steeper in y
            dir = yDir;
        else
            dir = xDir;                    //steeper in x
        if(dir == xDir)
        {
            if(xSlope < 0)
                loc.x++;
            else
                loc.x--;
        }
        else
        {
            if(ySlope < 0)
                loc.y++;
            else
                loc.y--;
        }
        if(World::getGround(loc) == RIVER) //fed into another river, stop
            break;
        World::setGround(RIVER, loc);
        if(World::getHeight(loc) == 0)     //flowed to ocean, stop
            break;
    }
}

Pos2 TerrainGen::formLake(Pos2 lastRiver)
{
    cout << "Forming lake at " << lastRiver.x << ", " << lastRiver.y << endl;
    //find min of neighboring heights, and flood tile(s) with that height
    //this function is called when lastRiver tile is in a pit, and a lake needs to be created there so it spills into the next leg which can flow downhill
    list<Pos2> boundaryTiles = {lastRiver};
    World::setGround(LAKE, lastRiver);
    Pos2 outletTile = {-1, -1};
    Height nextFillHeight;
    while(true)
    {
        cout << "Starting new boundary tile iteration.";
        cout << "Currently have " << boundaryTiles.size() << endl;
        nextFillHeight = 32767;
        for(auto it = boundaryTiles.begin(); it != boundaryTiles.end();)
        {
            if(surroundedByLake(*it))
            {
                it = boundaryTiles.erase(it);
                continue;
            }
            Pos2& bound = *it;
            //left, right, up, down
            if(World::getGround(bound.x - 1, bound.y) != LAKE)
                nextFillHeight = min(nextFillHeight, World::getHeight(bound.x - 1, bound.y));
            if(World::getGround(bound.x, bound.y - 1) != LAKE)
                nextFillHeight = min(nextFillHeight, World::getHeight(bound.x, bound.y - 1));
            if(World::getGround(bound.x + 1, bound.y) != LAKE)
                nextFillHeight = min(nextFillHeight, World::getHeight(bound.x + 1, bound.y));
            if(World::getGround(bound.x, bound.y + 1) != LAKE)
                nextFillHeight = min(nextFillHeight, World::getHeight(bound.x, bound.y + 1));
            it++;
        }
        cout << "Flooding tiles up to and including " << nextFillHeight << endl;
        //now have next level to flood to
        int A = 0;
        for(auto bound = boundaryTiles.begin(); bound != boundaryTiles.end(); bound++)
        {
            cout << "Processing boundary tile " << A << " for next filll height." << endl;
            cout << "Tile is at " << bound->x << ", " << bound->y << endl;
            cout << "Size of boundary tile list is " << boundaryTiles.size() << endl << endl;
            A++;
            if(processLakeBoundaryTile(boundaryTiles, *bound, {short(bound->x - 1), bound->y}, nextFillHeight))
                outletTile = {short(bound->x - 1), bound->y};
            if(processLakeBoundaryTile(boundaryTiles, *bound, {bound->x, short(bound->y - 1)}, nextFillHeight))
                outletTile = {bound->x, short(bound->y - 1)};
            if(processLakeBoundaryTile(boundaryTiles, *bound, {short(bound->x + 1), bound->y}, nextFillHeight))
                outletTile = {short(bound->x + 1), bound->y};
            if(processLakeBoundaryTile(boundaryTiles, *bound, {bound->x, short(bound->y + 1)}, nextFillHeight))
                outletTile = {bound->x, short(bound->y + 1)};
        }
        if(outletTile.x != -1)
            break;
    }
    World::setHeight(nextFillHeight, lastRiver);
    recurseSetLakeHeight(nextFillHeight, lastRiver);
    return outletTile;
}

bool TerrainGen::hasNonLakeDownhill(Pos2 loc)
{
    Height locH = World::getHeight(loc);
    if(World::getHeight(loc.x - 1, loc.y) < locH && World::getGround(loc.x - 1, loc.y) != LAKE)
        return true;
    if(World::getHeight(loc.x + 1, loc.y) < locH && World::getGround(loc.x + 1, loc.y) != LAKE)
        return true;
    if(World::getHeight(loc.x, loc.y - 1) < locH && World::getGround(loc.x, loc.y - 1) != LAKE)
        return true;
    if(World::getHeight(loc.x, loc.y + 1) < locH && World::getHeight(loc.x, loc.y + 1) != LAKE)
        return true;
    return false;
}

Height TerrainGen::maxHeightOfTile(Pos2 loc)
{
    Height h = World::getHeight(loc.x, loc.y);
    h = max(h, World::getHeight(loc.x + 1, loc.y));
    h = max(h, World::getHeight(loc.x + 1, loc.y + 1));
    h = max(h, World::getHeight(loc.x, loc.y + 1));
    return h;
}

bool TerrainGen::surroundedByLake(Pos2 loc)
{
    if(World::getGround(loc.x - 1, loc.y) != LAKE)
        return false;
    if(World::getGround(loc.x, loc.y - 1) != LAKE)
        return false;
    if(World::getGround(loc.x + 1, loc.y) != LAKE)
        return false;
    if(World::getGround(loc.x, loc.y + 1) != LAKE)
        return false;
    return true;
}

bool TerrainGen::processLakeBoundaryTile(list<Pos2>& boundList, Pos2 bound, Pos2 check, Height floodH)
{
    if(World::getGround(check) == LAKE)
        return false;
    if(World::getHeight(check) <= floodH)
    {
        World::setGround(LAKE, check);
        boundList.push_back(check);
        if(hasNonLakeDownhill(check))
            return true;
    }
    return false;
}

void TerrainGen::recurseSetLakeHeight(Height h, Pos2 pos)
{
    Pos2 left = {short(pos.x - 1), pos.y};
    Pos2 up = {pos.x, short(pos.y - 1)};
    Pos2 right = {short(pos.x + 1), pos.y};
    Pos2 down = {pos.x, short(pos.y + 1)};
    if(World::getGround(left) == LAKE && World::getHeight(left) != h)
    {
        World::setHeight(h, left);
        recurseSetLakeHeight(h, left);
    }
    if(World::getGround(up) == LAKE && World::getHeight(up) != h)
    {
        World::setHeight(h, up);
        recurseSetLakeHeight(h, up);
    }
    if(World::getGround(right) == LAKE && World::getHeight(right) != h)
    {
        World::setHeight(h, right);
        recurseSetLakeHeight(h, right);
    }
    if(World::getGround(down) == LAKE && World::getHeight(down) != h)
    {
        World::setHeight(h, down);
        recurseSetLakeHeight(h, down);
    }
}