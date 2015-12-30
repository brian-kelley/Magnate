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
#include "GlmHeaders.h"
#include "RenderRoutines.h"
#include "Constants.h"
#include "Renderer.h"
#include "Heightmap.h"

typedef struct
{
    Vertex3D p1;
    Vertex3D p2;
    Vertex3D p3;
    Vertex3D p4;
} Quad;

namespace WorldRenderer
{
    void init(Heightmap* worldHeights, Heightmap* worldBiomes);
    void dispose();
    bool freeChunk(int x, int z); //mark slot in VBO as free
    bool allocChunk(int x, int z); //true if successful
    bool isChunkAllocated(int x, int z);
    void setTexCoords(Quad* q, Ground g);
    void getTileQuad(Quad* q, int x, int z);
    void calcCenterChunk();
    void updateVBOChunks(bool force = false); //call once during frame if camera moves or rotates
    const short CHUNK_FREE = 0x7FFF; //value for x to mark free chunk
    const int VBO_BYTES_PER_CHUNK = constants::CHUNK_SIZE * constants::CHUNK_SIZE * sizeof(Vertex3D) * 4;
    const float ambientWeight = 0.3;
    const float diffuseWeight = 0.7;
    const glm::vec3 sunlight = {0.3, 0.9055, 0.3}; //eventually could make this dynamic
    extern Quad* vboScratchBuf;
    extern Pos2 centerChunk; //chunk at center of view
    extern Pos2 chunkAlloc[constants::VBO_CHUNKS];
    extern Heightmap* heights;
    extern Heightmap* grounds;
}

#endif /* defined(__Magnate__WorldRenderer__) */