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
    int WS1 = WORLD_SIZE + 1;
    //Iterate over heightmap vertices, add vertices row-by-row
    for(int i = 0; i <= WORLD_SIZE; i++)        //tile x, rows
    {
        for(int j = 0; j <= WORLD_SIZE; j++)    //tile y, columns
        {
            Vertex loc = Coord::tileToWorld(i, heights.get(i, j), j).xyz();
            vertices.alloc(loc);
        }
    }
    //Add edges and faces quad-by-quad
    //Each quad has 2 faces: upper-left and lower-right
    //Each quad has left, top, and diagonal edges
    //Additionally, quads on right and bottom boundary of world have right/bottom resp.
    //Assume (i,j) is tile coords of upper-left corner
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            int ulv = i * WS1 + j;          //upper left vertex etc.
            int urv = ulv + 1;
            int llv = ulv + WS1;
            int lrv = llv + 1;
            int f1 = (i * WORLD_SIZE + j) * 2;
            int f2 = f1 + 1;                //other triangle in the quad is next
            int fleft = f1 - 1;             //these values may be incorrect (NONE)
            int ftop = f1 - 2 * WORLD_SIZE + 1; //up a row to above upper-left, plus 1
            //Diagonal edge 
            edges.alloc(Edge(urv, llv, f1, f2));
            //Vertical edges (left and right of quad)
            //Create the normal edge, and then modify if edges/corners of map
            Edge eleft(ulv, llv, f1, fleft);
            if(j == 0)
            {
                //on left edge of map, left = NO_FACE
                eleft.f[1] = NO_FACE;
            }
            else if(j == WORLD_SIZE - 1)
            {
                //on right edge of map, create a right edge also
                edges.alloc(Edge(urv, lrv, f2, NO_FACE));
            }
            edges.alloc(eleft);
            Edge etop(ulv, urv, f1, ftop);
            if(i == 0)
            {
                //map top, top = NO_FACE
                etop.f[1] = NO_FACE;
            }
            else if(i == WORLD_SIZE - 1)
            {
                edges.alloc(Edge(llv, lrv, f2, NO_FACE));
            }
            edges.alloc(etop);
            //Faces (upper-left and lower-right)
            int faceval = faceValues.get(i, j); //both triangles have same value
            //upper-left
            faces.alloc(Face(ulv, ulv + 1, ulv + WS1, fleft, ftop, f2, faceval));
            //lower-right
            faces.alloc(Face(ulv + 1, ulv + 1 + WS1, ulv + WS1, 0, 0, 0, faceval));
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

int hmVertIndex(int x, int y)
{
    return (WORLD_SIZE + 1) * y + x; 
}

int hmEdgeIndex(int x, int y, EdgeDir which)
{
    int ulv = hmVertIndex(x, y);
}

int hmFaceIndex(int x, int y, FaceDir which)
{

}

