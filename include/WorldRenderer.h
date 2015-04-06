//
//  WorldRenderer.h
//  Magnate
//
//  Created by Brian Kelley on 3/9/1568.
//
//

#ifndef __Magnate__WorldRenderer__
#define __Magnate__WorldRenderer__

#include <stdio.h>
#include <set>
#include "Terrain.h"
#include "World.h"
#include "Coord.h"
#include "Chunk.h"
#include "RenderRoutines.h"
#include "Constants.h"

namespace WorldRenderer
{
    const int MAX_CHUNK_CACHE = 100;
    extern std::map<std::pair<int, int>, Chunk*> chunkCache;
    extern void preload();
    extern void render();
    extern void trimChunkCache();
    extern void drawTerrain();
    extern void drawChunk(Chunk* c);
    extern void drawChunkBorder(Chunk* c1, Chunk* c2);
    extern void drawChunkIntersection(Chunk* c1, Chunk* c2, Chunk* c3, Chunk* c4);
    extern void assertInCache(std::pair<int, int> chunkLoc);
    extern std::pair<int, int> pixelToChunk(int scrX, int scrY);
}

#endif /* defined(__Magnate__WorldRenderer__) */