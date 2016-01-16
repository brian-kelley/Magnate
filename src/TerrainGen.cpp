#include "TerrainGen.h"

using namespace std;
using namespace Coord;
using namespace GlobalConfig;

TerrainGen::TerrainGen(Heightmap& worldHeights, Heightmap& worldBiomes) : world(worldHeights), biomes(worldBiomes)
{
    generate();
}

void getSlope(Heightmap& world)
{
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            world.set(i * 5 - 50, i, j);
        }
    }
}

void getCanal(Heightmap& world)
{
    getSlope(world);
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            world.add(abs(WORLD_SIZE / 2 - j) * 2, i, j);
        }
    }
}

void TerrainGen::generate()
{
    RandomUtils::seed(time(NULL));
    Heightmap rainfall(WORLD_SIZE, WORLD_SIZE);
    rainfall.diamondSquare(2, 0, 0, false);
    rainfall.normalize();
    defaultGen();
    Erosion erosion(world, rainfall);
    assignBiomes(rainfall);
    addWatershed(40, 0.5);
}

void TerrainGen::defaultGen()
{
    world.diamondSquare(2, 10, 1000, true);
    scatterCentralVolcanoes();
    clampSeaLevel();
    sphereMask();
    clampSeaLevel();
    stretchToFill();
    flattenWater();
    defuzz();
}

void TerrainGen::defuzz()
{
    for(short i = 0; i < WORLD_SIZE; i++)
    {
        for(short j = 0; j < WORLD_SIZE; j++)
        {
            if(world.get(i, j) != 0)
            {
                //Land tile
                int waterNeighbors = 0;
                for(int k = UP; k <= RIGHT; k++)
                {
                    if(world.get(getTileInDir({i, j}, k)) == 0)
                        waterNeighbors++;
                }
                //If too many water neighbors, convert to water
                if(waterNeighbors >= 3)
                    world.set(0, i, j);
            }
            else
            {
                //Non-ocean tile
                int landNeighbors = 0;
                for(int k = UP; k <= RIGHT; k++)
                {
                    if(world.get(getTileInDir({i, j}, k)) != 0)
                        landNeighbors++;
                }
                if(landNeighbors >= 3)
                    world.set(1, i, j);
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
            if(biomes.get(i, j) == LAKE || biomes.get(i, j) == WATER)
            {
                //get minimum height of all 4 corners
                auto first = world.get(i, j);
                auto minh = min(first, world.get(i + 1, j));
                minh = min(minh, world.get(i + 1, j + 1));
                minh = min(minh, world.get(i, j + 1));
                //set all corners to have this height
                if(minh != first)
                {
                    world.set(minh, i, j);
                    world.set(minh, i + 1, j);
                    world.set(minh, i + 1, j + 1);
                    world.set(minh, i, j + 1);
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
                world.add(change, i, j);
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
    short reduction = SHRT_MIN;
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
            if(world.get(i, j) <= 0)
                world.set(0, i, j);
        }
    }
}

void TerrainGen::smooth(int iters)
{
    Heightmap copy(world);
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
                copy.set(0.5 + sum / 5, i, j);
            }
        }
        world = copy;
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
            if(world.get(i, j) != WATER)
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
    short* buf = new short[WORLD_SIZE * WORLD_SIZE];
    for(int i = 0; i < WORLD_SIZE; i++) //i is new (dest) x
    {
        for(int j = 0; j < WORLD_SIZE; j++) //j is new (dest) y
        {
            float oldX = minx + (float(i) / WORLD_SIZE) * (maxx - minx);
            float oldY = miny + (float(j) / WORLD_SIZE) * (maxy - miny);
            //int(oldX) and int(oldX + 1) are the interpolants
            //cout << oldX << ", " << oldY << endl;
            float x1 = int(oldX);
            float x2 = x1 + 1;
            float y1 = int(oldY);
            float y2 = y1 + 1;
            short q11 = world.get(x1, y1);
            short q21 = world.get(x2, y1);
            short q12 = world.get(x1, y2);
            short q22 = world.get(x2, y2);
            buf[i * WORLD_SIZE + j] = q11*(x2-oldX)*(y2-oldY) + q21*(oldX-x1)*(y2-oldY) + q12*(x2-oldX)*(oldY-y1) + q22*(oldX-x1)*(oldY-y1);
        }
    }
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            world.set(buf[WORLD_SIZE * i + j], i, j);
        }
    }
    delete[] buf;
}

void TerrainGen::combinedGen()
{
    PRINT("Beginning world generation.");
    //Generate one buffer
    TIMEIT(defaultGen());
    TIMEIT(stretchToFill());
    //Save a copy of the first buffer
    Heightmap first = world;
    //Generate another buffer
    TIMEIT(clearAll());
    TIMEIT(defaultGen());
    TIMEIT(stretchToFill());
    //Combine them
    TIMEIT(world.add(first));
    TIMEIT(clampSeaLevel());
    Heightmap rainfall(world.getW(), world.getH());
    rainfall.diamondSquare(2.0, 0, 0, false);
    TIMEIT(assignBiomes(rainfall));
    TIMEIT(addWatershed(50, 0.6));
    TIMEIT(flattenWater());
    PRINT("Done with world generation.");
}

short TerrainGen::getAverageHeight()
{
    int sum = 0;
    int n = 0;
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            if(world.get(i, j) > 0)
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
            if(world.get(i, j) > 0)
                c++;
        }
    }
    return float(c) / (WORLD_SIZE * WORLD_SIZE);
}

void TerrainGen::addWatershed(int numRivers, float cutoff)
{
    short avgHeight = getAverageHeight();
    short maxHeight = getMaxHeight();
    unsmooth(maxHeight);
    scaleHeight(WORLD_SIZE / 2, getMaxHeight());
    //Add rivers to the world, then erode near rivers realistically
    Watershed ws(world, biomes, avgHeight + cutoff * (maxHeight - avgHeight), numRivers);
    scaleHeight(maxHeight, getMaxHeight());
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
    double factor = double(target) / maxH;
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            if(world.get(i, j) != 0)
                world.set(max<short>(1, world.get(i, j) * factor), i, j);
        }
    }
}

void TerrainGen::unsmooth(short maxH)
{
    //Scale up linearly so highest point is near SHRT_MAX
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

//Does dir describe how p1 is in relation to p2?
//Is p1 "more extreme" in that direction than p2?
bool dirComparePoints(Pos2 p1, Pos2 p2, int dir)
{
    switch(dir)
    {
        case 0: //N
            if(p1.y < p2.y)
                return true;
            return false;
        case 1: //NE
            if(p1.x - p1.y > p2.x - p2.y)
                return true;
            return false;
        case 2: //E
            if(p1.x > p2.x)
                return true;
            return false;
        case 3: //SE
            if(p1.x + p1.y > p2.x + p2.y)
                return true;
            return false;
        case 4: //S
            if(p1.y > p2.y)
                return true;
            return false;
        case 5: //SW
            if(p1.x - p1.y < p2.x - p2.y)
                return true;
            return false;
        case 6: //W
            if(p1.x < p2.x)
                return true;
            return false;
        case 7: //NW
            if(p1.x + p1.y < p2.x + p2.y)
                return true;
            return false;
        default:
            return false;
    }
}

Pos2 findExtreme(vector<Pos2>& pset, int dir)
{
    Pos2 rv = pset[0];
    for(Pos2 p : pset)
    {
        if(dirComparePoints(p, rv, dir))
            rv = p;
    }
    return rv;
}

void TerrainGen::assignBiomes(Heightmap& rainfall)
{
    //TODO: Use rainfall, height, latitude to decide biomes
    
    /*
    //First, add some random rainy regions
    int coldLat = WORLD_SIZE / 4;               //Combined width of bands of polar climate in north and south
    int tropicalLat = WORLD_SIZE / 6;
    for(int y = 0; y < WORLD_SIZE; y++)
    {
        Latitude lat;
        if(y < coldLat / 2 || y > (WORLD_SIZE - coldLat / 2))
            lat = POLAR;
        else if(y > (WORLD_SIZE - tropicalLat) / 2 && y < (WORLD_SIZE + tropicalLat) / 2)
            lat = TROPICAL;
        else
            lat = MODERATE;
        for(int x = 0; x < WORLD_SIZE; x++)
        {
            if(world.get(x, y) != WATER && world.get(x, y) != LAKE && world.get(x, y) != RIVER)
            {
                bool onOcean = false;
                for(int dir = UP; dir <= RIGHT; dir++)
                {
                    if(world.get(getTileInDir(Pos2(x, y), dir)) == WATER)
                    {
                        onOcean = true;
                        break;
                    }
                }
                if(onOcean && RandomUtils::gen() % 3)
                    biomes.set(BEACH, x, y);
                else
                    biomes.set(decideGround(rainMap[x][y], world.get(x, y) > peakAlt, lat), x, y);
            }
        }
    }
    TIMEIT(biomeSmooth(1));
     */
}

Ground TerrainGen::decideGround(bool rain, bool high, Latitude lat)
{
    //Wet: decid_forest snowcap rainforest taiga
    //Dry: conifer_forest plains desert stone tundra
    if(rain)
    {
        if(high)
            return STONE;
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

void TerrainGen::biomeSmooth(int iters)
{
    if(iters == 0)
        return;
    Heightmap copy = biomes;
    for(int it = 0; it < iters; it++)
    {
        //Smooth buffer based on World ground array
        for(int i = 0; i < WORLD_SIZE; i++)
        {
            for(int j = 0; j < WORLD_SIZE; j++)
            {
                Ground before = (Ground) copy.get(i, j);
                if(before != WATER && before != RIVER && before != LAKE)
                {
                    int occur[NUM_TYPES];
                    for(int k = 0; k < NUM_TYPES; k++)
                        occur[k] = 0;
                    //Find the most common ground type(s) in a 5x5 grid
                    for(int a = -2; a <= 2; a++)
                    {
                        for(int b = -1; b <= 2; b++)
                        {
                            occur[copy.get(i + a, j + b)]++;
                        }
                    }
                    //Now find the max. Pick randomly in case of a tie
                    int maxOccur = 0;
                    for(int k = 0; k < NUM_TYPES; k++)
                    {
                        if(occur[k] > maxOccur)
                            maxOccur = occur[k];
                    }
                    vector<Ground> maxGrounds;
                    for(int k = 0; k < NUM_TYPES; k++)
                    {
                        if(occur[k] == maxOccur)
                            maxGrounds.push_back(Ground(k));
                    }
                    if(maxGrounds.size() == 1)
                        biomes.set(maxGrounds[0], i, j);
                    else
                        biomes.set(maxGrounds[RandomUtils::gen() % maxGrounds.size()], i, j);
                }
            }
        }
    }
}

void TerrainGen::roughCone()
{
    const int h = WORLD_SIZE * 5;
    const int pad = WORLD_SIZE / 10;
    int r = WORLD_SIZE / 2 - pad;
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            int dx = WORLD_SIZE / 2 - i;
            int dy = WORLD_SIZE / 2 - j;
            int dist = sqrt(dx * dx + dy * dy);
            if(dist > r)
                world.set(0, i, j);
            else
                world.set((r - dist) * (float(h) / r), i, j);
        }
    }
}