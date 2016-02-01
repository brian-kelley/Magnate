#include "TerrainGen.h"

using namespace std;
using namespace GlobalConfig;
using namespace Coord;

TerrainGen::TerrainGen() : world(World::getHeights()), biomes(World::getBiomes())
{
    generate();
}

void TerrainGen::generate()
{
    clock_t start = clock();
    RandomUtils::seed(time(NULL));
    //RandomUtils::seed(2);
    clearAll(); //prevent invalid height/ground in world
    combinedGen();
    cout << "Generation took " << (double(clock()) - start) / CLOCKS_PER_SEC << " seconds." << endl;
}

void TerrainGen::tester()
{
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            world.set(i, i, j);
            biomes.set(DESERT, i, j);
        }
    }
    for(int i = 0; i < 50; i++)
    {
        addVolcano(RandomUtils::gen() % WORLD_SIZE, RandomUtils::gen() % WORLD_SIZE, -100, 150);
    }
    clampSeaLevel();
    Watershed(50, getMaxHeight() * 0.6);
}

//Generates height based on average surrounding height, and how fine grid is
Height TerrainGen::getHeight(int avg, int size)
{
    int range = double(size) * ROUGHNESS;
    if(range == 0)
        return avg;
    range = range > SHRT_MAX ? SHRT_MAX : range;
    int result = (avg + (RandomUtils::gen() % range) - (range / 2));
    return result;
}

void TerrainGen::diamondSquare()
{
    const Height seedH = 100;
    //World corners
    world.set(10, 0, 0);
    world.set(10, WORLD_SIZE - 1, 0);
    world.set(10, 0, WORLD_SIZE - 1);
    world.set(10, WORLD_SIZE - 1, WORLD_SIZE - 1);
    //World edge midpoints
    world.set(10, WORLD_SIZE / 2, 0);
    world.set(10, 0, WORLD_SIZE);
    world.set(10, WORLD_SIZE / 2, WORLD_SIZE - 1);
    world.set(10, WORLD_SIZE - 1, WORLD_SIZE / 2);
    //Center tile
    world.set(seedH, WORLD_SIZE / 2, WORLD_SIZE / 2);
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
        sum += world.get(x - size / 2, y);
    }
    if(inMesh(x + size / 2, y))
    {
        n++;
        sum += world.get(x + size / 2, y);
    }
    if(inMesh(x, y - size / 2))
    {
        n++;
        sum += world.get(x, y - size / 2);
    }
    if(inMesh(x, y + size / 2))
    {
        n++;
        sum += world.get(x, y + size / 2);
    }
    if(n == 0)
        return;
    world.set(getHeight(sum / n, size), x, y);
    biomes.set(DESERT, x, y);
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
        sum += world.get(x - size / 2, y - size / 2);
    }
    if(inMesh(x - size / 2, y + size / 2))
    {
        n++;
        sum += world.get(x - size / 2, y + size / 2);
    }
    if(inMesh(x + size / 2, y + size / 2))
    {
        n++;
        sum += world.get(x + size / 2, y + size / 2);
    }
    if(inMesh(x + size / 2, y - size / 2))
    {
        n++;
        sum += world.get(x + size / 2, y - size / 2);
    }
    //Put in height for center of square
    if(n == 0)
        return;
    world.set(getHeight(sum / n, size), x, y);
    biomes.set(DESERT, x, y);
}

void TerrainGen::defuzz()
{
    for(short i = 0; i < WORLD_SIZE; i++)
    {
        for(short j = 0; j < WORLD_SIZE; j++)
        {
            if(biomes.get(i, j) != WATER)
            {
                int waterNeighbors = 0;
                for(int k = UP; k <= RIGHT; k++)
                {
                    if(biomes.get(getTileInDir({i, j}, k)) == WATER)
                        waterNeighbors++;
                }
                if(waterNeighbors >= 3)
                {
                    biomes.set(WATER, i, j);
                }
            }
            else
            {
                int waterNeighbors = 0;
                for(int k = UP; k <= RIGHT; k++)
                {
                    if(biomes.get(getTileInDir({i, j}, k)) != WATER)
                        waterNeighbors++;
                }
                if(waterNeighbors >= 3)
                {
                    world.set(1, i, j);
                    biomes.set(DESERT, i, j);
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
            if(biomes.get(i, j) == WATER)
            {
                Height minHeight = world.get(i, j);
                bool needsUpdate = false;
                if(world.get(i + 1, j) < minHeight)
                {
                    minHeight = world.get(i + 1, j);
                    needsUpdate = true;
                }
                if(world.get(i + 1, j + 1) < minHeight)
                {
                    minHeight = world.get(i + 1, j + 1);
                    needsUpdate = true;
                }
                if(world.get(i, j + 1) < minHeight)
                {
                    minHeight = world.get(i, j + 1);
                    needsUpdate = true;
                }
                if(needsUpdate)
                {
                    world.set(minHeight, i, j);
                    world.set(minHeight, i + 1, j);
                    world.set(minHeight, i + 1, j + 1);
                    world.set(minHeight, i, j + 1);
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
                int change = 0.5 + height - (float(height) * (dist / rad));
                world.set(world.get(i, j) + change, i, j);
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
            biomes.set(WATER, i, j);
            world.set(0, i, j);
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
    Height reduction = -SHRT_MAX;
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        if(world.get(i, 0) > reduction)
            reduction = world.get(i, 0);
        if(world.get(0, i) > reduction)
            reduction = world.get(0, i);
        if(world.get(i, WORLD_SIZE - 1) > reduction)
            reduction = world.get(i, WORLD_SIZE - 1);
        if(world.get(WORLD_SIZE - 1, i) > reduction)
            reduction = world.get(WORLD_SIZE - 1, i);
    }
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            double dist = double(max(abs(i - WORLD_SIZE / 2), abs(j - WORLD_SIZE / 2))) / (WORLD_SIZE / 2);
            world.set(world.get(i, j) - dist * reduction, i, j);
            //world.set(world.get(i, j) - reduction, i, j);
        }
    }
}

void TerrainGen::sphereMask()
{
    Height reduction = -SHRT_MAX;
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        if(world.get(i, 0) > reduction)
            reduction = world.get(i, 0);
        if(world.get(0, i) > reduction)
            reduction = world.get(0, i);
        if(world.get(i, WORLD_SIZE - 1) > reduction)
            reduction = world.get(i, WORLD_SIZE - 1);
        if(world.get(WORLD_SIZE - 1, i) > reduction)
            reduction = world.get(WORLD_SIZE - 1, i);
    }
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            float dx = i - WORLD_SIZE / 2;
            float dy = j - WORLD_SIZE / 2;
            float dist = sqrt(dx * dx + dy * dy) / (WORLD_SIZE / 2);
            world.add(-dist * reduction, i, j);
        }
    }
}

void TerrainGen::clampSeaLevel()
{
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            if(world.get(i, j) < 0)
            {
                biomes.set(WATER, i, j);
                world.set(0, i, j);
            }
        }
    }
}

void TerrainGen::erode(int numTimesteps)
{
    
}

Height TerrainGen::maxHeightOfTile(Pos2 loc)
{
    Height h = world.get(loc.x, loc.y);
    h = max(h, world.get(loc.x + 1, loc.y));
    h = max(h, world.get(loc.x + 1, loc.y + 1));
    h = max(h, world.get(loc.x, loc.y + 1));
    return h;
}

void TerrainGen::shelfMask()
{
    Height slopeDrop = world.get(0, 0);
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        if(world.get(0, i) > slopeDrop)
            slopeDrop = world.get(0, i);
        if(world.get(i, 0) > slopeDrop)
            slopeDrop = world.get(i, 0);
        if(world.get(WORLD_SIZE - 1, i) > slopeDrop)
            slopeDrop = world.get(WORLD_SIZE - 1, i);
        if(world.get(i, WORLD_SIZE - 1) > slopeDrop)
            slopeDrop = world.get(i, WORLD_SIZE - 1);
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
                world.add(-drop, i, j);
                world.add(-drop, WORLD_SIZE - j, i);
                world.add(-drop, WORLD_SIZE - i, WORLD_SIZE - j);
                world.add(-drop, j, WORLD_SIZE - i);
                if(j == slopeLen - 1)
                {
                    biomes.set(MOUNTAINS, i, j);
                    biomes.set(MOUNTAINS, WORLD_SIZE - j, i);
                    biomes.set(MOUNTAINS, WORLD_SIZE - i, WORLD_SIZE - j);
                    biomes.set(MOUNTAINS, j, WORLD_SIZE - i);
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
                sum += world.get(i, j);
                sum += world.get(i + 1, j);
                sum += world.get(i - 1, j);
                sum += world.get(i, j + 1);
                sum += world.get(i, j - 1);
                buf[i * WORLD_SIZE + j] = sum / 5;
            }
        }
        for(int i = 1; i < WORLD_SIZE - 1; i++)
        {
            for(int j = 1; j < WORLD_SIZE - 1; j++)
            {
                world.set(buf[i * WORLD_SIZE + j], i, j);
            }
        }
    }
    delete[] buf;
}

void TerrainGen::scatterCentralVolcanoes()
{
    const int numVolcanoes = 3;
    const int minEdgeDist = 300;
    const int minDiam = 300;
    const int maxDiam = 600;
    const float minSlope = 0.1;
    const float maxSlope = 0.5;
    int x = 0;
    int y = 0;
    int diam;
    float slope;
    for(int i = 0; i < numVolcanoes; i++)
    {
        while(world.get(x, y) > 500) //Don't place the volcano center on already high ground
        {
            x = RandomUtils::gen() % (WORLD_SIZE - minEdgeDist * 2) + minEdgeDist;
            y = RandomUtils::gen() % (WORLD_SIZE - minEdgeDist * 2) + minEdgeDist;
        }
        diam = minDiam + RandomUtils::gen() % (maxDiam - minDiam);
        slope = minSlope + (RandomUtils::gen() % 1000) * (maxSlope - minSlope) / 1000;
        addVolcano(x, y, diam * slope / 2, diam / 2);
        for(int i = -5; i < 5; i++)
        {
            for(int j = -5; j < 5; j++)
            {
                biomes.set(MOUNTAINS, i + x, j + y);
            }
        }
    }
}

void TerrainGen::addEntropy()
{
    for(int i = 0; i < WORLD_SIZE * WORLD_SIZE / 100; i++)
    {
        world.add(-2, RandomUtils::gen() % WORLD_SIZE, RandomUtils::gen() % WORLD_SIZE);
    }
}

void TerrainGen::verticalNormalize()
{
    Height reduction = world.get(0, 0);
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        if(world.get(0, i) > reduction)
            reduction = world.get(0, i);
        if(world.get(i, 0) > reduction)
            reduction = world.get(i, 0);
        if(world.get(WORLD_SIZE - 1, i) > reduction)
            reduction = world.get(WORLD_SIZE - 1, i);
        if(world.get(i, WORLD_SIZE - 1) > reduction)
            reduction = world.get(i, WORLD_SIZE - 1);
    }
    reduction += 3;
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            world.add(-reduction, i, j);
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
            if(biomes.get(i, j) != WATER)
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
            buf[WORLD_SIZE * i + j] = world.get(minx + float(i) / WORLD_SIZE * (maxx - minx), miny + float(j) / WORLD_SIZE * (maxy - miny));
        }
    }
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            world.set(buf[WORLD_SIZE * i + j], i, j);
            if(world.get(i, j) <= 0)
                biomes.set(WATER, i, j);
            else
                biomes.set(DESERT, i, j);
        }
    }
    delete[] buf;
}

void TerrainGen::defaultGen()
{
    diamondSquare();
    scatterCentralVolcanoes();
    sphereMask();
    clampSeaLevel();
    smooth();
    defuzz();
}

void TerrainGen::combinedGen()
{
    //Generate one buffer
    defaultGen();
    stretchToFill();
    //Save the first buffer
    Height* firstGen = getHeightBuffer();
    //Generate another buffer
    clearAll();
    defaultGen();
    stretchToFill();
    //Combine them
    addBuffer(firstGen);
    smooth(6);
    delete[] firstGen;
    flattenWater();
    Height avgH = getAverageHeight();
    Height maxH = getMaxHeight();
    unsmooth(maxH);
    addWatershed(0.6, maxH, avgH);
    scaleHeight(WORLD_SIZE / 2, getMaxHeight());
    clampSeaLevel();
}

Height* TerrainGen::getHeightBuffer()
{
    Height* buf = new Height[WORLD_SIZE * WORLD_SIZE];
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            buf[i + j * WORLD_SIZE] = world.get(i, j);
        }
    }
    return buf;
}

void TerrainGen::addBuffer(Height *buf)
{
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            world.add(buf[i + j * WORLD_SIZE], i, j);
            if(world.get(i, j) > 0)
                biomes.set(DESERT, i, j);
        }
    }
}

Height TerrainGen::getAverageHeight()
{
    int sum = 0;
    int n = 0;
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            if(biomes.get(i, j) != WATER)
            {
                sum += world.get(i, j);
                n++;
            }
        }
    }
    return double(sum) / n;
}

float TerrainGen::getLandArea()
{
    int c = 0;
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            if(biomes.get(i, j) != WATER)
                c++;
        }
    }
    return float(c) / (WORLD_SIZE * WORLD_SIZE);
}

void TerrainGen::fancySmooth()
{
    const int bsize = 3;
    Height* buf = new Height[WORLD_SIZE * WORLD_SIZE];
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            int sum = 0;
            for(int k = i - bsize / 2; k <= i + bsize / 2; k++)
            {
                for(int l = j - bsize / 2; l <= j + bsize / 2; l++)
                {
                    sum += world.get(k, l);
                }
            }
            buf[i + j * WORLD_SIZE] = sum / (bsize * bsize);
        }
    }
    clearAll();
    addBuffer(buf);
    delete[] buf;
}

void TerrainGen::addWatershed(float cutoff, Height maxH, Height avgH)
{
    Watershed(50, avgH + cutoff * (maxH - avgH));
}

/*
void TerrainGen::removeLake(Pos2 pos)
{
    queue<Pos2> q;
    q.push(pos);
    while(q.size() > 0)
    {
        Pos2 proc = q.front();
        q.pop();
        short procG = biomes.get(proc);
        if(procG == LAKE || procG == FLOODING)
        {
            biomes.set(DESERT, proc);
            for(int dir = UP; dir <= RIGHT; dir++)
            {
                Pos2 inDir = getTileInDir(proc, dir);
                Ground g = biomes.get(inDir);
                if(g == LAKE || g == FLOODING)
                    q.push(inDir);
            }
        }
    }
}
*/

void TerrainGen::placeRivers(float headAlt, int num)
{
    Height avg = getAverageHeight();
    for(int i = 0; i < num; i++)
    {
        Pos2 loc;
        do
        {
            loc.x = RandomUtils::gen() % WORLD_SIZE;
            loc.y = RandomUtils::gen() % WORLD_SIZE;
        }
        while(world.get(loc) < headAlt * avg);
    }
}

Height TerrainGen::getMaxHeight()
{
    Height h = -1000;
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            if(world.get(i, j) > h)
                h = world.get(i, j);
        }
    }
    return h;
}

void TerrainGen::riverHeightAdjust()
{
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            //Create very shallow river valley
            if(biomes.get(i, j) == RIVER)
                addVolcano(i, j, -1, 40);
        }
    }
}

void TerrainGen::scaleHeight(int target, int maxH)
{
    double factor = (double) target / maxH;
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            if(world.get(i, j) != 0)
                world.set(world.get(i, j) * factor, i, j);
            else if (biomes.get(i, j) == DESERT)  //Make sure land tiles are never at 0 height
                world.add(1, i, j);
        }
    }
}

void TerrainGen::unsmooth(Height maxH)
{
    scaleHeight(5 * maxH, maxH);
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            if(biomes.get(i, j) != WATER && world.get(i, j) > 0)
            {
                Height h, n1, n2, n3, n4;
                while(true)
                {
                    h = world.get(i, j);
                    n1 = world.get(i + 1, j);
                    n2 = world.get(i - 1, j);
                    n3 = world.get(i, j + 1);
                    n4 = world.get(i, j - 1);
                    if(h == n1 || h == n2 || h == n3 || h == n4)
                        world.add(1, i, j);
                    else
                        break;
                }
            }
        }
    }
}