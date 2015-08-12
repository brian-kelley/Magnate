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
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "Terrain.h"
#include "World.h"
#include "Coord.h"
#include "Chunk.h"
#include "RenderRoutines.h"
#include "Constants.h"
#include "Renderer.h"

typedef struct
{
    Vertex3D p1;
    Vertex3D p2;
    Vertex3D p3;
    Vertex3D p4;
} Quad;

namespace WorldRenderer
{
    void init();
    void dispose();
    bool freeChunk(int x, int z); //mark slot in VBO as free
    bool allocChunk(int x, int z); //true if successful
    bool isChunkAllocated(int x, int z);
    void setTexCoords(Quad* q, GROUND g);
    void getTileQuad(Quad* q, int x, int z);
    void calcCenterChunk();
    void updateVBOChunks(bool force = false); //call once during frame if camera moves or rotates
    void camRotateLeft();
    void camRotateRight();
    void camForward();
    void camBackward();
    void camLeft();
    void camRight();
    const short CHUNK_FREE = 0x7FFF; //value for x to mark free chunk
    const int VBO_BYTES_PER_CHUNK = constants::CHUNK_SIZE * constants::CHUNK_SIZE * sizeof(Vertex3D) * 4;
    extern Quad* vboScratchBuf;
    extern Pos2 centerChunk; //chunk at center of view
    extern Pos2 chunkAlloc[constants::VBO_CHUNKS];
}

#endif /* defined(__Magnate__WorldRenderer__) */
/*
 TerrainTileSize = 0.25 (const)
 When scale = 1.0, 0-255 height values translate to pixels (how it was before zooming)
 When scale not 1, height 

*/