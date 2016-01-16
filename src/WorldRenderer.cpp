#include "WorldRenderer.h"

using namespace std;
using namespace Coord;
using namespace glm;

WorldRenderer::WorldRenderer() :
sunlight(0.3, 0.9055, 0.3),
CHUNK_SIZE(64),
VBO_CHUNKS(25),
CHUNK_FREE(0x7FFF),
ambientWeight(0.3),
diffuseWeight(0.7),
vbo(CHUNK_SIZE * CHUNK_SIZE * VBO_CHUNKS * 4, VBO::v3D, GL_DYNAMIC_DRAW)
{
    createUVCache();
    //create a statically sized buffer for tiles (copy of VBO)
    vertexBuf = new Vertex3D[4 * CHUNK_SIZE * CHUNK_SIZE];
    //create list of chunk allocation slots, and mark all as free
    chunkAlloc = new Pos2[VBO_CHUNKS];
    for(int i = 0; i < VBO_CHUNKS; i++)
        chunkAlloc[i].x = CHUNK_FREE;
    updateVBOChunks(true);
}

WorldRenderer::~WorldRenderer()
{
    delete vertexBuf;
    delete chunkAlloc;
}

void WorldRenderer::draw()
{
    glEnable(GL_DEPTH_TEST);
    vbo.draw(0, CHUNK_SIZE * CHUNK_SIZE * VBO_CHUNKS, GL_QUADS);
}

Pos3 WorldRenderer::getTileVertexPos(int chunkX, int chunkZ, int tileX, int tileZ)
{
    Pos2 loc(chunkX * CHUNK_SIZE + tileX, chunkZ * CHUNK_SIZE + tileZ);
    return Coord::tileToWorld(loc.x, World::getHeights().get(loc), loc.y).xyz();
}

//set attributes for the 4 vertices of a tile
void WorldRenderer::getTileQuad(int quadIndex, int x, int z)
{
    auto heights = World::getHeights();
    auto terrain = World::getBiomes();
    Vertex3D* quad = &vertexBuf[quadIndex * 4];
    //Positions
    quad[0].pos = tileToWorld(x, heights.get(x, z), z).xyz();
    quad[1].pos = tileToWorld(x + 1, heights.get(x + 1, z), z).xyz();
    quad[2].pos = tileToWorld(x + 1, heights.get(x + 1, z + 1), z + 1).xyz();
    quad[3].pos = tileToWorld(x, heights.get(x, z + 1), z + 1).xyz();
    //get normal to produce color (on CPU)
    vec3 normal = normalize(cross(quad[1].pos - quad[3].pos, quad[0].pos - quad[2].pos));
    float diffuse = dot(normal, sunlight);
    if(diffuse < 0)
        diffuse = 0;
    float light = diffuse * diffuseWeight + ambientWeight;
    auto ground = terrain.get(x, z);
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
    int vertexOffset = 4 * CHUNK_SIZE * CHUNK_SIZE * allocTarget;
    vbo.writeData(vertexOffset, CHUNK_SIZE * CHUNK_SIZE * 4, vertexBuf);
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

void WorldRenderer::processCameraUpdate(void* ins, const glm::mat4 &viewMat)
{
    auto wr = (WorldRenderer*) ins;
    //First, figure out if center chunk has changed and update VBO accordingly
    auto center = Coord::getViewCenter(viewMat);
    auto centerTile = Coord::worldToTile(center);
    Pos2 newCenterChunk(centerTile.x / wr->CHUNK_SIZE, centerTile.y / wr->CHUNK_SIZE);
    if(wr->centerChunk != newCenterChunk)
    {
        //center of view has moved across chunk boundary
        wr->centerChunk = newCenterChunk;
        wr->updateVBOChunks();
    }
}

void WorldRenderer::updateVBOChunks(bool force)
{
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

void WorldRenderer::createUVCache()
{
    for(int g = 0; g < Ground::NUM_TYPES; g++)
    {
        auto tile = Atlas::textureFromName(Terrain::getTextureName((Ground) g));
        uvCache[g * 4 + 0] = {tile.x, tile.y};
        uvCache[g * 4 + 1] = {tile.x + tile.width, tile.y};
        uvCache[g * 4 + 2] = {tile.x + tile.width, tile.y + tile.height};
        uvCache[g * 4 + 3] = {tile.x, tile.y + tile.height};
    }
}