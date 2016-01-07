//
//  WorldRenderer.cpp
//  Magnate
//
//  Created by Brian Kelley on 3/9/1568.
//
//

#include "WorldRenderer.h"

using namespace std;
using namespace Coord;
using namespace glm;

WorldRenderer::WorldRenderer(Atlas& mainAtlas, const Heightmap* heights, const Heightmap* terrain)
{
    this->heights = heights;
    this->terrain = terrain;
    createUVCache(mainAtlas);
    //create a statically sized buffer for tiles (copy of VBO)
    vertexBuf = new Vertex3D[4 * CHUNK_SIZE * CHUNK_SIZE];
    //mark all vbo chunk slots as free
    for(int i = 0; i < VBO_CHUNKS; i++)
    {
        chunkAlloc[i].x = CHUNK_FREE;
    }
    updateVBOChunks(true);
}

WorldRenderer::~WorldRenderer()
{
    delete vertexBuf;
}

Pos3 WorldRenderer::getTileVertexPos(int chunkX, int chunkZ, int tileX, int tileZ)
{
    Pos2 loc(chunkX * CHUNK_SIZE + tileX, chunkZ * CHUNK_SIZE + tileZ);
    return Coord::tileToWorld(loc.x, heights->get(loc), loc.y);
}

//set attributes for the 4 vertices of a tile
void WorldRenderer::getTileQuad(int quadIndex, int x, int z)
{
    Vertex3D* quad = &vertexBuf[quadIndex * 4];
    //Positions
    quad[0].pos = tileToWorld(x, heights->get(x, z), z);
    quad[1].pos = tileToWorld(x + 1, heights->get(x + 1, z), z);
    quad[2].pos = tileToWorld(x + 1, heights->get(x + 1, z + 1), z + 1);
    quad[3].pos = tileToWorld(x, heights->get(x, z + 1), z + 1);
    //get normal to produce color (on CPU)
    vec3 normal = normalize(cross(quad[1].pos - quad[3].pos, quad[0].pos - quad[2].pos));
    float diffuse = dot(normal, sunlight);
    if(diffuse < 0)
        diffuse = 0;
    float light = diffuse * diffuseWeight + ambientWeight;
    auto ground = terrain->get(x, z);
    quad[0].texcoord = uvCache[ground * 4 + 0];
    quad[1].texcoord = uvCache[ground * 4 + 1];
    quad[2].texcoord = uvCache[ground * 4 + 2];
    quad[3].texcoord = uvCache[ground * 4 + 3];
    Color4 color = {(unsigned char)(light * 255), (unsigned char)(light * 255), (unsigned char)(light * 255), 255};
    quad[0].color = color;
    quad[1].color = color;
    quad[2].color = color;
    quad[3].color = color;
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
            getTileQuad(j * CHUNK_SIZE + i, chunkXOffset + i, chunkZOffset + j);
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

void WorldRenderer::createUVCache(Atlas &atlas)
{
    for(Ground g = (Ground) 0; g < Ground::NUM_TYPES, g++)
    {
        auto tile = Atlas::textureFromName(Terrain::getTextureName(g));
        
    }
}