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
#include <list>
#include "Terrain.h"
#include "World.h"
#include "Coord.h"
#include "Chunk.h"
#include "RenderRoutines.h"
#include "Constants.h"

namespace WorldRenderer
{
    const int MAX_CHUNK_CACHE = 100;
    extern std::map<SDL_Point, Chunk*> chunkCache;
    extern void render();
    extern void trimChunkCache();
    extern void updateChunkCache();
    extern void drawTerrain();
    extern void drawChunk(Chunk* c);
}

#endif /* defined(__Magnate__WorldRenderer__) */
