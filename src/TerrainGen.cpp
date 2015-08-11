#include "TerrainGen.h"

using namespace std;
using namespace constants;

QTNode* QTNode::treeRoot = NULL;

QTNode::QTNode(float h, float x, float z, float size, QTNode* master)
{
    this->h = h;
    this->x = x;
    this->z = z;
    this->size = size;
    this->master = master;
}

bool QTNode::isRoot()
{
    return master == NULL;
}

QTNode* QTNode::find(float xo, float zo)
{
    if(xo < x + size / 2)
    {
        if(zo < z + size / 2)
        {
            if(n1)
                return n1->find(xo, zo);
            else
                return this;
        }
        else
        {
            if(n2)
                return n2->find(xo, zo);
            else
                return this;
        }
    }
    else
    {
        if(zo < z + size / 2)
        {
            if(n4)
                return n4->find(xo, zo);
            else
                return this;
        }
        else
        {
            if(n3)
                return n3->find(xo, zo);
            else
                return this;
        }
    }
}

QTNode* QTNode::up()
{
    //Figure out where I am in master
    float targetX = x + size / 2; //center of square above this one
    float targetZ = z - size / 2;
    return treeRoot->findWithSize(targetX, targetZ, size);
}

QTNode* QTNode::left()
{
    float targetX = x - size / 2;
    float targetZ = z + size / 2;
    return treeRoot->findWithSize(targetX, targetZ, size);
}

QTNode* QTNode::right()
{
    float targetX = x + size * 3 / 2;
    float targetZ = z + size / 2;
    return treeRoot->findWithSize(targetX, targetZ, size);
}

QTNode* QTNode::down()
{
    float targetX = x + size / 2;
    float targetZ = z + size * 3 / 2;
    return treeRoot->findWithSize(targetX, targetZ, size);
}

int QTNode::getDepth()
{
    int depth = 0;
    QTNode* iter = this;
    while(iter)
    {
        depth++;
        iter = iter->master;
    }
    return depth;
}

QTNode* QTNode::findWithSize(float xo, float zo, float searchSize)
{
    bool thisMatch = searchSize == size / 2;
    if(xo < x + size / 2)
    {
        //left of center
        if(zo < z + size / 2)
        {
            //up-left
            if(thisMatch)
                return n1;
            else
                return n1->findWithSize(xo, zo, searchSize);
        }
        else
        {
            //lower-left
            if(thisMatch)
                return n4;
            else
                return n4->findWithSize(xo, zo, searchSize);
        }
    }
    else
    {
        //right of center
        if(zo < z + size / 2)
        {
            //up-right
            if(thisMatch)
                return n2;
            else
                return n2->findWithSize(xo, zo, searchSize);
        }
        else
        {
            //lower-right
            if(thisMatch)
                return n3;
            else
                return n3->findWithSize(xo, zo, searchSize);
        }
    }
    return NULL;
}

void TerrainGen::generate()
{
    QTNode::treeRoot = new QTNode(0, 0, 0, WORLD_SIZE, NULL);
    diamondSquare();
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            World::setGround(FOREST, i, j);
            if(World::getHeight(i, j) == 0)
            {
                World::setGround(WATER, i, j);
            }
            else if(World::getHeight(i, j) > 200)
            {
                World::setGround(MOUNTAINS, i, j);
            }
        }
    }
    int markerX = random() % WORLD_SIZE / 2;
    int markerY = random() % WORLD_SIZE / 2;
    for(int i = markerX - 8; i < markerX + 8; i++)
    {
        for(int j = markerY - 8; j < markerY + 8; j++)
        {
            World::setGround(DESERT, i, j);
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

void TerrainGen::diamondSquare()
{
    World::setHeight(50, 0, 0);
    World::setHeight(50, WORLD_SIZE - 1, 0);
    World::setHeight(50, 0, WORLD_SIZE - 1);
    World::setHeight(50, WORLD_SIZE - 1, WORLD_SIZE - 1);
    //First diamond (puts pt at very center of chunk)
    World::setHeight(255, WORLD_SIZE / 2, WORLD_SIZE / 2);
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
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            int height = World::getHeight(i, j);
            int centerDist = abs(WORLD_SIZE / 2 - i) + abs(WORLD_SIZE / 2 - j);
            height -= centerDist / 5;
            if(height < 0)
                height = 0;
            World::setHeight(height, i, j);
        }
    }
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            if(World::getGround(i, j) != WATER)
            {
                if(World::getGround(i + 1, j) == WATER && World::getGround(i, j + 1) == WATER && World::getGround(i - 1, j) == WATER && World::getGround(i, j - 1) == WATER)
                {
                    World::setGround(WATER, i, j);
                    World::setHeight(0, i, j);
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
        sum += World::getHeight(x - size / 2, y);
    }
    if(inMesh(x + size / 2, y))
    {
        n++;
        sum += World::getHeight(x + size / 2, y);
    }
    if(inMesh(x, y - size / 2))
    {
        n++;
        sum += World::getHeight(x, y - size / 2);
    }
    if(inMesh(x, y + size / 2))
    {
        n++;
        sum += World::getHeight(x, y + size / 2);
    }
    if(n == 0)
        return;
    World::setHeight(World::getHeight(sum / n, size), x, y);
    World::setGround(DESERT, x, y);
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
        sum += World::getHeight(x - size / 2, y - size / 2);
    }
    if(inMesh(x - size / 2, y + size / 2))
    {
        n++;
        sum += World::getHeight(x - size / 2, y + size / 2);
    }
    if(inMesh(x + size / 2, y + size / 2))
    {
        n++;
        sum += World::getHeight(x + size / 2, y + size / 2);
    }
    if(inMesh(x + size / 2, y - size / 2))
    {
        n++;
        sum += World::getHeight(x + size / 2, y - size / 2);
    }
    //Put in height for center of square
    if(n == 0)
        return;
    World::setHeight(getHeight(sum / n, size), x, y);
    World::setGround(DESERT, x, y);
}