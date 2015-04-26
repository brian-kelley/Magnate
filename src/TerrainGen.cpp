#include "TerrainGen.h"

using namespace std;

void TerrainGen::generate(mesh_t& dest, int i, int j)
{
    unsigned int si = (unsigned int) (i + INT_MAX / 2);
    unsigned int sj = (unsigned int) (j + INT_MAX / 2);
    srandom(((si & 0xFFFF) << 16) | (sj & 0xFFFF));
    for(int i = 0; i < CHUNK_SIZE; i++)
    {
        for(int j = 0; j < CHUNK_SIZE; j++)
        {
            dest[i][j].g = MOUNTAINS;
            dest[i][j].height = 0;
        }
    }
    diamondSquare(dest);
}

//Generates height based on average surrounding height, and how fine grid is
unsigned char TerrainGen::getHeight(int avg, int size)
{
    int range = double(size) / (CHUNK_SIZE - 1) * 255 * ROUGHNESS;
    if(range == 0)
        return avg;
    int result = (avg + (random() % range) - (range / 2));
    if(result < 0)
        result = 0;
    if(result > 255)
        result = 255;
    //cout << "Height is " << result << " for size " << size << " and avg " << avg << " and range " << range << endl;
    return result;
}

void TerrainGen::diamondSquare(mesh_t& dest)
{
    //Manually create the first square, with each corner
    dest[0][0].height = 128;
    dest[CHUNK_SIZE - 1][0].height = 128;
    dest[0][CHUNK_SIZE - 1].height = 128;
    dest[CHUNK_SIZE - 1][CHUNK_SIZE - 1].height = 128;
    //First diamond (puts pt at very center of chunk)
    fillSquare(dest, (CHUNK_SIZE - 1) / 2, (CHUNK_SIZE - 1) / 2, CHUNK_SIZE - 1);
    //repeatedly go over mesh and do these steps, making grid progressively finer
    //(squares first, then diamonds)
    //Note: if squares then diamonds, same size can be used each iteration
    int size = (CHUNK_SIZE - 1) / 2;
    while(size > 0)
    {
        //Fill in the diamonds
        //Iterate over all grid points, check if needs a diamond fill-in
        for(int i = 0; i < CHUNK_SIZE; i += size)
        {
            for(int j = 0; j < CHUNK_SIZE; j += size)
            {
                if((i + j) % (size * 2) != 0)
                {
                    fillDiamond(dest, i, j, size * 2);
                }
            }
        }
        for(int i = size / 2; i < CHUNK_SIZE; i += size)
        {
            for(int j = size / 2; j < CHUNK_SIZE; j += size)
            {
                fillSquare(dest, i, j, size);
            }
        }
        size /= 2;
    }
}

void TerrainGen::diamond(mesh_t &dest, int x, int y, int size)
{
    //this finds the value at x, y such that x, y are at the center
    //of a square with size 'size' and adding the point creates diamonds
    int sum = 0;
    int n = 0;
    //sometimes (x,y) is right on boundary of chunk so we have to check this
    if(inMesh(x - size / 2, y - size / 2))
    {
        n++;
        sum += (int) dest[x - size / 2][y - size / 2].height;
    }
    if(inMesh(x - size / 2, y + size / 2))
    {
        n++;
        sum += dest[x - size / 2][y + size / 2].height;
    }
    if(inMesh(x + size / 2, y + size / 2))
    {
        n++;
        sum += dest[x + size / 2][y + size / 2].height;
    }
    if(inMesh(x + size / 2, y - size / 2))
    {
        n++;
        sum += dest[x + size / 2][y - size / 2].height;
    }
    //Put in height for center of square
    if(n == 0)
        return;
    dest[x][y].height = getHeight(sum / n, size);
    dest[x][y].g = DESERT;
}

void TerrainGen::square(mesh_t &dest, int x, int y, int size)
{
    //Square: Find height at center of diamond to create squares
    int sum = 0;
    int n = 0;
    if(inMesh(x - size / 2, y))
    {
        n++;
        sum += dest[x - size / 2][y].height;
    }
    if(inMesh(x + size / 2, y))
    {
        n++;
        sum += dest[x + size / 2][y].height;
    }
    if(inMesh(x, y - size / 2))
    {
        n++;
        sum += dest[x][y - size / 2].height;
    }
    if(inMesh(x, y + size / 2))
    {
        n++;
        sum += dest[x][y + size / 2].height;
    }
    if(n == 0)
        return;
    dest[x][y].height = getHeight(sum / n, size);
    dest[x][y].g = DESERT;
}

bool TerrainGen::inMesh(int x, int y)
{
    if(x > 0 && x < CHUNK_SIZE && y > 0 && y < CHUNK_SIZE)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void TerrainGen::fillDiamond(mesh_t &dest, int x, int y, int size)
{
    //place point at center of a diamond (square corner)
    //size = total width/height of the diamond being filled in
    int sum = 0;
    int n = 0;
    //cout << "Filling in diamond @ " << x << "," << y << " with size " << size << endl;
    if(inMesh(x - size / 2, y))
    {
        n++;
        sum += (int) dest[x - size / 2][y].height;
        //cout << "Using point " << x - size / 2 << "," << y << endl;
    }
    if(inMesh(x + size / 2, y))
    {
        n++;
        sum += (int) dest[x + size / 2][y].height;
        //cout << "Using point " << x + size / 2 << "," << y << endl;
    }
    if(inMesh(x, y - size / 2))
    {
        n++;
        sum += (int) dest[x][y - size / 2].height;
        //cout << "Using point " << x << "," << y - size / 2 << endl;
    }
    if(inMesh(x, y + size / 2))
    {
        n++;
        sum += (int) dest[x][y + size / 2].height;
        //cout << "Using point " << x << "," << y + size / 2 << endl;
    }
    if(n == 0)
        return;
    dest[x][y].height = getHeight(sum / n, size);
    dest[x][y].g = DESERT;
}

void TerrainGen::fillSquare(mesh_t &dest, int x, int y, int size)
{
    int sum = 0;
    int n = 0;
    //cout << "Filling in square @ " << x << "," << y << " with size " << size << endl;
    //sometimes (x,y) is right on boundary of chunk so we have to check this
    //size is the side length of square being filled in
    if(inMesh(x - size / 2, y - size / 2))
    {
        n++;
        sum += (int) dest[x - size / 2][y - size / 2].height;
    }
    if(inMesh(x - size / 2, y + size / 2))
    {
        n++;
        sum += (int) dest[x - size / 2][y + size / 2].height;
    }
    if(inMesh(x + size / 2, y + size / 2))
    {
        n++;
        sum += (int) dest[x + size / 2][y + size / 2].height;
    }
    if(inMesh(x + size / 2, y - size / 2))
    {
        n++;
        sum += (int) dest[x + size / 2][y - size / 2].height;
    }
    //Put in height for center of square
    if(n == 0)
        return;
    dest[x][y].height = getHeight(sum / n, size);
    dest[x][y].g = DESERT;
}