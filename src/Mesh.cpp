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
Pool<Vertex>* Face::vertArray = nullptr;    //TODO: won't work if multiple Meshes in use
Pool<Vertex>* Edge::vertArray = nullptr;
Pool<Edge>* Edge::edgeArray = nullptr;

void Vertex::addEdge(int e)
{
    edges.push_back(e);
}

void Vertex::removeEdge(int e)
{
    for(auto it = edges.begin(); it != edges.end(); it++)
    {
        if(*it == e)
        {
            edges.erase(it);
            break;
        }
    }
}

bool Vertex::isOnBoundary()
{
    return minWorldX(pos) || maxWorldX(pos) || minWorldY(pos) || maxWorldY(pos);
}

bool Vertex::isInCorner()
{
    int score = 0;
    if(minWorldX(pos))
        score++;
    if(maxWorldX(pos))
        score++;
    if(minWorldY(pos))
        score++;
    if(maxWorldY(pos))
        score++;
    //if score is 0, point in interior
    //if score is 1, point on edge 
    //if score is 2, point on corner
    DBASSERT(score <= 2);
    return score == 2;
}

Edge::Edge()
{
    v[0] = -1;
    v[1] = -1;
    f[0] = -1;
    f[1] = -1;
}

Edge::Edge(int v1, int v2, int f1, int f2)
{
    v[0] = v1;
    v[1] = v2;
    f[0] = f1;
    f[1] = f2;
    (*vertArray)[v1].edges.push_back(edgeArray->size);
    (*vertArray)[v2].edges.push_back(edgeArray->size);
}

bool Edge::hasFace(int query)
{
    return f[0] == query || f[1] == query;
}

bool Edge::hasVert(int query)
{
    return v[0] == query || v[1] == query;
}

void Edge::replaceFaceLink(int toReplace, int newLink)
{
    if(f[0] == toReplace)
    {
        f[0] = newLink;
    }
    else if(f[1] == toReplace)
    {
        f[1] = newLink;
    }
}

Face::Face()
{
    for(int i = 0; i < 3; i++)
    {
        v[i] = -1;
        e[i] = -1;
    }
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
    return normalize(cross(v3.pos - v2.pos, v2.pos - v1.pos));
}

bool Face::hasEdge(int query)
{
    return e[0] == query || e[1] == query || e[2] == query;
}

bool Face::hasVert(int query)
{
    return v[0] == query || v[1] == query || v[2] == query;
}

bool Face::isClockwise(int v1, int v2)
{
    int v1loc = -1;
    int v2loc = -1;
    for(int i = 0; i < 3; i++)
    {
        if(v[i] == v1)
        {
            v1loc = i;
            break;
        }
    }
    if(v1loc == -1)
        return false;
    for(int i = 0; i < 3; i++)
    {
        if(v[i] == v2)
        {
            v2loc = i;
            break;
        }
    }
    if(v2loc == -1)
        return false;
    return (v1loc + 1) % 3 == v2loc;
}

void Face::replaceEdgeLink(int toReplace, int newLink)
{
    for(int i = 0; i < 3; i++)
    {
        if(e[i] == toReplace)
        {
            e[i] = newLink;
            return;
        }
    }
    PRINT("Error: Tried to replace link to edge " << toReplace << " with " << newLink << " but face didn't have reference!");
    throw runtime_error("oh man");
}

void Face::replaceVertexLink(int toReplace, int newLink)
{
    for(int i = 0; i < 3; i++)
    {
        if(v[i] == toReplace)
        {
            v[i] = newLink;
            return;
        }
    }
    //OK if no vertices matched
}

void Face::checkNormal()
{
    if(getNorm().y < 0)
    {
        SWAP(v[0], v[1]);
    }
}

Mesh::Mesh() : vertices(getMaxVertices()), edges(getMaxEdges()), faces(getMaxFaces())
{
    Face::vertArray = &vertices;
    Edge::vertArray = &vertices;
    Edge::edgeArray = &edges;
}

void Mesh::initWorldMesh(Heightmap& heights, Heightmap& faceValues, float faceMatchCutoff)
{
    PRINT("Constructing tri mesh.");
    //pools are already sized to store all features of the most detailed mesh
    simpleLoadHeightmap(heights, faceValues);
    //TIMEIT(fullCorrectnessCheck());
    //Testing edge collapse
    PRINT("collapsing edge 64...");
    //edgeCollapse(90);
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
            Vertex loc;
            loc.pos = Coord::tileToWorld(x, heights.get(x, y), y).xyz();
            vertices.alloc(loc);
            //can't add edges yet
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
                DBASSERT(eright == edges.alloc(Edge(urv, lrv, f2, -1)));
            }
            //bottom, if on bottom edge of map
            if(y == WORLD_SIZE - 1)
            {
                int loc = edges.alloc(Edge(llv, lrv, f2, -1));
                if(ebot != loc)
                {
                    PRINT("Expected " << ebot);
                    PRINT("Actual " << loc);
                    throw runtime_error("A");
                }
            } 
            //Faces (upper-left and lower-right)
            int faceval = faceValues.get(x, y); //both triangles have same value
            //upper-left
#ifdef MAGNATE_DEBUG
            if(eleft == etop)
                PRINT("err 1");
            if(ediag == etop)
                PRINT("err 2");
            if(eleft == ediag)
                PRINT("err 3");
            if(ediag == eright)
                PRINT("err 4");
            if(ediag == ebot)
                PRINT("err 5");
            if(eright == ebot)
                PRINT("err 6");
#endif
            faces.alloc(Face(ulv, urv, llv, eleft, etop, ediag, faceval));
            //lower-right
            faces.alloc(Face(urv, lrv, llv, ediag, eright, ebot, faceval));
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
            //check that geometric conditions are met
            if(!collapseConnectivity(it.loc) || !collapseFlip(it.loc))
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
    int f1, f2, f11, f12, f21, f22;
    getNeighbors(edgeNum, f1, f2, f11, f12, f21, f22);
    PRINTVAR(f1);
    PRINTVAR(f2);
    PRINTVAR(f11);
    PRINTVAR(f12);
    PRINTVAR(f21);
    PRINTVAR(f22);
    PRINTVAR(faces[f1]);
    PRINTVAR(faces[f2]);
    PRINTVAR(faces[f11]);
    PRINTVAR(faces[f12]);
    PRINTVAR(faces[f21]);
    PRINTVAR(faces[f22]);
    int e11 = getSharedEdge(f1, f11);
    int e12 = getSharedEdge(f1, f12);
    int e21 = getSharedEdge(f2, f21);
    int e22 = getSharedEdge(f2, f22);
    int v1 = edge.v[0];
    int v2 = edge.v[1];
    PRINTVAR(e11);
    PRINTVAR(e12);
    PRINTVAR(e21);
    PRINTVAR(e22);
    PRINTVAR(v1);
    PRINTVAR(v2);
    bool moveVertex = true; //by default, move remaining vertex to midpoint of old ones
#ifdef MAGNATE_DEBUG
    DBASSERT(!vertices[v1].isInCorner() || !vertices[v2].isInCorner());
#endif
    //don't want to move or remove a vertex on the map boundary
    if(vertices[v1].isInCorner() || (vertices[v1].isOnBoundary() && !vertices[v2].isInCorner()))
    {
        //keep v1, delete v2 instead (just by swapping names)
        SWAP(v1, v2);
        moveVertex = false;
    }
    if(vertices[v1].isOnBoundary())
    {
        moveVertex = false;
    }
    replaceVertexLinks(v1, v2); //note: does not free v1, but does free edgeNum
    //e11.v == e12.v, e21.v == e22.v (only face links different, will change)
    if(moveVertex)
    {
        vertices[v2].pos = (vertices[v1].pos + vertices[v2].pos) * 0.5f;
    }
    //mesh no longer contains any links to v1, can safely delete it
    vertices.free(v1);
    //update fxx edge links
    //PRINT("Replacing links in face " << f11 << " to edge " << e11 << " with " << e12);
    faces[f11].replaceEdgeLink(e11, e12);
    if(f11 != f21)
        faces[f21].replaceEdgeLink(e21, e22);
    //now e11 and e21 can be deleted
    edges.free(e11);
    edges.free(e21);
    //prepare to remove f1/f2 by updating e12/e22 face links
    edges[e12].replaceFaceLink(f1, f11);
    edges[e22].replaceFaceLink(f2, f21);
    //nothing links to f1, f2 anymore, delete them
    faces.free(f1);
    faces.free(f2);
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
    {
        base += x;
    }
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
    if(x < WORLD_SIZE - 1 && which == EdgeDir::RIGHT)
    {
        return hmEdgeIndex(x + 1, y, EdgeDir::LEFT);
    }
    if(y < WORLD_SIZE - 1 && which == EdgeDir::BOTTOM)
    {
        return hmEdgeIndex(x, y + 1, EdgeDir::TOP);
    }
    if(x == WORLD_SIZE - 1 && y != WORLD_SIZE - 1 && which == EdgeDir::RIGHT)
    {
        //right of map boundary (but not bottom right) creates right edge
        return base + 3;
    }
    if(y == WORLD_SIZE - 1 &&  x != WORLD_SIZE - 1 && which == EdgeDir::BOTTOM)
    {
        //bottom of map boundary (but not bottom right) creates bottom edge
        return base + 3;
    }
    //now on very bottom-right corner tile of map
    if(which == EdgeDir::RIGHT)
        return base + 3;
    if(which == EdgeDir::BOTTOM)
        return base + 4;
#ifdef MAGNATE_DEBUG
    throw runtime_error("Should not have gotten here!");
#endif
    return -1;
}

int Mesh::hmFaceIndex(int x, int y, FaceDir which)
{
    if(x < 0 || x >= WORLD_SIZE || y < 0 || y >= WORLD_SIZE)
        return -1;
    int base = 2 * (x + y * WORLD_SIZE);
    if(which == FaceDir::UPPER_LEFT)
        return base;
    else 
        return base + 1;
}

void Mesh::getNeighbors(int e, int& f1, int& f2, int& f11, int& f12, int& f21, int& f22)
{
    auto& edge = edges[e];
    f1 = edge.f[0];
    f2 = edge.f[1];
    int v1 = edge.v[0];
    int v2 = edge.v[1];
    if(!faces[f1].isClockwise(v1, v2))
    {
        SWAP(f1, f2);
        SWAP(edge.f[0], edge.f[1]);
    }
    getFaceNeighbors(f1, f2, f11, f12); //f11 to f12 is clockwise around f1
    getFaceNeighbors(f2, f1, f21, f22); //f21 to f22 is clockwise around f2
}

void Mesh::getFaceNeighbors(int f, int exclude, int& f1, int& f2)
{
    Face& face = faces[f];
    Face& excludeFace = faces[exclude];
    int excludeEdge = 0;
    while(!excludeFace.hasEdge(face.e[excludeEdge]))
    {
        excludeEdge++;
    }
    int e1 = (excludeEdge + 1) % 3;
    int e2 = (e1 + 1) % 3;
    //PRINTVAR(exclude);
    //PRINT("In getFaceNeighbors...");
    f1 = getOtherFace(f, e1);
    f2 = getOtherFace(f, e2);
    /*
    PRINTVAR(e1);
    PRINTVAR(e2);
    PRINTVAR(f1);
    PRINTVAR(f2);
    PRINT("Returning from getFaceNeighbors");
    */
} 

int Mesh::getOtherFace(int f, int e)
{
    Face& face = faces[f];
    Edge& edge = edges[face.e[e]];
    /*
    PRINT("In getOtherFace");
    PRINTVAR(f);
    PRINTVAR(e);
    PRINTVAR(edge.f[0]);
    PRINTVAR(edge.f[1]);*/
    int rv = edge.f[0] == f ? edge.f[1] : edge.f[0];
    //PRINTVAR(rv);
    //PRINT("getOtherFace ret");
    return rv;
}

int Mesh::getSharedEdge(int f1, int f2)
{
    for(int i = 0; i < 3; i++)
    {
        int edge = faces[f1].e[i];
        if(faces[f2].hasEdge(edge))
            return edge;
    }
#ifdef MAGNATE_DEBUG
    PRINT("Serious warning: expected faces " << f1 << " and " << f2 << " to share an edge but they don't. Returning -1.");
    throw runtime_error("shart");
#endif
    return -1;
}

void Mesh::replaceVertexLinks(int toReplace, int newLink)
{
    auto& vertex = vertices[toReplace];
    for(int edgeNum : vertex.edges)
    {
        auto& edge = edges[edgeNum];
        if(edge.v[0] == toReplace)
        {
            edge.v[0] = newLink;
        }
        else if(edge.v[1] == toReplace)
        {
            edge.v[1] = newLink;
        }
        //remove the edge completely if now degenerate
        //two faces will have references to that edge -- they will be removed later
        if(edge.v[0] == edge.v[1])
        {
            removeEdgeRefs(edgeNum);
            edges.free(edgeNum);
        }
        faces[edge.f[0]].replaceVertexLink(toReplace, newLink);
        faces[edge.f[0]].checkNormal();
        faces[edge.f[1]].replaceVertexLink(toReplace, newLink);
        faces[edge.f[1]].checkNormal();
    }
}

void Mesh::removeEdgeRefs(int e)
{
    auto& edge = edges[e];
    vertices[edge.v[0]].removeEdge(e);
    vertices[edge.v[1]].removeEdge(e);
}

bool minWorldX(vec3& loc)
{
    return loc.x < 1e-7;
}
bool maxWorldX(vec3& loc)
{
    return loc.x > WORLD_SIZE * Coord::TERRAIN_TILE_SIZE - 1e-7;

}
bool minWorldY(vec3& loc)
{
    return loc.z < 1e-7;
}
bool maxWorldY(vec3& loc)
{
    return loc.z > WORLD_SIZE * Coord::TERRAIN_TILE_SIZE - 1e-7;
}

std::ostream& operator<<(std::ostream& os, const MeshTypes::Face& face)
{
    os << "v: " << face.v[0] << " " << face.v[1] << " " << face.v[2] << "   ";
    return os << "e: " << face.e[0] << " " << face.e[1] << " " << face.e[2];
}

void Mesh::fullCorrectnessCheck()
{
    //check validity of all F => V and F => E links
    for(auto it = faces.begin(); it != faces.end(); it++)
    {
        for(int j = 0; j < 3; j++)
        {
            if(!validVertex(it->v[j]))
            {
                PRINT("Face " << it.loc << " has ref to vertex " << it->v[j] << " but vert not allocated!");
                throw exception();
            }
            if(!!validEdge(it->e[j]))
            {
                PRINT("Face " << it.loc << " has ref to edge " << it->e[j] << " but edge not allocated!");
                throw exception();
            }
        }
    }
    //check validity of all E => V and E => F links
    for(auto it = edges.begin(); it != edges.end(); it++)
    {
        for(int j = 0; j < 2; j++)
        {
            if(!validVertex(it->v[j]))
            {
                PRINT("Edge " << it.loc << " has ref to vertex " << it->v[j] << " but vert not allocated!");
                throw exception();
            }
            if(validEdge(it->f[j]))
            {
                PRINT("Edge " << it.loc << " has ref to edge " << it->f[j] << " but edge not allocated!");
                throw exception();
            }
        }
    }
    //check that all vertex adjacency lists include only valid edges, and that those edges link back to the vertex
    for(auto it = vertices.begin(); it != vertices.end(); it++)
    {
        for(auto adj : it->edges)
        {
            if(!validEdge(adj))
            {
                PRINT("Vertex " << it.loc << " has ref to edge " << adj << " but that edge not allocated!");
                throw exception();
            }
            if(!edges[adj].hasVert(it.loc))
            {
                PRINT("Vertex " << it.loc << " has ref to edge " << adj << " but that edge doesn't link back to the vertex!");
                throw exception();
            }
        }
    }
    //check that edges don't connect a vertex to itself, or have same face on both sides
    for(auto it = edges.begin(); it != edges.end(); it++)
    {
        if(it->v[0] == it->v[1])
        {
            PRINT("Edge " << it.loc << " links vertex " << it->v[0] << " to itself!");
            throw exception();
        }
        if(it->f[0] == it->f[1])
        {
            PRINT("Edge " << it.loc << " has same face " << it->f[0] << " on both sides!");
            throw exception();
        }
    }
    //check that faces don't have duplicate edges or vertices
    for(auto it = faces.begin(); it != faces.end(); it++)
    {
        if(it->v[0] == it->v[1] || it->v[1] == it->v[2] || it->v[0] == it->v[2] || it->e[0] == it->e[1] || it->e[1] == it->e[2] || it->e[0] == it->e[2])
        {
            PRINT("Face #" << it.loc << " contains a vertex or edge more than once!");
            PRINT("The face: " << faces[it.loc]);
            throw exception();
        }
    }
    //check for backwards-facing faces
    for(auto it = faces.begin(); it != faces.end(); it++)
    {
        if(it->getNorm().y < 0)
        {
            PRINT("Mesh error! Face " << it.loc << " facing wrong way!");
            throw exception();
        }
    }
}

bool Mesh::validFace(int f)
{
    return f >= 0 && f < faces.capacity && faces.isAllocated(f);
}
bool Mesh::validEdge(int e)
{
    return e >= 0 && e < edges.capacity && edges.isAllocated(e);
}
bool Mesh::validVertex(int v)
{
    return v >= 0 && v < vertices.capacity && vertices.isAllocated(v);
}

