#include "WorldRenderer.h"

using namespace std;
using namespace Coord;
using namespace glm;

WorldRenderer::WorldRenderer() :
vbo(VBO_CHUNKS * CHUNK_SIZE * CHUNK_SIZE * 4, VBO::v3D, GL_DYNAMIC_DRAW)
{
    createUVCache();
    //create a statically sized buffer for tiles (copy of VBO)
    vertexBuf = new Vertex3D[4 * CHUNK_SIZE * CHUNK_SIZE];
    //create list of chunk allocation slots, and mark all as free
    chunkAlloc = new Pos2[VBO_CHUNKS];
    for(int i = 0; i < VBO_CHUNKS; i++)
        chunkAlloc[i].x = CHUNK_FREE;
    updateVBOChunks();
    Camera::cameraMotion.addListener(this, processCameraUpdate);
    World::worldLoaded.addListener(this, processWorldLoad);
}

WorldRenderer::~WorldRenderer()
{
    delete vertexBuf;
    delete chunkAlloc;
}

void WorldRenderer::draw()
{
    if(World::isDrawing())
    {
        glEnable(GL_DEPTH_TEST);
        vbo.draw(0, 4 * CHUNK_SIZE * CHUNK_SIZE * VBO_CHUNKS, GL_QUADS);
    }
}

Pos3 WorldRenderer::getTileVertexPos(int chunkX, int chunkZ, int tileX, int tileZ)
{
    Pos2 loc(chunkX * CHUNK_SIZE + tileX, chunkZ * CHUNK_SIZE + tileZ);
    return Coord::tileToWorld(loc.x, World::getHeights().get(loc), loc.y).xyz();
}

//set attributes for the 4 vertices of a tile
void WorldRenderer::getTileQuad(int quadIndex, int x, int z)
{
    const auto& heights = World::getHeights();
    const auto& terrain = World::getBiomes();
    Vertex3D* quad = &vertexBuf[quadIndex * 4];
    //Positions
    quad[0].pos = tileToWorld(x, heights.get(x, z), z).xyz();
    quad[1].pos = tileToWorld(x + 1, heights.get(x + 1, z), z).xyz();
    quad[2].pos = tileToWorld(x + 1, heights.get(x + 1, z + 1), z + 1).xyz();
    quad[3].pos = tileToWorld(x, heights.get(x, z + 1), z + 1).xyz();
    //get normal to produce color (on CPU)
    vec3 normal = normalize(cross(quad[1].pos - quad[3].pos, quad[0].pos - quad[2].pos));
    auto ground = terrain.get(x, z);
    if(ground == 0)
    {
        if(heights.get(x, z) == 0)
            ground = WATER;
    }
    quad[0].texcoord = uvCache[ground * 4 + 0];
    quad[1].texcoord = uvCache[ground * 4 + 1];
    quad[2].texcoord = uvCache[ground * 4 + 2];
    quad[3].texcoord = uvCache[ground * 4 + 3];
    quad[0].norm = normal;
    quad[1].norm = normal;
    quad[2].norm = normal;
    quad[3].norm = normal;
    quad[0].color = {255, 255, 255, 255};
    quad[1].color = {255, 255, 255, 255};
    quad[2].color = {255, 255, 255, 255};
    quad[3].color = {255, 255, 255, 255};
}

void WorldRenderer::allocChunk(Pos2 pos)
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
    DBASSERT(allocTarget != -1);
    int chunkXOffset = CHUNK_SIZE * pos.x;
    int chunkZOffset = CHUNK_SIZE * pos.y;
    //Populate scratch space with quads for requested chunk
    for(int i = 0; i < CHUNK_SIZE; i++) //tiles by x
    {
        for(int j = 0; j < CHUNK_SIZE; j++) //tiles by z
        {
            getTileQuad(j * CHUNK_SIZE + i, chunkXOffset + i, chunkZOffset + j);
        }
    }
    //mark slot in chunkAlloc as filled
    chunkAlloc[allocTarget].x = pos.x;
    chunkAlloc[allocTarget].y = pos.y;
    int vertexOffset = 4 * CHUNK_SIZE * CHUNK_SIZE * allocTarget;
    vbo.writeData(vertexOffset, CHUNK_SIZE * CHUNK_SIZE * 4, vertexBuf);
}

bool WorldRenderer::isChunkAllocated(Pos2 pos)
{
    for(int i = 0; i < VBO_CHUNKS; i++)
    {
        if(chunkAlloc[i] == pos)
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

void WorldRenderer::updateVBOChunks()
{
    //go through the currently allocated chunks and free if they are no longer in the visible square
    //first, make a rectangle representing chunks that should be loaded
    int s = sqrt(VBO_CHUNKS);
    Rectangle toLoad(centerChunk.x - s / 2, centerChunk.y - s / 2, s, s);
    for(int i = 0; i < VBO_CHUNKS; i++)
    {
        if(chunkAlloc[i].x != CHUNK_FREE && !isPointInRect(toLoad, chunkAlloc[i]))
        {
            chunkAlloc[i] = Pos2{CHUNK_FREE, 0};
        }
    }
    //now go through all chunks that need to be in the vbo and allocate them if not already there
    for(int i = toLoad.x; i < toLoad.x + toLoad.w; i++)
    {
        for(int j = toLoad.y; j < toLoad.y + toLoad.h; j++)
        {
            Pos2 pos(i, j);
            if(!isChunkAllocated(pos))
                allocChunk(pos);
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

void WorldRenderer::processWorldLoad(void* ins, const bool& val)
{
    if(val)
    {
        WorldRenderer* wr = (WorldRenderer*) ins;
        for(int i = 0; i < wr->VBO_CHUNKS; i++)
        {
            wr->chunkAlloc[i].x = CHUNK_FREE;
        }
        wr->updateVBOChunks();
    }
}
