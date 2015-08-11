//
//  WorldRenderer.cpp
//  Magnate
//
//  Created by Brian Kelley on 3/9/1568.
//
//

#include "WorldRenderer.h"

using namespace std;
using namespace constants;
using namespace Coord;
using namespace Renderer;
using namespace glm;

//4 vertices, each with 2 floats (u, v)
unsigned short terrainUV[GROUND::NUM_TYPES * 4 * 2];
//list of chunks currently cached in VBO, mapped to the actual vbo
Pos2 WorldRenderer::chunkAlloc[VBO_CHUNKS];
Quad* WorldRenderer::vboScratchBuf = NULL;
Pos2 WorldRenderer::centerChunk;

bool ptInScreen(Point p);
bool chunkInWorld(int x, int y);

void WorldRenderer::init()
{
    //Init fast array of float rect stuff
    //Ideally this will iterate through every ground type (hopefully)
    int i = 0;
    for(GROUND g = (GROUND) 0; g < NUM_TYPES; g = (GROUND) ((int) g + 1))
    {
        int terrainTexID = Terrain::terrainTextures[g];
        floatRect_t uvrect = RenderRoutines::getTexCoords(terrainTexID);
        float* rectVal = (float*) &uvrect;
        for(int j = 0; j < 4; j++)
        {
            *rectVal *= ATLAS_SIZE;
            rectVal++;
        }
        terrainUV[8 * i + 0] = uvrect.x;
        terrainUV[8 * i + 1] = uvrect.y;
        terrainUV[8 * i + 2] = uvrect.x + uvrect.w;
        terrainUV[8 * i + 3] = uvrect.y;
        terrainUV[8 * i + 4] = uvrect.x + uvrect.w;
        terrainUV[8 * i + 5] = uvrect.y + uvrect.h;
        terrainUV[8 * i + 6] = uvrect.x;
        terrainUV[8 * i + 7] = uvrect.y + uvrect.h;
        i++;
    }
    vboScratchBuf = (Quad*) malloc(VBO_BYTES_PER_CHUNK);
    //allocate initial set of chunks
    //first, find intersection of camDir with y = 0 plane
    calcCenterChunk();
    const int allocatedAreaLength = sqrt(VBO_CHUNKS);
    int minChunkX = centerChunk.x - (allocatedAreaLength - 1) / 2;
    int maxChunkX = centerChunk.x + (allocatedAreaLength - 1) / 2;
    int minChunkY = centerChunk.y - (allocatedAreaLength - 1) / 2;
    int maxChunkY = centerChunk.y + (allocatedAreaLength - 1) / 2;
    for(int i = minChunkX; i <= maxChunkX; i++)
    {
        for(int j = minChunkY; j <= maxChunkY; j++)
        {
            allocChunk(i, j);
        }
    }
}

void WorldRenderer::dispose()
{
    delete[] vboScratchBuf;
}

Pos3 getTileVertexPos(int chunkX, int chunkZ, int tileX, int tileZ)
{
    Pos3 pos;
    pos.x = TERRAIN_TILE_SIZE * (chunkX * CHUNK_SIZE + tileX);
    pos.y = TERRAIN_Y_SCALE * World::getHeight(chunkX * CHUNK_SIZE + tileX, chunkZ * CHUNK_SIZE + tileZ);
    pos.z = TERRAIN_TILE_SIZE * (chunkZ * CHUNK_SIZE + tileZ);
    return pos;
}

void WorldRenderer::setTexCoords(Quad *q, GROUND ground)
{
    //TODO: modify when doing multiple variants of a texture
    int g = (int) ground;
    q->p1.texcoord = {terrainUV[g * 8 + 0], terrainUV[g * 8 + 1]};
    q->p2.texcoord = {terrainUV[g * 8 + 2], terrainUV[g * 8 + 3]};
    q->p3.texcoord = {terrainUV[g * 8 + 4], terrainUV[g * 8 + 5]};
    q->p4.texcoord = {terrainUV[g * 8 + 6], terrainUV[g * 8 + 7]};
}

//set attributes for the 4 vertices of a tile
void WorldRenderer::getTileQuad(Quad* q, int x, int z)
{
    Color4 color = {0xFF, 0xFF, 0xFF, 0xFF};
    setTexCoords(q, World::getGround(x, z));
    q->p1.pos = tileToWorld(x, World::getHeight(x, z), z);
    q->p1.color = color;
    q->p2.pos = tileToWorld(x, World::getHeight(x, z + 1), z + 1);
    q->p2.color = color;
    q->p3.pos = tileToWorld(x + 1, World::getHeight(x + 1, z + 1), z + 1);
    q->p3.color = color;
    q->p4.pos = tileToWorld(x + 1, World::getHeight(x + 1, z), z);
    q->p4.color = color;
}

bool WorldRenderer::allocChunk(int x, int z)
{
    int allocTarget = -1;
    for(int i = 0; i < VBO_CHUNKS; i++)
    {
        if(chunkAlloc[i].x == CHUNK_FREE)
        {
            allocTarget = i;
            break;
        }
    }
    if(allocTarget == -1)
        return false;
    int chunkXOffset = CHUNK_SIZE * x;
    int chunkZOffset = CHUNK_SIZE * z;
    //Populate scratch space with quads for requested chunk
    for(int i = 0; i < CHUNK_SIZE; i++) //tiles by x
    {
        for(int j = 0; j < CHUNK_SIZE; j++) //tiles by z
        {
            getTileQuad(&vboScratchBuf[j * CHUNK_SIZE + i], chunkXOffset + i, chunkZOffset + j);
        }
    }
    //mark slot in chunkAlloc as filled
    chunkAlloc[allocTarget].x = x;
    chunkAlloc[allocTarget].y = z;
    //copy quads to terrain VBO
    //must already be bound!
    glBufferSubData(GL_ARRAY_BUFFER, allocTarget * VBO_BYTES_PER_CHUNK, VBO_BYTES_PER_CHUNK, (GLvoid*) vboScratchBuf);
    return true;
}

bool WorldRenderer::freeChunk(int x, int z)
{
    for(int i = 0; i < VBO_CHUNKS; i++)
    {
        if(chunkAlloc[i].x == x && chunkAlloc[i].y == z)
        {
            chunkAlloc[i] = {CHUNK_FREE, 0};
            return true;
        }
    }
    return false;
}

bool WorldRenderer::isChunkAllocated(int x, int z)
{
    for(int i = 0; i < VBO_CHUNKS; i++)
    {
        if(chunkAlloc[i].x == x && chunkAlloc[i].y == z)
            return true;
    }
    return false;
}

void WorldRenderer::calcCenterChunk()
{
    vec2 camCenter;
    float camDirMult = camPos.y / camDir.y;
    camCenter.x = camPos.x + camDir.x * camDirMult;
    camCenter.y = camPos.z + camDir.z * camDirMult;
    const float CHUNK_LENGTH = TERRAIN_TILE_SIZE * CHUNK_SIZE;
    centerChunk.x = camCenter.x / CHUNK_LENGTH;
    centerChunk.y = camCenter.y / CHUNK_LENGTH;
}

void WorldRenderer::updateVBOChunks()
{
    //save old center chunk - if it didn't actually change, don't do anything
    Pos2 oldCenter = centerChunk;
    calcCenterChunk();
    if(oldCenter.x == centerChunk.x || oldCenter.y == centerChunk.y)
        return;
    //go through the currently allocated chunks and free if they are no longer in the visible square
    int dist = (int(sqrt(VBO_CHUNKS)) - 1) / 2; //maximum distance from centerChunk in x or z
    for(int i = 0; i < VBO_CHUNKS; i++)
    {
        if(abs(chunkAlloc[i].x - centerChunk.x) > dist || abs(chunkAlloc[i].y - centerChunk.y) > dist)
        {
            chunkAlloc[i] = {CHUNK_FREE, 0};
        }
    }
    //now go through all chunks that need to be in the vbo and allocate them if not already there
    for(int i = centerChunk.x - dist; i <= centerChunk.x + dist; i++)
    {
        for(int j = centerChunk.y - dist; j <= centerChunk.y + dist; j++)
        {
            if(!isChunkAllocated(i, j))
            {
                allocChunk(i, j);
            }
        }
    }
}

void WorldRenderer::camRotateLeft()
{
    camDir = rotate(camDir, CAM_ROTATE_SPEED, {0, 1, 0});
}

void WorldRenderer::camRotateRight()
{
    camDir = rotate(camDir, -CAM_ROTATE_SPEED, {0, 1, 0});
}

void WorldRenderer::camForward()
{
    //get y component out of camDir
    vec2 lookDir = normalize(vec2(camDir.x, camDir.z));
    lookDir *= PAN_SPEED;
    camPos.x += lookDir.x;
    camPos.z += lookDir.y;
}

void WorldRenderer::camBackward()
{
    vec2 lookDir = normalize(vec2(camDir.x, camDir.z));
    lookDir *= -PAN_SPEED;
    camPos.x += lookDir.x;
    camPos.z += lookDir.y;
}

void WorldRenderer::camLeft()
{
    vec2 lookDir = normalize(vec2(camDir.x, camDir.z));
    //rotate pi/2 to the left
    lookDir = {-lookDir.y, lookDir.x};
    lookDir *= PAN_SPEED;
    camPos.x += lookDir.x;
    camPos.z += lookDir.y;
}

void WorldRenderer::camRight()
{
    vec2 lookDir = normalize(vec2(camDir.x, camDir.z));
    lookDir = {lookDir.y, -lookDir.x};
    lookDir *= PAN_SPEED;
    camPos.x += lookDir.x;
    camPos.z += lookDir.y;
}

bool ptInScreen(Point p)
{
    if(p.x >= 0 && p.x < WINDOW_W && p.y >= 0 && p.y < WINDOW_H)
        return true;
    return false;
}

bool chunkInWorld(int x, int y)
{
    if(x >= 0 && x < WORLD_CHUNKS && y >= 0 && y < WORLD_CHUNKS)
        return true;
    return false;
}