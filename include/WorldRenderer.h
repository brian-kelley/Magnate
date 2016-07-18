#ifndef __WORLD_RENDERER_H__
#define __WORLD_RENDERER_H__

#include <vector>
#include "Terrain.h"
#include "Coord.h"
#include "GlmHeaders.h"
#include "Atlas.h"
#include "GenTypes.h"
#include "Camera.h"
#include "VBO.h"
#include "World.h"
#include "Heightmap.h"
#include "Input.h"
#include "Mesh.h"
#include "SdlHeaders.h"

namespace WorldRenderer
{
    void init();
    void dispose();
    void draw();
    void buildMeshVBO();
    void processCameraUpdate(void* ins, const glm::mat4& viewMat);   //called by Camera when cam moves
    void processWorldLoad(void* ins, const bool& val);
    void createUVCache();
    void allocChunk(Pos2 pos);
    bool isChunkAllocated(Pos2 pos);
    void updateVBOChunks(); //call once during frame if camera moves or rotates
    vec3 getTileVertexPos(int chunkX, int chunkZ, int tileX, int tileZ);
    void getTileQuad(int quadIndex, int x, int z);
    void drawDebugWireframeMesh();
    extern Pos2 centerChunk; //chunk at center of view
    extern TexCoord uvCache[Ground::NUM_TYPES * 4];
    extern const int CHUNK_SIZE;      //64 tiles on a side
    extern const int VBO_CHUNKS;      //5x5 grid
    extern const short CHUNK_FREE; //value for x to mark free chunk
    extern Vertex3D* vertexBuf;             //Scratch buffer for a single chunk before sending to GPU
    extern Pos2* chunkAlloc;                //Fixed-length list of chunks that are currently in VBO
    extern VBO vbo;
    extern VBO meshVBO;                     //For testing mesh generation/simplification
}

#endif
