#include "TerrainGen.h"

using namespace std;
using namespace GlobalConfig;
using namespace Coord;

TerrainGen::TerrainGen() : world(World::getHeights()), biomes(World::getBiomes()), rainfall(WORLD_SIZE, WORLD_SIZE)
{
    RandomUtils::seed(12);
    generate();
    //analyzeBiomes();
}

void TerrainGen::generate()
{
    clock_t start = clock();
    RandomUtils::seed(start);
    clearAll(); //prevent invalid height/ground in world
    //Generate one buffer
    getDSLayer();
    stretchToFill();
    //Save the first buffer
    Heightmap first = world;
    //Generate another buffer
    clearAll();
    getDSLayer();
    stretchToFill();
    //Combine them
    world.add(first);
    world.smooth(2);
    short avgH = getAverageHeight();
    short maxH = getMaxHeight();
    //Erosion e(world, focusLocs);
    unsmooth(maxH);
    assignBiomes();                 //this intializes [0,1000) rainfall map
    defuzz();
    addWatershed(0.6, maxH, avgH);
    scaleHeight(WORLD_SIZE / 2, getMaxHeight());
    flattenWater();
}

void TerrainGen::defuzz()
{
    for(short i = 0; i < WORLD_SIZE; i++)
    {
        for(short j = 0; j < WORLD_SIZE; j++)
        {
            Pos2 loc(i, j);
            int numDifferent = 0;
            auto here = biomes.get(loc);
            for(int dir = UP; dir <= RIGHT; dir++)
            {
                auto neiGround = biomes.get(getTileInDir(loc, dir));
                if(neiGround != here)
                    numDifferent++;
            }
            if(numDifferent >= 3)
                biomes.set(biomes.get(getTileInDir(loc, RandomUtils::gen() % 4)), loc);
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

void TerrainGen::scatterCentralVolcanoes()
{
    const int numVolcanoes = 10;
    const int minEdgeDist = WORLD_SIZE / 6;
    const int minDiam = WORLD_SIZE / 8;
    const int maxDiam = 2 * minDiam;
    const float minSlope = 0.2;
    const float maxSlope = 3;
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
        focusLocs.push_back(Pos2(x, y));
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
    world.smooth();
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

void TerrainGen::getDSLayer()
{
    biomes.set(DESERT);
    world.diamondSquare(2, 10, 100, true);
    scatterCentralVolcanoes();
    sphereMask();
    clampSeaLevel();
    defuzz();
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

void TerrainGen::assignBiomes()
{
    Heightmap rainfall(WORLD_SIZE, WORLD_SIZE);
    rainfall.diamondSquare(10, 0, 0, false);
    rainfall.normalize();
    float heightScale = 1000.0 / (SHRT_MAX - 50);
    Heightmap temperature(WORLD_SIZE, WORLD_SIZE);
    temperature.diamondSquare(10, 0, 0, false);
    temperature.normalize(300);
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            temperature.add(-world.get(i, j) * heightScale, i, j);  //Higher = colder
            temperature.add(-3 * abs(WORLD_SIZE / 2 - j), i, j);     //Higher latitude = colder
        }
    }
    temperature.normalize();
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            if(world.get(i, j) <= 0)
                biomes.set(WATER, i, j);
            else
            {
                biomes.set(getGround(temperature.get(i, j), rainfall.get(i, j), world.get(i, j) * heightScale), i, j);
            }
        }
    }
}

Ground TerrainGen::getGround(int temperature, int wetness, int altitude)
{
    if(altitude > 900)
    {
        if(temperature < 450)
            return SNOWCAP;
        else
            return MOUNTAINS;
    }
    else
    {
        //Non-mountainous heights
        if(wetness > 800)
            //wet
            return RAINFOREST;
        else if(wetness < 300)
        {
            //dry
            if(temperature < 300)
                //dry & cold
                return TUNDRA;
            else if(temperature > 700)
                //dry & hot
                return DESERT;
            else
                //dry & med. temp
                return PLAINS;
        }
        else
        {
            if(temperature > 800)
                return SAVANNA;
            else if(temperature < 300)
                return BOREAL_FOREST;
            else
                return DECID_FOREST;
        }
    }
}

void TerrainGen::analyzeBiomes()
{
    float occur[Ground::NUM_TYPES] = {0};
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            occur[biomes.get(i, j)]++;
        }
    }
    for(int i = 0; i < NUM_TYPES; i++)
    {
        occur[i] /= (WORLD_SIZE * WORLD_SIZE);
        occur[i] *= 100;
        if(occur[i] > 1)
            printf("%-15s: %f\n", Terrain::getName(Ground(i)).c_str(), occur[i]);
    }
}