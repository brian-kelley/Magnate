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
private:
    void createUVCache();
    bool freeChunk(int x, int z); //mark slot in VBO as free
    bool allocChunk(int x, int z); //true if successful
    bool isChunkAllocated(int x, int z);
    void updateVBOChunks(bool force = false); //call once during frame if camera moves or rotates
    Pos3 getTileVertexPos(int chunkX, int chunkZ, int tileX, int tileZ);
    void getTileQuad(int quadIndex, int x, int z);
    Pos2 centerChunk; //chunk at center of view
    TexCoord uvCache[Ground::NUM_TYPES * 4];
    glm::vec3 sunlight; //eventually could make this dynamic (day-night cycle?)
    const int CHUNK_SIZE = 64;      //64 tiles on a side
    const int VBO_CHUNKS = 25;      //5x5 grid
    const short CHUNK_FREE = 0x7FFF; //value for x to mark free chunk
    Vertex3D* vertexBuf;             //Scratch buffer for a single chunk before sending to GPU
    Pos2* chunkAlloc;                //Fixed-length list of chunks that are currently in VBO
    const float ambientWeight = 0.3; //Eventual TODO: Have fancy shaders where different materials have different lighting properties
    const float diffuseWeight = 0.7;
    VBO vbo;
};

#endif