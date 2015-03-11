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
using namespace coord;

map<Point, Chunk*> WorldRenderer::chunkCache;

void WorldRenderer::render()
{
    updateChunkCache();
    drawTerrain();
}

void WorldRenderer::trimChunkCache()
{
    while(chunkCache.size() > MAX_CHUNK_CACHE)
    {
        chunkCache.erase(chunkCache.begin());
    }
}

void WorldRenderer::updateChunkCache()
{
    //Ideally, absolute maximum of 4 chunks should be visible at any one time
    //Therefore, only need to check that the four corners of the viewport are covered by chunkCache
    Point checkPt;
    checkPt.x = int(xi(screenX, screenY)) / Chunk::CHUNK_SIZE;
    checkPt.y = int(yj(screenX, screenY)) / Chunk::CHUNK_SIZE;
    if(chunkCache.find(checkPt) == chunkCache.end())
    {
        chunkCache[checkPt] = new Chunk(checkPt.x, checkPt.y);
    }
    checkPt.x = int(xi(screenX + WINDOW_W, screenY)) / Chunk::CHUNK_SIZE;
    checkPt.y = int(yj(screenX + WINDOW_W, screenY)) / Chunk::CHUNK_SIZE;
    if(chunkCache.find(checkPt) == chunkCache.end())
    {
        chunkCache[checkPt] = new Chunk(checkPt.x, checkPt.y);
    }
    checkPt.x = int(xi(screenX + WINDOW_W, screenY + WINDOW_H)) / Chunk::CHUNK_SIZE;
    checkPt.y = int(yj(screenX + WINDOW_W, screenY + WINDOW_H)) / Chunk::CHUNK_SIZE;
    if(chunkCache.find(checkPt) == chunkCache.end())
    {
        chunkCache[checkPt] = new Chunk(checkPt.x, checkPt.y);
    }
    checkPt.x = int(xi(screenX + WINDOW_W, screenY + WINDOW_H)) / Chunk::CHUNK_SIZE;
    checkPt.y = int(yj(screenX + WINDOW_W, screenY + WINDOW_H)) / Chunk::CHUNK_SIZE;
    if(chunkCache.find(checkPt) == chunkCache.end())
    {
        chunkCache[checkPt] = new Chunk(checkPt.x, checkPt.y);
    }
    trimChunkCache();
}

void WorldRenderer::drawTerrain()
{
    Point loc;
    loc.x = int(xi(screenX, screenY)) / Chunk::CHUNK_SIZE;
    loc.y = int(yj(screenX, screenY)) / Chunk::CHUNK_SIZE;
    drawChunk(chunkCache[loc]);
    int rightI = int(xi(screenX + WINDOW_W, screenY)) / Chunk::CHUNK_SIZE;
    int bottomJ = int(xi(screenX, screenY)) / Chunk::CHUNK_SIZE;
    if(loc.x != rightI)
    {
        Chunk* left = chunkCache[loc];
        //loc.x never has to be decremented to get full coverage, only incremented
        loc.x++;
        Chunk* right = chunkCache[loc];
        drawChunk(right);
        for(int i = 0; i < Chunk::CHUNK_SIZE - 1; i++)
        {
            RenderRoutines::isoBlit(Terrain::terrainTextures[left->mesh[Chunk::CHUNK_SIZE -1][i].g], right->getIOffset() - TERRAIN_TILE_SIZE, left->getJOffset() + i * TERRAIN_TILE_SIZE, left->mesh[Chunk::CHUNK_SIZE - 1][i].height, right->mesh[0][i].height, right->mesh[0][i + 1].height, left->mesh[Chunk::CHUNK_SIZE - 1][i + 1].height);
        }
        loc.x--;
    }
    if(loc.y != bottomJ)
    {
        Chunk* top = chunkCache[loc];
        loc.y++;
        Chunk* bottom = chunkCache[loc];
        drawChunk(bottom);
        for(int i = 0; i < Chunk::CHUNK_SIZE - 1; i++)
        {
            RenderRoutines::isoBlit(Terrain::terrainTextures[top->mesh[i][0].g], top->getIOffset() + i * TERRAIN_TILE_SIZE, top->getJOffset(), top->mesh[i][0].height, top->mesh[i + 1][0].height, bottom->mesh[i + 1][Chunk::CHUNK_SIZE - 1].height, bottom->mesh[i][Chunk::CHUNK_SIZE - 1].height);
        }
        loc.y--;
    }
    if(loc.x != rightI && loc.y != bottomJ)
    {
        loc.x++;
        loc.y++;
        drawChunk(chunkCache[loc]);
        //but both edges along the border of this chunk with other visible ones
        //have already been taken care of in the above 2 ifs
    }
}

void WorldRenderer::drawChunk(Chunk* c)
{
    Chunk* draw = c;
    for(int i = 0; i < Chunk::CHUNK_SIZE - 1; i++)
    {
        for(int j = 0; j < Chunk::CHUNK_SIZE - 1; j++)
        {
            Point nodeDst = coord::project3DPoint(draw->getIOffset() + TERRAIN_TILE_SIZE * i, draw->getJOffset() + TERRAIN_TILE_SIZE * j, draw->mesh[i][j].height / 256.0);
            if(nodeDst.x + ISO_WIDTH / 2 > screenX && nodeDst.x - ISO_WIDTH < screenX + WINDOW_W)
            {
                if(nodeDst.y + ISO_HEIGHT / 2 > screenY && nodeDst.y - ISO_HEIGHT / 2 < screenY + WINDOW_H)
                {
                    RenderRoutines::isoBlit(Terrain::terrainTextures[draw->mesh[i][j].g],
                                            draw->getIOffset() + TERRAIN_TILE_SIZE * i,
                                            draw->getJOffset() + TERRAIN_TILE_SIZE * j,
                                            draw->mesh[i][j].height,
                                            draw->mesh[i + 1][j].height,
                                            draw->mesh[i + 1][j + 1].height,
                                            draw->mesh[i][j + 1].height);
                }
            }
        }
    }
}
