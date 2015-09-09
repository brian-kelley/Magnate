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
short terrainUV[GROUND::NUM_TYPES * 4 * 2];
//list of chunks currently cached in VBO, mapped to the actual vbo
Pos2 WorldRenderer::chunkAlloc[VBO_CHUNKS];
Quad* WorldRenderer::vboScratchBuf = NULL;
Pos2 WorldRenderer::centerChunk;

bool ptInScreen(Point p);
bool chunkInWorld(int x, int y);

void WorldRenderer::init()
{
    using namespace Atlas;
    //Init fast-access array of terrain texcoords
    int i = 0;
    for(GROUND g = (GROUND) 0; g < NUM_TYPES; g = (GROUND) ((int) g + 1))
    {
        int terrainTexID = Terrain::terrainTextures[g];
        intRect_t uvrect;
        uvrect.x = tileX(terrainTexID);
        uvrect.y = tileY(terrainTexID);
        uvrect.w = tileW(terrainTexID);
        uvrect.h = tileH(terrainTexID);
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
    //mark all vbo chunk slots as free
    for(int i = 0; i < VBO_CHUNKS; i++)
    {
        chunkAlloc[i].x = CHUNK_FREE;
    }
    updateVBOChunks(true);
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
    //get positions
    q->p1.pos = vec3(tileToWorld(x, World::getHeight(x, z), z));
    q->p2.pos = vec3(tileToWorld(x + 1, World::getHeight(x + 1, z), z));
    q->p3.pos = vec3(tileToWorld(x + 1, World::getHeight(x + 1, z + 1), z + 1));
    q->p4.pos = vec3(tileToWorld(x, World::getHeight(x, z + 1), z + 1));
    //get normal to produce color (on CPU)
    vec3 normal = normalize(cross(q->p2.pos - q->p4.pos, q->p1.pos - q->p3.pos));
    float diffuse = dot(normal, sunlight);
    if(diffuse < 0)
        diffuse = 0;
    float light = diffuse * diffuseWeight + ambientWeight;
    Color4 color = {(unsigned char)(light * 255), (unsigned char)(light * 255), (unsigned char)(light * 255), 255};
    setTexCoords(q, World::getGround(x, z));
    q->p1.color = color;
    q->p2.color = color;
    q->p3.color = color;
    q->p4.color = color;
}

bool WorldRenderer::allocChunk(int x, int z)
{
    //PRINT("Allocating chunk at " << x << ", " << z)
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
    //trace ray @ center of view, find intersection with y=0 plane
    vec4 back = {0, 0, FAR, 1};
    vec4 front = {0, 0, NEAR, 1};
    //reverse translate points through projection, then view
    mat4 projInv = inverse(proj3);
    mat4 viewInv = inverse(view3);
    back = projInv * back;
    back /= back.w;
    front = projInv * front;
    front /= front.w;
    back = viewInv * back;
    front = viewInv * front;
    //"front" will have a positive y component (above ground)
    vec4 dir = back - front;
    float mult = front.y / -dir.y;
    vec4 inter; //point of intersection (xz)
    inter.x = front.x + dir.x * mult;
    inter.y = 0;
    inter.z = front.z + dir.z * mult;
    inter.w = 1;
    centerChunk = Coord::worldToTile(inter);
    centerChunk.x /= CHUNK_SIZE;
    centerChunk.y /= CHUNK_SIZE;
}

void WorldRenderer::updateVBOChunks(bool force)
{
    Renderer::bindWorldVBO();
    //save old center chunk - if it didn't actually change, don't do anything
    Pos2 oldCenter = centerChunk;
    calcCenterChunk();
    if(!force && (oldCenter.x == centerChunk.x && oldCenter.y == centerChunk.y))
        return;
    //go through the currently allocated chunks and free if they are no longer in the visible square
    int dist = (int(sqrt(VBO_CHUNKS)) - 1) / 2; //maximum distance from centerChunk in x or z
                                                //PRINT("Updating vbo chunks with center chunk at " << centerChunk.x << ", " << centerChunk.y)
    for(int i = 0; i < VBO_CHUNKS; i++)
    {
        if(chunkAlloc[i].x != CHUNK_FREE && (abs(chunkAlloc[i].x - centerChunk.x) > dist || abs(chunkAlloc[i].y - centerChunk.y) > dist))
        {
            //PRINT("Chunk at " << chunkAlloc[i].x << ", " << chunkAlloc[i].y << " is too far from center, deallocating.")
            chunkAlloc[i] = {CHUNK_FREE, 0};
        }
    }
    //now go through all chunks that need to be in the vbo and allocate them if not already there
    for(int i = centerChunk.x - dist; i <= centerChunk.x + dist; i++)
    {
        for(int j = centerChunk.y - dist; j <= centerChunk.y + dist; j++)
        {
            if(!isChunkAllocated(i, j))
                DBASSERT(allocChunk(i, j));
        }
    }
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