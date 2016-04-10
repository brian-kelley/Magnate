#include "Mesh.h"
#include "DebugTools.h"
#include "RandomUtils.h"
#include "GenTypes.h"
#include "GlobalConfig.h"
#include "Coord.h"
#include "Terrain.h"

using namespace std;
using namespace glm;
using namespace GlobalConfig;
using namespace MeshTypes;

int Face::NONE_VALUE = Ground::WATER; //the value of triangles outside map
const int MeshTypes::NO_FACE = -1;
Pool<Vertex>* Face::vertArray = nullptr;    //TODO: won't work if multiple Meshes in use

Edge::Edge(int v1, int v2, int f1, int f2)
{
    v[0] = v1;
    v[1] = v2;
    f[0] = f1;
    f[1] = f2;
}

Face::Face(int v1, int v2, int v3, int e1, int e2, int e3, int value)
{
    v[0] = v1;
    v[1] = v2;
    v[2] = v3;
    e[0] = e1;
    e[1] = e2;
    e[2] = e3;
    if(getNorm().y < 0)     //ensure triangle faces outward
    {
        int temp = v[0];
        v[0] = v[1];
        v[1] = temp;
    }
    this->value = value;
}

Vec3 Face::getNorm()
{
    auto& v1 = (*vertArray)[v[0]];
    auto& v2 = (*vertArray)[v[1]];
    auto& v3 = (*vertArray)[v[2]];
    return normalize(cross(v3 - v2, v2 - v1));
}

Mesh::Mesh() : vertices(getMaxVertices()), edges(getMaxEdges()), faces(getMaxFaces())
{
    Face::vertArray = &vertices;
}

void Mesh::initWorldMesh(Heightmap& heights, Heightmap& faceValues, float faceMatchCutoff)
{
    PRINT("Constructing tri mesh.");
    //pools are now big enough to store all features of the most detailed mesh
    simpleLoadHeightmap(heights, faceValues);
    //simplify(faceMatchCutoff);
    PRINT("Done with mesh.");
}

void Mesh::simpleLoadHeightmap(Heightmap& heights, Heightmap& faceValues)
{
    //IMPORTANT: this depends heavily on the fact that the Pool allocations
    //are consecutive and in order until a free() happens
    //Clear all lists
    vertices.clear();
    edges.clear();
    faces.clear();
    //Iterate over heightmap vertices, add vertices row-by-row
    for(int y = 0; y <= WORLD_SIZE; y++)
    {
        for(int x = 0; x <= WORLD_SIZE; x++)
        {
            Vertex loc = Coord::tileToWorld(x, heights.get(x, y), y).xyz();
            vertices.alloc(loc);
        }
    }
    //Add edges and faces quad-by-quad
    //Each quad has 2 faces: upper-left and lower-right
    //Each quad has left, top, and diagonal edges
    //Additionally, quads on right and bottom boundary of world have right/bottom resp.
    //Assume (i,j) is tile coords of upper-left corner
    for(int y = 0; y < WORLD_SIZE; y++)
    {
        for(int x = 0; x < WORLD_SIZE; x++)
        {
            int ulv = hmVertIndex(x, y);
            int urv = hmVertIndex(x + 1, y);
            int llv = hmVertIndex(x, y + 1);
            int lrv = hmVertIndex(x + 1, y + 1);
            int eleft = hmEdgeIndex(x, y, EdgeDir::LEFT);
            int eright = hmEdgeIndex(x, y, EdgeDir::RIGHT);
            int etop = hmEdgeIndex(x, y, EdgeDir::TOP);
            int ebot = hmEdgeIndex(x, y, EdgeDir::BOTTOM);
            int ediag = hmEdgeIndex(x, y, EdgeDir::DIAGONAL);
            int f1 = hmFaceIndex(x, y, FaceDir::UPPER_LEFT);
            int f2 = hmFaceIndex(x, y, FaceDir::LOWER_RIGHT);
            int fleft = hmFaceIndex(x - 1, y, FaceDir::LOWER_RIGHT);
            int ftop = hmFaceIndex(x, y - 1, FaceDir::LOWER_RIGHT);
            //left, top, diagonal, right, bottom
            //left
            DBASSERT(eleft == edges.alloc(Edge(ulv, llv, f1, fleft)));
            //top
            DBASSERT(etop == edges.alloc(Edge(ulv, urv, f1, ftop)));
            //Diagonal edge 
            DBASSERT(ediag == edges.alloc(Edge(urv, llv, f1, f2)));
            //right, if on right edge of map
            if(x == WORLD_SIZE - 1)
            {
                DBASSERT(eright == edges.alloc(Edge(urv, lrv, f2, NO_FACE)));
            }
            //bottom, if on bottom edge of map
            if(y == WORLD_SIZE - 1)
            {
                if(ebot != edges.alloc(Edge(llv, lrv, f2, NO_FACE)))
                {
                    PRINT("Expected: " << ebot);
                    PRINT("Actual: " << edges.size - 1);
                    PRINT("At " << x << ", " << y);
                    throw exception();
                }
                //DBASSERT(ebot == edges.alloc(Edge(llv, lrv, f2, NO_FACE)));
            } 
            //Faces (upper-left and lower-right)
            int faceval = faceValues.get(x, y); //both triangles have same value
            //upper-left
            DBASSERT(f1 == faces.alloc(Face(ulv, urv, llv, eleft, etop, ediag, faceval)));
            //lower-right
            DBASSERT(f2 == faces.alloc(Face(urv, lrv, llv, ediag, eright, ebot, faceval)));
        }
    }
}

void Mesh::simplify(float faceMatchCutoff)
{
    int changes = 1;
    while(changes)
    {
        changes = 0;
        for(auto it = edges.begin(); it != edges.end(); it++)
        {
            auto& edge = *it;
            //get unit normals for faces sharing this edge
            auto& f1 = faces[edge.f[0]];
            auto& f2 = faces[edge.f[1]];
            if(f1.value != f2.value)
                continue;
            auto dotprod = dot(f1.getNorm(), f2.getNorm());
            if(dotprod < faceMatchCutoff)
                continue;
            edgeCollapse(it.loc);
            changes++;
        }
    }
}

void Mesh::edgeCollapse(int edgeNum)
{
    //Deletes an edge, merges two pairs of edges, deletes 2 faces, and a vertex
    auto& edge = edges[edgeNum];
    int f1 = edge.f[0];
    int f2 = edge.f[1];
    int v1 = edge.v[0];
    int v2 = edge.v[1];
    int f11, f12, f21, f22;
    {
/*        auto f1nei = getNeighborFaces(f1, f2);
        f11 = f1nei.first;
        f12 = f1nei.second;
        auto f2nei = getNeighborFaces(f2, f1);
        f21 = f2nei.first;
        f22 = f2nei.second;
        */
    }
}

int Mesh::getMaxVertices()
{
    return (WORLD_SIZE + 1) * (WORLD_SIZE + 1);
}

int Mesh::getMaxEdges()
{
    return 3 * (WORLD_SIZE * WORLD_SIZE) + 2 * WORLD_SIZE;
}

int Mesh::getMaxFaces()
{
    return 2 * WORLD_SIZE * WORLD_SIZE;
}

/* Mesh utility functions */

bool Mesh::collapseConnectivity(int edge)
{

}

int Mesh::hmVertIndex(int x, int y)
{
    return (WORLD_SIZE + 1) * y + x; 
}

int Mesh::hmEdgeIndex(int x, int y, EdgeDir which)
{
    int base =  y + 3 * (x + WORLD_SIZE * y);
    if(y == WORLD_SIZE - 1)
        base += x;
    switch(which)
    {
        case EdgeDir::LEFT:
            return base;
        case EdgeDir::TOP:
            return base + 1;
        case EdgeDir::DIAGONAL:
            return base + 2;
        default:;
    }
    if(x == WORLD_SIZE - 1 && y == WORLD_SIZE - 1)
    {
        if(which == EdgeDir::RIGHT)
            return base + 3;
        else
            return base + 4;
    }
    return base + 3;
}

int Mesh::hmFaceIndex(int x, int y, FaceDir which)
{
    if(x < 0 || x >= WORLD_SIZE || y < 0 || y >= WORLD_SIZE)
        return NO_FACE;
    int base = 2 * (x + y * WORLD_SIZE);
    if(which == FaceDir::UPPER_LEFT)
        return base;
    else 
        return base + 1;
}

