#include "TerrainGen.h"

using namespace std;
using namespace Coord;
using namespace GlobalConfig;

TerrainGen::TerrainGen() : world(World::getHeights()), biomes(World::getBiomes())
{
    generate();
}

void TerrainGen::generate()
{
    clock_t start = clock();
    //RandomUtils::seed(2);
    clearAll(); //prevent invalid height/ground in world
    combinedGen();
    cout << "Generation took " << (double(clock()) - start) / CLOCKS_PER_SEC << " seconds." << endl;
}

bool TerrainGen::inMesh(int x, int y)
{
    if(x > 0 && x < WORLD_SIZE && y > 0 && y < WORLD_SIZE)
        return true;
    else
        return false;
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
                if(world.validPoint(i, j))
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
        float heightMult = Coord::TERRAIN_TILE_SIZE / Coord::TERRAIN_Y_SCALE;
        int radius = (height * (RandomUtils::gen() % 5 + 5)) / heightMult;
        addVolcano(x, y, height, radius);
    }
}

void TerrainGen::pyramidMask()
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
            double dist = double(max(abs(i - WORLD_SIZE / 2), abs(j - WORLD_SIZE / 2))) / (WORLD_SIZE / 2);
            world.set(world.get(i, j) - dist * reduction, i, j);
        }
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
                world.set(0, i, j);
        }
    }
}

void TerrainGen::erode(int numTimesteps)
{
    //Erosion(world, rainfall);
}

short TerrainGen::maxHeightOfTile(Pos2 loc)
{
    short h = world.get(loc.x, loc.y);
    h = max(h, world.get(loc.x + 1, loc.y));
    h = max(h, world.get(loc.x + 1, loc.y + 1));
    h = max(h, world.get(loc.x, loc.y + 1));
    return h;
}

void TerrainGen::shelfMask()
{
    short slopeDrop = world.get(0, 0);
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
                short drop = (slopeLen - j) * slope;
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
    //Do bilinear interpolation to get new heights
    int minx = WORLD_SIZE / 2;
    int maxx = minx;
    int miny = minx;
    int maxy = minx;
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            if(world.get(i, j) > 0)
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
    Heightmap copy = world;
    for(int i = 0; i < WORLD_SIZE; i++) //i is new (dest) x
    {
        for(int j = 0; j < WORLD_SIZE; j++) //j is new (dest) y
        {
            float oldX = minx + (float(i) / WORLD_SIZE) * (maxx - minx);
            float oldY = miny + (float(j) / WORLD_SIZE) * (maxy - miny);
            float x1 = int(oldX);
            float x2 = x1 + 1;
            float y1 = int(oldY);
            float y2 = y1 + 1;
            short q11 = copy.get(x1, y1);
            short q21 = copy.get(x2, y1);
            short q12 = copy.get(x1, y2);
            short q22 = copy.get(x2, y2);
            world.set(q11*(x2-oldX)*(y2-oldY) + q21*(oldX-x1)*(y2-oldY) + q12*(x2-oldX)*(oldY-y1) + q22*(oldX-x1)*(oldY-y1), i, j);
        }
    }
}

void TerrainGen::defaultGen()
{
    world.diamondSquare(2, 10, 10, true);
    scatterCentralVolcanoes();
    sphereMask();
    clampSeaLevel();
    smooth();
    defuzz();
}

void TerrainGen::combinedGen()
{
    //Create two "default gen" diamond-square islands
    //Stretch both to the boundaries of map
    //Add them together
    defaultGen();
    stretchToFill();
    //Save the first buffer
    Heightmap first = world;
    defaultGen();
    stretchToFill();
    //Combine them
    world.add(first);
    clampSeaLevel();
    unsmooth();
    assignBiomes();
    addWatershed(0.5, SHRT_MAX - 50, getAverageHeight());
    scaleHeight(WORLD_SIZE / 2, SHRT_MAX - 50);
    smooth(3);
    clampSeaLevel();
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

void TerrainGen::unsmooth()
{
    short maxH = getMaxHeight();
    //Scale up as much as possible
    int mult = (SHRT_MAX - 50) / maxH;
    scaleHeight(mult * maxH, maxH);
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            if(world.get(i, j) > 0)
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

void TerrainGen::assignBiomes()
{
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            if(biomes.get(i, j) != RIVER && biomes.get(i, j) != LAKE)
            {
                if(world.get(i, j) <= 0)
                    biomes.set(WATER, i, j);
                else
                    biomes.set(DESERT, i, j);
            }
        }
    }
}

Ground TerrainGen::decideGround(bool rain, bool high, Latitude lat)
{
    //Wet: decid_forest snowcap rainforest taiga
    //Dry: conifer_forest plains desert stone tundra
    if(rain)
    {
        if(high)
            return SNOWCAP;
        if(lat == TROPICAL)
            return RAINFOREST;
        if(lat == POLAR)
            return TAIGA;
        //Default for wet, moderate elevation, moderate latitude
        return DECID_FOREST;
    }
    else
    {
        if(high)
            return STONE;
        if(lat == POLAR)
            return TUNDRA;
        
            return PLAINS;
        return CONIFER_FOREST;
    }
}

void TerrainGen::addWatershed(float cutoff, short maxH, short avgH)
{
    Watershed(cutoff * maxH, 30);
}