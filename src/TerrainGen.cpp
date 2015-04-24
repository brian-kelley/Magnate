#include "TerrainGen.h"

using namespace std;

void TerrainGen::generate(mesh_t& dest, int i, int j)
{
    //i, j are signed, so add half of INT_MAX to them to constrain them to signed
    unsigned int si = (unsigned int) (i + INT_MAX / 2);
    unsigned int sj = (unsigned int) (j + INT_MAX / 2);
    //Make sure the RNG behaves exactly the same, every ti e this function is run
    //TODO: to make sure consistent behavior across platforms, use a boost::random prng instead?
    //so, chunks get repeated every 65536 in any direction, this is OK
    srandom(((si & 0xFFFF) << 16) | (sj & 0xFFFF));
    //For now, fill all with mountains to just look at form
    for(int x = 0; x < CHUNK_SIZE; x++)
    {
        for(int y = 0; y < CHUNK_SIZE; y++)
        {
            dest[x][y].g = MOUNTAINS;
        }
    }
    //Use diamond-square to assign initial rough heightmap, ground types later
    //Could try to use this to match up with neighboring chunks, or seed for a certain kind of terrain in the chunk (flat, mountainous, sloping)
    int size = CHUNK_SIZE - 1;
    while(size > 1)
    {
        for(int i = size / 2; i < CHUNK_SIZE; i += size)
        {
            for(int j = size / 2; j < CHUNK_SIZE; j += size)
            {
                if(i % size != 0 || j % size != 0)
                    square(dest, i, j, size);
                if(size > 2)
                    diamond(dest, i, j, size);
            }
        }
        size /= 2;
    }
}

void TerrainGen::square(mesh_t &dest, int x, int y, int size)
{
    //cout << "Doing square @ " << size << " and (" << x << "," << y << ")\n";
    //If the size of the shape is 1, we're done. If smaller, something's wrong but return anyway.
    if(size <= 1)
        return;
    int sum = 0;
    int n = 0;
    if(inMesh(x, y - size / 2))
    {
        sum += dest[x][y - size / 2].height;
        n++;
    }
    if(inMesh(x + size / 2, y))
    {
        sum += dest[x + size / 2][y].height;
        n++;
    }
    if(inMesh(x, y + size / 2))
    {
        sum += dest[x][y + size / 2].height;
        n++;
    }
    if(inMesh(x - size / 2, y))
    {
        sum += dest[x - size / 2][y].height;
        n++;
    }
    dest[x][y].height = getHeight(sum / n, size);
    dest[x][y].g = DESERT;
}
void TerrainGen::diamond(mesh_t &dest, int x, int y, int size)
{
    //cout << "Doing diamond @ " << size << " and (" << x << "," << y << ")\n";
    if(size <= 1)
        return;
    int sum = 0;
    int n = 0;
    if(inMesh(x - size / 2, y - size / 2))
    {
        sum += dest[x - size / 2][y - size / 2].height;
        n++;
    }
    if(inMesh(x + size / 2, y - size / 2))
    {
        sum += dest[x + size / 2][y - size / 2].height;
        n++;
    }
    if(inMesh(x + size / 2, y + size / 2))
    {
        sum += dest[x + size / 2][y + size / 2].height;
        n++;
    }
    if(inMesh(x - size / 2, y + size / 2))
    {
        sum += dest[x - size / 2][y + size / 2].height;
        n++;
    }
    dest[x][y].height = getHeight(sum / n, size);
    dest[x][y].g = DESERT;
}

bool TerrainGen::inMesh(int x, int y)
{
    if(x >= 0 && x < CHUNK_SIZE && y >= 0 && y < CHUNK_SIZE)
    {
        return true;
    }
    else
    {
        return false;
    }
}

unsigned char TerrainGen::getHeight(int avg, int size)
{
    /*
    if(size == CHUNK_SIZE - 1)
    {
        //For now, establish the first five major points with full range
        return 128;
    }
    int range = float(size) / (CHUNK_SIZE - 1) * 255 * ROUGHNESS;
    //Set range to 1 if it's 0, to avoid div by zero error
    if(avg + range > 255)
        range = 255 - avg;
    if(avg - range < 0)
        range = avg;
    if(range == 0)
        range = 1;
    int result = (avg + (random() % range) - (range / 2));
    result &= 0xFF;
    //cout << "Height is " << result << " for size " << size << " and avg " << avg << " and range " << range << endl;
    return result;
     */
    return 0;
}