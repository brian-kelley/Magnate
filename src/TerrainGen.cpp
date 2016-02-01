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
    RandomUtils::seed(start);
    clearAll(); //prevent invalid height/ground in world
    combinedGen();
    cout << "Generation took " << (double(clock()) - start) / CLOCKS_PER_SEC << " seconds." << endl;
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
                short minHeight = world.get(i, j);
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

void TerrainGen::sphereMask()
{
    short reduction = -SHRT_MAX;
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

short TerrainGen::maxHeightOfTile(Pos2 loc)
{
    short h = world.get(loc.x, loc.y);
    h = max(h, world.get(loc.x + 1, loc.y));
    h = max(h, world.get(loc.x + 1, loc.y + 1));
    h = max(h, world.get(loc.x, loc.y + 1));
    return h;
}

void TerrainGen::smooth(int iters)
{
    Heightmap copy = world;
    for(int it = 0; it < iters; it++)
    {
        for(int i = 1; i < WORLD_SIZE - 1; i++)
        {
            for(int j = 1; j < WORLD_SIZE - 1; j++)
            {
                int sum = 0;
                sum += copy.get(i, j);
                sum += copy.get(i + 1, j);
                sum += copy.get(i - 1, j);
                sum += copy.get(i, j + 1);
                sum += copy.get(i, j - 1);
                world.set(sum / 5, i, j);
            }
        }
    }
}

void TerrainGen::scatterCentralVolcanoes()
{
    const int numVolcanoes = 6;
    const int minEdgeDist = WORLD_SIZE / 8;
    const int minDiam = WORLD_SIZE / 10;
    const int maxDiam = 2 * minDiam;
    const float minSlope = 0.1;
    const float maxSlope = 0.5;
    int x = 0;
    int y = 0;
    int diam;
    float slope;
    for(int i = 0; i < numVolcanoes; i++)
    {
        x = RandomUtils::gen() % (WORLD_SIZE - minEdgeDist * 2) + minEdgeDist;
        y = RandomUtils::gen() % (WORLD_SIZE - minEdgeDist * 2) + minEdgeDist;
        diam = minDiam + RandomUtils::gen() % (maxDiam - minDiam);
        slope = minSlope + (RandomUtils::gen() % 1000) * (maxSlope - minSlope) / 1000;
        addVolcano(x, y, diam * slope / 2, diam / 2);
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
    short reduction = world.get(0, 0);
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
    const int pad = 10;
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
    minx -= pad; //give 10 tiles of space on all sides
    maxx += pad;
    miny -= pad;
    maxy += pad;
    if(minx < 0)
        minx = 0;
    if(maxx >= WORLD_SIZE)
        maxx = WORLD_SIZE - 1;
    if(miny < 0)
        miny = 0;
    if(maxy >= WORLD_SIZE)
        maxy = WORLD_SIZE - 1;
    Heightmap copy = world;
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            world.set(copy.get(minx + float(i) / WORLD_SIZE * (maxx - minx), miny + float(j) / WORLD_SIZE * (maxy - miny)), i, j);
        }
    }
    smooth();
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            if(world.get(i, j) <= 0)
                biomes.set(WATER, i, j);
            else
                biomes.set(DESERT, i, j);
        }
    }
}

void TerrainGen::defaultGen()
{
    biomes.set(DESERT);
    world.diamondSquare(2, 10, 100, true);
    scatterCentralVolcanoes();
    sphereMask();
    clampSeaLevel();
    defuzz();
}

void TerrainGen::combinedGen()
{
    //Generate one buffer
    defaultGen();
    stretchToFill();
    //Save the first buffer
    Heightmap first = world;
    //Generate another buffer
    clearAll();
    defaultGen();
    stretchToFill();
    //Combine them
    world.add(first);
    smooth(2);
    simpleBiomes();
    short avgH = getAverageHeight();
    short maxH = getMaxHeight();
    unsmooth(maxH);
    addWatershed(0.6, maxH, avgH);
    scaleHeight(WORLD_SIZE / 2, getMaxHeight());
}

short TerrainGen::getAverageHeight()
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

void TerrainGen::addWatershed(float cutoff, short maxH, short avgH)
{
    Watershed(50, avgH + cutoff * (maxH - avgH));
}

short TerrainGen::getMaxHeight()
{
    short h = -1000;
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

void TerrainGen::unsmooth(short maxH)
{
    scaleHeight(5 * maxH, maxH);
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            if(biomes.get(i, j) != WATER && world.get(i, j) > 0)
            {
                short h, n1, n2, n3, n4;
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

void TerrainGen::simpleBiomes()
{
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            if(world.get(i, j) > 0)
                biomes.set(DESERT, i, j);
            else
                biomes.set(WATER, i, j);
        }
    }
}