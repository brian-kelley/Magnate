#ifndef __WORLD_RENDERER_H__
#define __WORLD_RENDERER_H__

#include <vector>
#include "Terrain.h"
#include "Coord.h"
#include "GlmHeaders.h"
#include "Renderer.h"
#include "Heightmap.h"
#include "Atlas.h"
#include "GenTypes.h"

class WorldRenderer
{
public:
    WorldRenderer(Atlas& mainAtlas, const Heightmap* heights, const Heightmap* terrain);
    ~WorldRenderer();
    void loadWorld(Heightmap& heights, Heightmap& terrain);
    const std::vector<Vertex3D> getVertices();
private:
    void createUVCache(Atlas& atlas);
    bool freeChunk(int x, int z); //mark slot in VBO as free
    bool allocChunk(int x, int z); //true if successful
    bool isChunkAllocated(int x, int z);
    void calcCenterChunk();
    void updateVBOChunks(bool force = false); //call once during frame if camera moves or rotates
    Pos3 getTileVertexPos(int chunkX, int chunkZ, int tileX, int tileZ);
    void getTileQuad(int quadIndex, int x, int z);
    //configure VBO size (64x64 chunk, 25 chunks gives 5 MB VBO)
    static const int CHUNK_SIZE = 64;      //64 tiles on a side
    static const int VBO_CHUNKS = 25;      //5x5 grid
    const short CHUNK_FREE = 0x7FFF; //value for x to mark free chunk
    Vertex3D* vertexBuf;             //Scratch buffer for a single chunk before sending to GPU
    //Terrain lighting
    const float ambientWeight = 0.3;
    const float diffuseWeight = 0.7;
    const glm::vec3 sunlight = {0.3, 0.9055, 0.3}; //eventually could make this dynamic (day-night cycle?)
    Pos2 centerChunk; //chunk at center of view
    Pos2 chunkAlloc[VBO_CHUNKS];
    TexCoord uvCache[Ground::NUM_TYPES * 4];
    const Heightmap* heights;
    const Heightmap* terrain;
};

#endif /* defined(__Magnate__WorldRenderer__) */