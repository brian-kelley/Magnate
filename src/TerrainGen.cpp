#include "TerrainGen.h"

using namespace std;
using namespace constants;
using namespace Coord;

void TerrainGen::generate()
{
    RandomUtils::seed(time(NULL));
    //RandomUtils::seed(2);
    clearAll(); //prevent invalid height/ground in world
    TIMEIT(erosionGen())
}

void TerrainGen::tester()
{
    diamondSquare();
}

void TerrainGen::erosionGen()
{
    Heightmap h = Erosion::erosion();
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            if(h.get(i, j) <= 0)
            {
                World::setHeight(0, i, j);
                World::setGround(WATER, i, j);
            }
            else
            {
                World::setHeight(h.get(i, j), i, j);
                World::setGround(DESERT, i, j);
            }
        }
    }
    scaleHeight(500, getMaxHeight());
    //addWatershed(0.6, 500, getAverageHeight());
    //assignBiomes();
}

void TerrainGen::diamondSquare()
{
    Heightmap h(WORLD_SIZE, WORLD_SIZE);
    h.diamondSquare(2.0, 10, 1000, true);
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            World::setHeight(h.get(i, j), i, j);
            if(World::getHeight(i, j) > 0)
                World::setGround(DESERT, i, j);
            else
                World::setGround(WATER, i, j);
        }
    }
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
                int change = 0.5 + height - (float(height) * (dist / rad));
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

void TerrainGen::sphereMask()
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
            float dx = i - WORLD_SIZE / 2;
            float dy = j - WORLD_SIZE / 2;
            float dist = sqrt(dx * dx + dy * dy) / (WORLD_SIZE / 2);
            World::chgHeight(-dist * reduction, i, j);
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

Height TerrainGen::maxHeightOfTile(Pos2 loc)
{
    Height h = World::getHeight(loc.x, loc.y);
    h = max(h, World::getHeight(loc.x + 1, loc.y));
    h = max(h, World::getHeight(loc.x + 1, loc.y + 1));
    h = max(h, World::getHeight(loc.x, loc.y + 1));
    return h;
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
        while(World::getHeight(x, y) > 500) //Don't place the volcano center on already high ground
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
    //Do bilinear interpolation to get new heights
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
            Height q11 = World::getHeight(x1, y1);
            Height q21 = World::getHeight(x2, y1);
            Height q12 = World::getHeight(x1, y2);
            Height q22 = World::getHeight(x2, y2);
            buf[i * WORLD_SIZE + j] = q11*(x2-oldX)*(y2-oldY) + q21*(oldX-x1)*(y2-oldY) + q12*(x2-oldX)*(oldY-y1) + q22*(oldX-x1)*(oldY-y1);
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
    PRINT("Beginning world generation.");
    //Generate one buffer
    TIMEIT(defaultGen());
    TIMEIT(stretchToFill());
    //Save the first buffer
    Height* firstGen = getHeightBuffer();
    //Generate another buffer
    TIMEIT(clearAll());
    TIMEIT(defaultGen());
    TIMEIT(stretchToFill());
    //Combine them
    TIMEIT(addBuffer(firstGen));
    delete[] firstGen;
    TIMEIT(flattenWater());
    TIMEIT(clampSeaLevel());
    TIMEIT(assignBiomes());
    TIMEIT(addPeaks());
    Height avgH = getAverageHeight();
    Height maxH = getMaxHeight();
    TIMEIT(unsmooth(maxH));
    TIMEIT(addWatershed(0.6, maxH, avgH));
    TIMEIT(scaleHeight(WORLD_SIZE / 2, getMaxHeight()));
    //TIMEIT(smooth(3));
    PRINT("Done with world generation.");
}

Height* TerrainGen::getHeightBuffer()
{
    Height* buf = new Height[WORLD_SIZE * WORLD_SIZE];
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            buf[i + j * WORLD_SIZE] = World::getHeight(i, j);
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
            World::chgHeight(buf[i + j * WORLD_SIZE], i, j);
            if(World::getHeight(i, j) > 0)
                World::setGround(DESERT, i, j);
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
            if(World::getGround(i, j) != WATER)
            {
                sum += World::getHeight(i, j);
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
            if(World::getGround(i, j) != WATER)
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
                    sum += World::getHeight(k, l);
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
    //Add rivers to the world, then erode near rivers realistically
    for(int i = 0; i < 50; i++)
    {
        Watershed::addRiver(avgH + cutoff * (maxH - avgH));
    }
}

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
        while(World::getHeight(loc) < headAlt * avg);
        Watershed::beginFlow(loc);
    }
}

Height TerrainGen::getMaxHeight()
{
    Height h = -1000;
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            if(World::getHeight(i, j) > h)
                h = World::getHeight(i, j);
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
            if(World::getGround(i, j) == RIVER)
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
            if(World::getHeight(i, j) != 0)
                World::setHeight(World::getHeight(i, j) * factor, i, j);
            else if (World::getGround(i, j) == DESERT)  //Make sure land tiles are never at 0 height
                World::chgHeight(1, i, j);
        }
    }
}

void TerrainGen::unsmooth(Height maxH)
{
    //Scale up as much as possible
    int mult = (HEIGHT_MAX - 50) / maxH;
    scaleHeight(mult * maxH, maxH);
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            if(World::getGround(i, j) != WATER && World::getHeight(i, j) > 0)
            {
                Height h, n1, n2, n3, n4;
                while(true)
                {
                    h = World::getHeight(i, j);
                    n1 = World::getHeight(i + 1, j);
                    n2 = World::getHeight(i - 1, j);
                    n3 = World::getHeight(i, j + 1);
                    n4 = World::getHeight(i, j - 1);
                    if(h == n1 || h == n2 || h == n3 || h == n4)
                        World::chgHeight(1, i, j);
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

void TerrainGen::addPeaks()
{
    //scatter a bunch of points in a high area
    //make a MST of the points
    using namespace boost;
    typedef adjacency_list<vecS, vecS, undirectedS> Graph;
    typedef pair<int, int> Edge;
}

void TerrainGen::assignBiomes()
{
    vector<vector<bool>> rainMap;
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        rainMap.push_back(vector<bool>(WORLD_SIZE));
        for(int j = 0; j < WORLD_SIZE; j++)
            rainMap[i][j] = false;
    }
    //First, add some random rainy regions
    int coldLat = WORLD_SIZE / 4;               //Combined width of bands of polar climate in north and south
    int tropicalLat = WORLD_SIZE / 6;
    int peakAlt = getMaxHeight() * (3.0 / 4);   //Heights at and above this are mountain
    addRandomRain(rainMap);
    //addCoastalRain(rainMap);
    //addTropicalRain(tropicalLat, rainMap);
    //addRiverRain(rainMap);
    //Then add some rainy regions in west coastal areas
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
            if(World::getGround(x, y) != WATER && World::getGround(x, y) != LAKE && World::getGround(x, y) != RIVER)
            {
                bool onOcean = false;
                for(int dir = UP; dir <= RIGHT; dir++)
                {
                    if(World::getGround(getTileInDir(Pos2(x, y), dir)) == WATER)
                    {
                        onOcean = true;
                        break;
                    }
                }
                if(onOcean && RandomUtils::gen() % 3)
                    World::setGround(BEACH, x, y);
                else
                    World::setGround(decideGround(rainMap[x][y], World::getHeight(x, y) > peakAlt, lat), x, y);
            }
        }
    }
    TIMEIT(biomeSmooth(1));
}

void TerrainGen::addRainCircle(Pos2 loc, int r, RainMap &rmap)
{
    for(int i = loc.x - r; i <= loc.x + r; i++)
    {
        for(int j = loc.y - r; j <= loc.y + r; j++)
        {
            if(i >= 0 && i < WORLD_SIZE && j >= 0 && j < WORLD_SIZE)
            {
                if((i - loc.x) * (i - loc.x) + (j - loc.y) * (j - loc.y) <= r * r)
                    rmap[i][j] = true;
            }
        }
    }
}

void TerrainGen::addRandomRain(RainMap& rmap)
{
    Heightmap rain(WORLD_SIZE, WORLD_SIZE);
    rain.diamondSquare(1000, 0, 0, false);
    const int numBands = 20;
    int minVal = SHRT_MAX;
    int maxVal = SHRT_MIN;
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            if(rain.get(i, j) < minVal)
                minVal = rain.get(i, j);
            if(rain.get(i, j) > maxVal)
                maxVal = rain.get(i, j);
        }
    }
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            if(rain.get(i, j) > 0)
                //if((rain.get(i, j) / ((maxVal - minVal) / numBands)) % 2)
                rmap[i][j] = true;
            else
                rmap[i][j] = false;
        }
    }
}

void TerrainGen::addCoastalRain(RainMap& rmap)
{
    const float chance = 0.01;
    for(int i = 0; i < WORLD_SIZE / 3; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            if(isCoastal(Pos2(i, j)) && RandomUtils::genFloat() < chance)
                addRainCircle(Pos2(i, j), RandomUtils::gen() % 30, rmap);
        }
    }
}

void TerrainGen::addRiverRain(RainMap& rmap)
{
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            if(World::getGround(i, j) == RIVER || World::getGround(i, j) == LAKE)
                addRainCircle(Pos2(i, j), 5, rmap);
        }
    }
}

bool TerrainGen::isCoastal(Pos2 loc)
{
    if(World::getGround(loc) == WATER)
        return false;
    for(int dir = UP; dir <= RIGHT; dir++)
    {
        if(World::getGround(getTileInDir(loc, dir)) == WATER)
            return true;
    }
    return false;
}

void TerrainGen::addTropicalRain(int lat, RainMap &rmap)
{
    //Width of band along equator
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = (WORLD_SIZE - lat) / 2; j <= (WORLD_SIZE + lat) / 2; j++)
        {
            rmap[i][j] = true;
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
    Ground* buf = new Ground[WORLD_SIZE * WORLD_SIZE];
    for(int it = 0; it < iters; it++)
    {
        //Smooth buffer based on World ground array
        for(int i = 0; i < WORLD_SIZE; i++)
        {
            for(int j = 0; j < WORLD_SIZE; j++)
            {
                Ground before = World::getGround(i, j);
                if(before == WATER || before == RIVER || before == LAKE)
                    buf[i + j * WORLD_SIZE] = before;
                else
                {
                    int occur[NUM_TYPES];
                    for(int k = 0; k < NUM_TYPES; k++)
                        occur[k] = 0;
                    //Find the most common ground type(s) in a 5x5 grid
                    for(int a = -2; a <= 2; a++)
                    {
                        for(int b = -1; b <= 2; b++)
                        {
                            occur[World::getGround(i + a, j + b)]++;
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
                        buf[i + j * WORLD_SIZE] = maxGrounds[0];
                    else
                        buf[i + j * WORLD_SIZE] = maxGrounds[RandomUtils::gen() % maxGrounds.size()];
                }
            }
        }
        //Copy buffer back to world
        for(int i = 0; i < WORLD_SIZE; i++)
        {
            for(int j = 0; j < WORLD_SIZE; j++)
            {
                World::setGround(buf[i + j * WORLD_SIZE], i, j);
            }
        }
    }
    delete[] buf;
}