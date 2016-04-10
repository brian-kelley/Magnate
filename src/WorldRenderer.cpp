#include "WorldRenderer.h"

using namespace std;
using namespace Coord;
using namespace glm;

Pos2 WorldRenderer::centerChunk;
TexCoord WorldRenderer::uvCache[4 * Ground::NUM_TYPES];
const int WorldRenderer::CHUNK_SIZE = 32;      //64 tiles on a side
const int WorldRenderer::VBO_CHUNKS = 81;      //5x5 grid
const short WorldRenderer::CHUNK_FREE = 0x7FFF; //value for x to mark free chunk
Vertex3D* WorldRenderer::vertexBuf;             //Scratch buffer for a single chunk before sending to GPU
Pos2* WorldRenderer::chunkAlloc;                //Fixed-length list of chunks that are currently in VBO
VBO WorldRenderer::vbo;
VBO WorldRenderer::meshVBO;

void WorldRenderer::init()
{
    vbo = VBO(VBO_CHUNKS * CHUNK_SIZE * CHUNK_SIZE * 4, VBO::v3D, GL_DYNAMIC_DRAW);
    meshVBO = VBO(0, VBO::v3D, GL_STATIC_DRAW);
    createUVCache();
    //create a statically sized buffer for tiles (copy of VBO)
    vertexBuf = new Vertex3D[4 * CHUNK_SIZE * CHUNK_SIZE];
    //create list of chunk allocation slots, and mark all as free
    chunkAlloc = new Pos2[VBO_CHUNKS];
    for(int i = 0; i < VBO_CHUNKS; i++)
        chunkAlloc[i].x = CHUNK_FREE;
    updateVBOChunks();
    Camera::cameraMotion.addListener(NULL, processCameraUpdate);
    World::worldLoaded.addListener(NULL, processWorldLoad);
}

void WorldRenderer::dispose()
{
    delete vertexBuf;
    delete chunkAlloc;
}

void WorldRenderer::draw()
{
    if(World::drawing)
    {
#ifdef MAGNATE_DEBUG
        if(Input::keystate[SDL_SCANCODE_M])
        {
            //Draw a red wireframe mesh instead of solid heightmap quads
            drawDebugWireframeMesh();
            return;
        }
#endif
        glEnable(GL_DEPTH_TEST);
        vbo.draw(0, 4 * CHUNK_SIZE * CHUNK_SIZE * VBO_CHUNKS, GL_QUADS);
    }
}

Pos3 WorldRenderer::getTileVertexPos(int chunkX, int chunkZ, int tileX, int tileZ)
{
    Pos2 loc(chunkX * CHUNK_SIZE + tileX, chunkZ * CHUNK_SIZE + tileZ);
    return Coord::tileToWorld(loc.x, World::getHeights().get(loc), loc.y).xyz();
}

//set attributes for the 4 vertices of a tile
void WorldRenderer::getTileQuad(int quadIndex, int x, int z)
{
    const auto& heights = World::getHeights();
    const auto& terrain = World::getBiomes();
    Vertex3D* quad = &vertexBuf[quadIndex * 4];
    //Positions
    quad[0].pos = tileToWorld(x, heights.get(x, z), z).xyz();
    quad[1].pos = tileToWorld(x + 1, heights.get(x + 1, z), z).xyz();
    quad[2].pos = tileToWorld(x + 1, heights.get(x + 1, z + 1), z + 1).xyz();
    quad[3].pos = tileToWorld(x, heights.get(x, z + 1), z + 1).xyz();
    //get normal to produce color (on CPU)
    vec3 normal = normalize(cross(quad[1].pos - quad[3].pos, quad[0].pos - quad[2].pos));
    auto ground = terrain.get(x, z);
    if(ground == 0)
    {
        if(heights.get(x, z) == 0)
            ground = WATER;
    }
    quad[0].texcoord = uvCache[ground * 4 + 0];
    quad[1].texcoord = uvCache[ground * 4 + 1];
    quad[2].texcoord = uvCache[ground * 4 + 2];
    quad[3].texcoord = uvCache[ground * 4 + 3];
    quad[0].norm = normal;
    quad[1].norm = normal;
    quad[2].norm = normal;
    quad[3].norm = normal;
    quad[0].color = {255, 255, 255, 255};
    quad[1].color = {255, 255, 255, 255};
    quad[2].color = {255, 255, 255, 255};
    quad[3].color = {255, 255, 255, 255};
}

void WorldRenderer::allocChunk(Pos2 pos)
{
    int allocTarget = -1;
    for(int i = 0; i < VBO_CHUNKS; i++)
    {
        if(chunkAlloc[i].x == CHUNK_FREE)
        {
            allocTarget = i;
            break;
        }
    }
    DBASSERT(allocTarget != -1);
    int chunkXOffset = CHUNK_SIZE * pos.x;
    int chunkZOffset = CHUNK_SIZE * pos.y;
    //Populate scratch space with quads for requested chunk
    for(int i = 0; i < CHUNK_SIZE; i++) //tiles by x
    {
        for(int j = 0; j < CHUNK_SIZE; j++) //tiles by z
        {
            getTileQuad(j * CHUNK_SIZE + i, chunkXOffset + i, chunkZOffset + j);
        }
    }
    //mark slot in chunkAlloc as filled
    chunkAlloc[allocTarget].x = pos.x;
    chunkAlloc[allocTarget].y = pos.y;
    int vertexOffset = 4 * CHUNK_SIZE * CHUNK_SIZE * allocTarget;
    vbo.writeData(vertexOffset, CHUNK_SIZE * CHUNK_SIZE * 4, vertexBuf);
}

bool WorldRenderer::isChunkAllocated(Pos2 pos)
{
    for(int i = 0; i < VBO_CHUNKS; i++)
    {
        if(chunkAlloc[i] == pos)
            return true;
    }
    return false;
}

void WorldRenderer::processCameraUpdate(void*, const glm::mat4 &viewMat)
{
    //First, figure out if center chunk has changed and update VBO accordingly
    auto center = Coord::getViewCenter(viewMat);
    auto centerTile = Coord::worldToTile(center);
    Pos2 newCenterChunk(centerTile.x / CHUNK_SIZE, centerTile.y / CHUNK_SIZE);
    if(centerChunk != newCenterChunk)
    {
        //center of view has moved across chunk boundary
        centerChunk = newCenterChunk;
        updateVBOChunks();
    }
}

void WorldRenderer::updateVBOChunks()
{
    //go through the currently allocated chunks and free if they are no longer in the visible square
    //first, make a rectangle representing chunks that should be loaded
    int s = sqrt(VBO_CHUNKS);
    Rectangle toLoad(centerChunk.x - s / 2, centerChunk.y - s / 2, s, s);
    for(int i = 0; i < VBO_CHUNKS; i++)
    {
        if(chunkAlloc[i].x != CHUNK_FREE && !isPointInRect(toLoad, chunkAlloc[i]))
        {
            chunkAlloc[i] = Pos2{CHUNK_FREE, 0};
        }
    }
    //now go through all chunks that need to be in the vbo and allocate them if not already there
    for(int i = toLoad.x; i < toLoad.x + toLoad.w; i++)
    {
        for(int j = toLoad.y; j < toLoad.y + toLoad.h; j++)
        {
            Pos2 pos(i, j);
            if(!isChunkAllocated(pos))
                allocChunk(pos);
        }
    }
}

void WorldRenderer::createUVCache()
{
    for(int g = 0; g < Ground::NUM_TYPES; g++)
    {
        auto tile = Atlas::textureFromName(Terrain::getTextureName((Ground) g));
        uvCache[g * 4 + 0] = {tile.x, tile.y};
        uvCache[g * 4 + 1] = {tile.x + tile.width, tile.y};
        uvCache[g * 4 + 2] = {tile.x + tile.width, tile.y + tile.height};
        uvCache[g * 4 + 3] = {tile.x, tile.y + tile.height};
    }
}

void WorldRenderer::processWorldLoad(void*, const bool& val)
{
    if(val)
    {
        for(int i = 0; i < VBO_CHUNKS; i++)
        {
            chunkAlloc[i].x = CHUNK_FREE;
        }
        updateVBOChunks();
        buildMeshVBO();
    }
}

void WorldRenderer::buildMeshVBO()
{
    //create a triangle for each face (VBO is already correct size)
    auto& faces = World::mesh.faces;
    auto& verts = World::mesh.vertices;
    Vertex3D v1, v2, v3;
    //only positions and norms are modified
    v1 = v2 = v3 = {Color4(170, 0, 0, 255), TexCoord(-1, -1), Pos3(0, 0, 0), Pos3(0, 1, 0)};
    int numVertices = 3 * faces.size;
    meshVBO.resize(numVertices);
    GLERR;
    Vertex3D* meshbuf = (Vertex3D*) malloc(numVertices * sizeof(Vertex3D));
    int vIndex = 0;
    for(auto it = faces.begin(); it != faces.end(); it++)
    {
        v1.norm = v2.norm = v3.norm = it->getNorm();
        v1.pos = verts[it->v[0]].pos;
        v2.pos = verts[it->v[1]].pos;
        v3.pos = verts[it->v[2]].pos;
        meshbuf[vIndex++] = v1;
        meshbuf[vIndex++] = v2;
        meshbuf[vIndex++] = v3;
    }
    meshVBO.writeData(0, numVertices, meshbuf);
    free(meshbuf);
}

void WorldRenderer::drawDebugWireframeMesh()
{
    //go to wireframe mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    meshVBO.draw(0, 3 * World::mesh.faces.size, GL_TRIANGLES);
    //restore
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

