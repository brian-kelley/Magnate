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

map<pair<int, int>, Chunk*> WorldRenderer::chunkCache;

void WorldRenderer::preload()
{
    int sideLen = sqrt(MAX_CHUNK_CACHE);
    pair<int, int> screenPos = pixelToChunk(screenX, screenY);
    screenPos.first -= sideLen / 2;
    screenPos.second -= sideLen / 2;
    for(int i = screenPos.first; i < screenPos.first + sideLen - 1; i++)
    {
        for(int j = screenPos.second; j < screenPos.second + sideLen - 1; j++)
        {
            assertInCache(pair<int, int>(i, j));
        }
    }
}

void WorldRenderer::render()
{
    drawTerrain();
}

void WorldRenderer::trimChunkCache()
{
    while(chunkCache.size() > MAX_CHUNK_CACHE)
    {
        chunkCache.erase(chunkCache.begin());
    }
}

void WorldRenderer::drawTerrain()
{
    glDisable(GL_SCISSOR_TEST);
    set<pair<int, int>> renderQueue;
    //test which chunks screen edge midpoints are in
    renderQueue.insert(pixelToChunk(screenX + WINDOW_W / 2, screenY));
    renderQueue.insert(pixelToChunk(screenX + WINDOW_W, screenY + WINDOW_H / 2));
    renderQueue.insert(pixelToChunk(screenX + WINDOW_W / 2, screenY + WINDOW_H));
    renderQueue.insert(pixelToChunk(screenX, screenY + WINDOW_H / 2));
    //test corners
    renderQueue.insert(pixelToChunk(screenX, screenY));
    renderQueue.insert(pixelToChunk(screenX + WINDOW_W, screenY));
    renderQueue.insert(pixelToChunk(screenX + WINDOW_W, screenY + WINDOW_H));
    renderQueue.insert(pixelToChunk(screenX, screenY + WINDOW_H));
    //need to populatethis value after checking if there's a 4-way chunk intersection onscreen
    pair<int, int> local = pixelToChunk(screenX, screenY);
    int minI = local.first + 3;
    int maxI = local.first - 3;
    int minJ = local.second + 3;
    int maxJ = local.second - 3;
    for(auto iter : renderQueue)
    {
        if(iter.first < minI)
        {
            minI = iter.first;
        }
        if(iter.first > maxI)
        {
            maxI = iter.first;
        }
        if(iter.second < minJ)
        {
            minJ = iter.second;
        }
        if(iter.second > maxJ)
        {
            maxJ = iter.second;
        }
    }
    //3 cases for # of rendered chunks: 1, 2 or 4
    if(minI == maxI && minJ == maxJ)       //Only one chunk onscreen
    {
        assertInCache(pair<int, int>(minI, minJ));
        drawChunk(chunkCache[pair<int, int>(minI, minJ)]);
    }
    else if(minI != maxI && minJ != maxJ) //Four chunks in a square
    {
        assertInCache(pair<int, int>(minI, minJ));
        assertInCache(pair<int, int>(minI, maxJ));
        assertInCache(pair<int, int>(maxI, minJ));
        assertInCache(pair<int, int>(maxI, maxJ));
        Chunk* east = chunkCache[pair<int, int>(maxI, maxJ)];
        Chunk* west = chunkCache[pair<int, int>(minI, minJ)];
        Chunk* north = chunkCache[pair<int, int>(minI, maxJ)];
        Chunk* south = chunkCache[pair<int, int>(maxI, minJ)];
        drawChunk(north);
        //west-north border, ground comes from edge of west
        for(int i = 0; i < Chunk::CHUNK_SIZE - 1; i++)
        {
            RenderRoutines::isoBlit(Terrain::terrainTextures[west->mesh[i][Chunk::CHUNK_SIZE - 1].g], west->getIOffset() + TERRAIN_TILE_SIZE * i, north->getJOffset() - TERRAIN_TILE_SIZE, west->mesh[i][Chunk::CHUNK_SIZE - 1].height, north->mesh[i][0].height, north->mesh[i + 1][0].height, west->mesh[i + 1][Chunk::CHUNK_SIZE - 1].height);
        }
        //north-east border, ground comes from north
        for(int j = 0; j < Chunk::CHUNK_SIZE - 1; j++)
        {
            RenderRoutines::isoBlit(Terrain::terrainTextures[north->mesh[Chunk::CHUNK_SIZE - 1][j].g], east->getIOffset() - TERRAIN_TILE_SIZE, east->getJOffset() + j * TERRAIN_TILE_SIZE, north->mesh[Chunk::CHUNK_SIZE - 1][j].height, north->mesh[Chunk::CHUNK_SIZE - 1][j + 1].height, east->mesh[0][j + 1].height, east->mesh[0][j].height);
        }
        //center tile in 4-way intersection that isn't covered by the edge loops
        RenderRoutines::isoBlit(Terrain::terrainTextures[west->mesh[Chunk::CHUNK_SIZE - 1][Chunk::CHUNK_SIZE - 1].g], east->getIOffset() - TERRAIN_TILE_SIZE, east->getJOffset() - TERRAIN_TILE_SIZE, west->mesh[Chunk::CHUNK_SIZE - 1][Chunk::CHUNK_SIZE - 1].height, north->mesh[Chunk::CHUNK_SIZE - 1][0].height, east->mesh[0][0].height, south->mesh[0][Chunk::CHUNK_SIZE - 1].height);
        drawChunk(west);
        drawChunk(east);
        //west-south border, ground from west
        for(int i = 0; i < Chunk::CHUNK_SIZE - 1; i++)
        {
            RenderRoutines::isoBlit(Terrain::terrainTextures[west->mesh[Chunk::CHUNK_SIZE - 1][i].g], south->getIOffset() - TERRAIN_TILE_SIZE, south->getJOffset() + TERRAIN_TILE_SIZE * i, west->mesh[Chunk::CHUNK_SIZE - 1][i].height, west->mesh[Chunk::CHUNK_SIZE - 1][i + 1].height, south->mesh[0][i + 1].height, south->mesh[0][i].height);
        }
        //south-east border, ground from south
        for(int i = 0; i < Chunk::CHUNK_SIZE - 1; i++)
        {
            RenderRoutines::isoBlit(Terrain::terrainTextures[south->mesh[i][Chunk::CHUNK_SIZE - 1].g], east->getIOffset() + TERRAIN_TILE_SIZE * i, east->getJOffset() - TERRAIN_TILE_SIZE, south->mesh[i][Chunk::CHUNK_SIZE - 1].height, east->mesh[i][0].height, east->mesh[i + 1][0].height, south->mesh[i + 1][Chunk::CHUNK_SIZE - 1].height);
        }
        drawChunk(south);
    }
    else if(minI != maxI)   //two adjacent chunks that only differ in I, J same for both
    {
        pair<int, int> point;
        point.first = minI;
        point.second = minJ;
        assertInCache(point);
        Chunk* topLeft = chunkCache[point];
        point.first++;
        assertInCache(point);
        Chunk* bottomRight = chunkCache[point];
        drawChunk(topLeft);
        for(int i = 0; i < Chunk::CHUNK_SIZE - 1; i++)
        {
            RenderRoutines::isoBlit(Terrain::terrainTextures[topLeft->mesh[Chunk::CHUNK_SIZE - 1][i].g], bottomRight->getIOffset() - TERRAIN_TILE_SIZE, bottomRight->getJOffset() + i * TERRAIN_TILE_SIZE, topLeft->mesh[Chunk::CHUNK_SIZE - 1][i].height, topLeft->mesh[Chunk::CHUNK_SIZE - 1][i + 1].height, bottomRight->mesh[0][i + 1].height, bottomRight->mesh[0][i].height);
        }
        drawChunk(bottomRight);
    }
    else if(minJ != maxJ)   //2 that only differ in J, I same for both
    {
        pair<int, int> point;
        point.first = minI;
        point.second = minJ;
        assertInCache(point);
        Chunk* bottomLeft = chunkCache[point];
        point.second++;
        assertInCache(point);
        Chunk* topRight = chunkCache[point];
        drawChunk(topRight);
        for(int i = 0; i < Chunk::CHUNK_SIZE - 1; i++)
        {
            RenderRoutines::isoBlit(Terrain::terrainTextures[bottomLeft->mesh[i][Chunk::CHUNK_SIZE - 1].g], bottomLeft->getIOffset() + i * TERRAIN_TILE_SIZE, topRight->getJOffset() - TERRAIN_TILE_SIZE, bottomLeft->mesh[i][Chunk::CHUNK_SIZE - 1].height, topRight->mesh[i][0].height, topRight->mesh[i + 1][0].height, bottomLeft->mesh[i + 1][Chunk::CHUNK_SIZE - 1].height);
        }
        drawChunk(bottomLeft);
    }
}

//Draws every tile that can be drawn with only the nodes in this chunk (lacks 2 edges)
void WorldRenderer::drawChunk(Chunk* c)
{
    for(int i = 0; i < Chunk::CHUNK_SIZE - 1; i++)
    {
        for(int j = 0; j < Chunk::CHUNK_SIZE - 1; j++)
        {
            Point p = project3DPoint(c->getIOffset() + TERRAIN_TILE_SIZE * i, c->getJOffset() + TERRAIN_TILE_SIZE * j, 0);
            if(p.x > -50 && p.x < WINDOW_W + 50 && p.y > -30 && p.y < WINDOW_H + 30)
            {
                RenderRoutines::isoBlit(Terrain::terrainTextures[c->mesh[i][j].g],
                                        c->getIOffset() + TERRAIN_TILE_SIZE * i,
                                        c->getJOffset() + TERRAIN_TILE_SIZE * j,
                                        c->mesh[i][j].height,
                                        c->mesh[i][j + 1].height,
                                        c->mesh[i + 1][j + 1].height,
                                        c->mesh[i + 1][j].height);
            }
        }
    }
}

pair<int, int> WorldRenderer::pixelToChunk(int scrX, int scrY)
{
    pair<int, int> coords;
    coords.first = floor(xi(scrX, scrY) / (TERRAIN_TILE_SIZE * Chunk::CHUNK_SIZE));
    coords.second = floor(yj(scrX, scrY) / (TERRAIN_TILE_SIZE * Chunk::CHUNK_SIZE));
    return coords;
}

void WorldRenderer::assertInCache(pair<int, int> chunkLoc)
{
    if(chunkCache.find(chunkLoc) == chunkCache.end())
    {
        chunkCache[chunkLoc] = new Chunk(chunkLoc.first, chunkLoc.second);
    }
    trimChunkCache();
}
