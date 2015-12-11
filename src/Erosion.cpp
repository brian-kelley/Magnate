#include "Erosion.h"

using namespace std;
using namespace constants;
using namespace Coord;

static const int NUM_TIMESTEPS = 100;
float Erosion::NORM_TO_H = 0.01; //maps to 0-9 heights

Heightmap getSlope()
{
    Heightmap hm(WORLD_SIZE, WORLD_SIZE);
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            hm.set(i + RandomUtils::gen() % 2, i, j);
        }
    }
    return hm;
}

Heightmap Erosion::erosion()
{
    RandomUtils::seed(0);
    cout << "Starting erosion simulation." << endl;
    //Heightmap world = getSlope();
    Heightmap world = getConeWithEntropy();
    Heightmap rainfall = getRainfall();
    for(int i = 0; i < NUM_TIMESTEPS; i++)
    {
        TIMEIT(timestep(world, rainfall));
    }
    cout << "Done with erosion simulation." << endl;
    return world;
}

void Erosion::timestep(Heightmap &world, Heightmap &rainfall)
{
    Heightmap steepness = getSteepness(world);
    Heightmap vegetation = getVegetation(rainfall, steepness);
    Heightmap loss = getHeightLoss(rainfall, vegetation, steepness);
    Heightmap captureRate = getSoilCaptureRate(vegetation, steepness);
    getActualSoilCapture(world, loss, captureRate);
    world -= loss;
}

Heightmap Erosion::getConeWithEntropy()
{
    const int border = WORLD_SIZE * 0.2;          //how much space to leave around the cone
    const Height peak = WORLD_SIZE;         //max height
    float rad = WORLD_SIZE / 2 - border;
    Heightmap hm(WORLD_SIZE, WORLD_SIZE);
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            int dx = i - WORLD_SIZE / 2;
            int dy = j - WORLD_SIZE / 2;
            float dist = sqrtf(dx * dx + dy * dy);
            if(dist > rad)
                hm.set(0, i, j);                            //sea level if outside radius
            else
            {
                Height coneH = peak * (rad - dist) / rad;
                coneH += (RandomUtils::gen() % 5) - 2;      //add random component
                hm.set(coneH, i, j);    //otherwise heights scale linearly wi8th dist, with a small random translation
            }
        }
    }
    return hm;
}

Heightmap Erosion::getRainfall()
{
    Heightmap hm(WORLD_SIZE, WORLD_SIZE);
    hm.diamondSquare(0, 0, 0, false);
    normalize(hm);
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            hm.set(1000, i, j);
        }
    }
    return hm;
}

Heightmap Erosion::getVegetation(Heightmap &rainfall, Heightmap &steepness)
{
    const float rainWeight = 1.2;
    const float steepWeight = -0.2;         //steep ground has slightly less vegetation
    Heightmap hm(WORLD_SIZE, WORLD_SIZE);
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            hm.set(rainWeight * rainfall.get(i, j) + steepWeight * steepness.get(i, j), i, j);
        }
    }
    normalize(hm);
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            hm.set(1000, i, j);
        }
    }
    return hm;
}

Heightmap Erosion::getSteepness(Heightmap &world)
{
    Heightmap hm(WORLD_SIZE, WORLD_SIZE);
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            //note: NOT gradient, only a measure of average steepness for 4 edges surrounding point
            int sx = (abs(world.get(i + 1, j) - world.get(i, j)) + abs(world.get(i, j) - world.get(i - 1, j))) / 2;
            int sy = (abs(world.get(i, j + 1) - world.get(i, j)) + abs(world.get(i, j) - world.get(i, j - 1))) / 2;
            hm.set(sx + sy, i, j);
        }
    }
    normalize(hm);
    return hm;
}

Heightmap Erosion::getHeightLoss(Heightmap &rainfall, Heightmap &vegetation, Heightmap &steepness)
{
    const float rainWeight = 0.7;       //more rain washes away more material
    const float steepWeight = 0.7;      //steeper ground erodes more
    const float vegWeight = -0.4;       //vegetation prevents some erosion
    Heightmap hm(WORLD_SIZE, WORLD_SIZE);
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            hm.set(rainfall.get(i, j) * rainWeight + vegetation.get(i, j) * vegWeight + steepness.get(i, j) * steepWeight * NORM_TO_H, i, j);
        }
    }
    normalize(hm, 3);
    return hm;
}

Heightmap Erosion::getSoilCaptureRate(Heightmap &vegetation, Heightmap &steepness)
{
    const float vegWeight = 1.4;
    const float steepWeight = -0.4;
    Heightmap hm(WORLD_SIZE, WORLD_SIZE);
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            hm.set(vegetation.get(i, j) * vegWeight + steepness.get(i, j) * steepWeight, i, j);
        }
    }
    normalize(hm);
    return hm;
}

void Erosion::getActualSoilCapture(Heightmap &world, Heightmap heightLoss, Heightmap &captureRate)
{
    auto downhill = getDownhillField(world);
    flow(heightLoss, downhill);
    world += heightLoss;
}

void Erosion::normalize(Heightmap &hm, int range)
{
    short minVal = SHRT_MAX;
    short maxVal = SHRT_MIN;
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            if(hm.get(i, j) < minVal)
                minVal = hm.get(i, j);
            if(hm.get(i, j) > maxVal)
                maxVal = hm.get(i, j);
        }
    }
    cout << "Range for normalization is " << minVal << " to " << maxVal << endl;
    float scale = double(range) / (maxVal - minVal);
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            hm.set((hm.get(i, j) - minVal) * scale, i, j);
        }
    }
}

void Erosion::addChanges(Heightmap &world, Heightmap &loss, Heightmap &capture)
{
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            //loss is still [0, 1000) but capture is actually height values
            world.set(world.get(i, j) - loss.get(i, j) + capture.get(i, j), i, j);
            if(world.get(i, j) < 0)
                world.set(0, i, j);
        }
    }
}

VecField Erosion::getDownhillField(Heightmap &world)
{
    VecField vf(WORLD_SIZE, WORLD_SIZE);
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            Pos2 here(i, j);
            int lowestDir;
            Height lowestH = world.get(i, j);
            //Get lowest neighbor, compare to this height
            for(int dir = UP; dir <= RIGHT; dir++)
            {
                Height neiH = world.get(getTileInDir(here, dir));
                if(neiH < lowestH)
                {
                    lowestH = neiH;
                    lowestDir = dir;
                }
            }
            if(lowestH == world.get(here))
            {
                //No lower neighbor, downhill vector immediately 0 in both directions
                vf.set(SmallVec(0, 0), i, j);
            }
            else
            {
                //Otherwise, determine if a (perpendicular) component should gain as well
                Height nextLowest = world.get(here);
                int secondaryDir;
                for(int dir = UP; dir <= RIGHT; dir++)
                {
                    //Don't double-count the original lowest neighbor
                    if(dir == lowestDir)
                        continue;
                    Height neiH = world.get(getTileInDir(here, dir));
                    if(neiH < nextLowest)
                    {
                        nextLowest = neiH;
                        secondaryDir = dir;
                    }
                }
                if(nextLowest == world.get(here))
                {
                    //No secondary neighbor, give entire vector to main downhill direction
                    vf.set(SmallVec(lowestDir, 100), here);
                }
                else
                {
                    //Has a secondary neighbor with a lesser component
                    //Have the main component be 100, and the perpendicular component be less
                    //proportionally to the height difference in world
                    Pos2 secondary = getTileInDir(here, secondaryDir);
                    int mainDiff = world.get(here) - lowestH;
                    int perpDiff = world.get(here) - world.get(secondary);
                    vf.set(SmallVec(lowestDir, 100, secondaryDir, 100 * float(perpDiff) / mainDiff), secondary);
                }
            }
        }
    }
    return vf;
}

void Erosion::flow(Heightmap& hm, VecField &vf)
{
    const int w = hm.getW();
    const int h = hm.getH();
    //Find the longest vector in vf
    int bestMag = 0;
    for(int i = 0; i < w; i++)
    {
        for(int j = 0; j < h; j++)
        {
            auto vec = vf.get(i, j);
            int thisMag = int(vec.x) * vec.x + int(vec.y) * vec.y;
            if(thisMag > bestMag)
                bestMag = thisMag;
        }
    }
    bestMag = sqrt(bestMag);
    Heightmap dest(w, h);   //Make an empty heightmap, place values here
    for(int i = 0; i < w; i++)
    {
        for(int j = 0; j < h; j++)
        {
            short val = hm.get(i, j);
            auto vec = vf.get(i, j);
            //Get x and y components of gradient, but reverse so they point downhill
            int xcomp = -vec.x;
            int ycomp = -vec.y;
            //Direction doesn't matter if component is 0, corresponds to no movement in that direction
            Direction xdir = xcomp <= 0 ? LEFT : RIGHT;
            Direction ydir = ycomp <= 0 ? UP : DOWN;
            xcomp = abs(xcomp);
            ycomp = abs(ycomp);
            //distribute value proportionally to magnitude of vector components
            int xdist = val * xcomp / float(xcomp + ycomp);
            int ydist = val * ycomp / float(xcomp + ycomp);
            if(xdir == LEFT)
                dest.add(xdist, i - 1, j);
            else
                dest.add(xdist, i + 1, j);
            if(ydir == UP)
                dest.add(ydist, i, j - 1);
            else
                dest.add(ydist, i, j + 1);
        }
    }
    hm = dest;
}