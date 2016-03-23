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

class WorldRenderer
{
public:
    WorldRenderer();
    ~WorldRenderer();
    void draw();
    static void processCameraUpdate(void* ins, const glm::mat4& viewMat);   //called by Camera when cam moves
    static void processWorldLoad(void* ins, const bool& val);
private:
    void createUVCache();
    void allocChunk(Pos2 pos);
    bool isChunkAllocated(Pos2 pos);
    void updateVBOChunks(); //call once during frame if camera moves or rotates
    Pos3 getTileVertexPos(int chunkX, int chunkZ, int tileX, int tileZ);
    void getTileQuad(int quadIndex, int x, int z);
    Pos2 centerChunk; //chunk at center of view
    TexCoord uvCache[Ground::NUM_TYPES * 4];
    static const int CHUNK_SIZE = 32;      //64 tiles on a side
    static const int VBO_CHUNKS = 81;      //5x5 grid
    static const short CHUNK_FREE = 0x7FFF; //value for x to mark free chunk
    Vertex3D* vertexBuf;             //Scratch buffer for a single chunk before sending to GPU
    Pos2* chunkAlloc;                //Fixed-length list of chunks that are currently in VBO
    VBO vbo;
};

#endif