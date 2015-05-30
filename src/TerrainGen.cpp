#include "TerrainGen.h"

using namespace std;
using namespace constants;

void TerrainGen::generate(World& world)
{
    diamondSquare(world);
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            world.setGround(FOREST, i, j);
            if(world.getHeight(i, j) == 0)
            {
                world.setGround(WATER, i, j);
            }
            else if(world.getHeight(i, j) > 200)
            {
                world.setGround(MOUNTAINS, i, j);
            }
        }
    }
    int markerX = random() % WORLD_SIZE / 2;
    int markerY = random() % WORLD_SIZE / 2;
    for(int i = markerX - 8; i < markerX + 8; i++)
    {
        for(int j = markerY - 8; j < markerY + 8; j++)
        {
            world.setGround(DESERT, i, j);
        }
    }
}

//Generates height based on average surrounding height, and how fine grid is
Height TerrainGen::getHeight(int avg, int size)
{
    int range = double(size) / (WORLD_SIZE - 1) * 255 * ROUGHNESS;
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

void TerrainGen::diamondSquare(World& world)
{
    world.setHeight(50, 0, 0);
    world.setHeight(50, WORLD_SIZE - 1, 0);
    world.setHeight(50, 0, WORLD_SIZE - 1);
    world.setHeight(50, WORLD_SIZE - 1, WORLD_SIZE - 1);
    //First diamond (puts pt at very center of chunk)
    world.setHeight(255, WORLD_SIZE / 2, WORLD_SIZE / 2);
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
                    fillDiamond(world, i, j, size * 2);
                }
            }
        }
        for(int i = size / 2; i < WORLD_SIZE; i += size)
        {
            for(int j = size / 2; j < WORLD_SIZE; j += size)
            {
                fillSquare(world, i, j, size);
            }
        }
        size /= 2;
    }
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            int height = world.getHeight(i, j);
            int centerDist = abs(WORLD_SIZE / 2 - i) + abs(WORLD_SIZE / 2 - j);
            height -= centerDist / 5;
            if(height < 0)
                height = 0;
            world.setHeight(height, i, j);
        }
    }
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            if(world.getGround(i, j) != WATER)
            {
                if(world.getGround(i + 1, j) == WATER && world.getGround(i, j + 1) == WATER && world.getGround(i - 1, j) == WATER && world.getGround(i, j - 1) == WATER)
                {
                    world.setGround(WATER, i, j);
                    world.setHeight(0, i, j);
                }
            }
        }
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

void TerrainGen::fillDiamond(World& world, int x, int y, int size)
{
    //place point at center of a diamond (square corner)
    //size = total width/height of the diamond being filled in
    int sum = 0;
    int n = 0;
    //cout << "Filling in diamond @ " << x << "," << y << " with size " << size << endl;
    if(inMesh(x - size / 2, y))
    {
        n++;
        sum += world.getHeight(x - size / 2, y);
    }
    if(inMesh(x + size / 2, y))
    {
        n++;
        sum += world.getHeight(x + size / 2, y);
    }
    if(inMesh(x, y - size / 2))
    {
        n++;
        sum += world.getHeight(x, y - size / 2);
    }
    if(inMesh(x, y + size / 2))
    {
        n++;
        sum += world.getHeight(x, y + size / 2);
    }
    if(n == 0)
        return;
    world.setHeight(getHeight(sum / n, size), x, y);
    world.setGround(DESERT, x, y);
}

void TerrainGen::fillSquare(World& world, int x, int y, int size)
{
    int sum = 0;
    int n = 0;
    //cout << "Filling in square @ " << x << "," << y << " with size " << size << endl;
    //sometimes (x,y) is right on boundary of chunk so we have to check this
    //size is the side length of square being filled in
    if(inMesh(x - size / 2, y - size / 2))
    {
        n++;
        sum += world.getHeight(x - size / 2, y - size / 2);
    }
    if(inMesh(x - size / 2, y + size / 2))
    {
        n++;
        sum += world.getHeight(x - size / 2, y + size / 2);
    }
    if(inMesh(x + size / 2, y + size / 2))
    {
        n++;
        sum += world.getHeight(x + size / 2, y + size / 2);
    }
    if(inMesh(x + size / 2, y - size / 2))
    {
        n++;
        sum += world.getHeight(x + size / 2, y - size / 2);
    }
    //Put in height for center of square
    if(n == 0)
        return;
    world.setHeight(getHeight(sum / n, size), x, y);
    world.setGround(DESERT, x, y);
}