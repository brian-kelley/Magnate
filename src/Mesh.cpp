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
    if(!hasEdge(e))
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

bool Vertex::hasEdge(int e)
{
    for(auto it : edges)
    {
        if(it == e)
            return true;
    }
    return false;
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

float Edge::getLength()
{
    auto& v1 = (*vertArray)[v[0]].pos;
    auto& v2 = (*vertArray)[v[1]].pos;
    return glm::distance(v1, v2);
}

void Edge::replaceVertexLink(int toReplace, int newLink)
{
    if(v[0] == toReplace)
    {
        v[0] = newLink;
        return;
    }
    else if(v[1] == toReplace)
    {
        v[1] = newLink;
        return;
    }
#ifdef MAGNATE_DEBUG
    throw runtime_error("Tried to replace a E => V link that didn't exist!");
#endif
}

void Edge::replaceFaceLink(int toReplace, int newLink)
{
    if(f[0] == toReplace)
        f[0] = newLink;
    else if(f[1] == toReplace)
        f[1] = newLink;
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
    DBASSERT(false);
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
        SWAP(v[0], v[1]);
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
    //Testing edge collapse
    simplify(faceMatchCutoff);
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
            Vertex v;
            v.pos = Coord::tileToWorld(x, heights.get(x, y), y).xyz();
            //set boundary/corner flags
            v.boundary = false;
            v.corner = false;
            if(y == 0 || y == WORLD_SIZE || x == 0 || x == WORLD_SIZE)
                v.boundary = true;
            if((y == 0 || y == WORLD_SIZE) && (x == 0 || x == WORLD_SIZE))
                v.corner = true;
            vertices.alloc(v);
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
            int eright = hmEdgeIndex(x, y, EdgeDir::RIGHT);
            int ebot = hmEdgeIndex(x, y, EdgeDir::BOTTOM);
            int f1 = hmFaceIndex(x, y, FaceDir::UPPER_LEFT);
            int f2 = hmFaceIndex(x, y, FaceDir::LOWER_RIGHT);
            int fleft = hmFaceIndex(x - 1, y, FaceDir::LOWER_RIGHT);
            int ftop = hmFaceIndex(x, y - 1, FaceDir::LOWER_RIGHT);
            //left, top, diagonal, right, bottom
            //left
            int eleft = edges.alloc(Edge(ulv, llv, f1, fleft));
            //top
            int etop = edges.alloc(Edge(ulv, urv, f1, ftop));
            //Diagonal edge
            int ediag = edges.alloc(Edge(urv, llv, f1, f2));
            //right, if on right edge of map
            if(x == WORLD_SIZE - 1)
                edges.alloc(Edge(urv, lrv, f2, -1));
            //bottom, if on bottom edge of map
            if(y == WORLD_SIZE - 1)
                edges.alloc(Edge(llv, lrv, f2, -1));
            //Faces (upper-left and lower-right)
            int faceval = faceValues.get(x, y); //both triangles have same value
            //upper-left
            faces.alloc(Face(ulv, urv, llv, eleft, etop, ediag, faceval));
            //lower-right
            faces.alloc(Face(urv, lrv, llv, ediag, eright, ebot, faceval));
        }
    }
}

void Mesh::simplify(float faceMatchCutoff)
{
    clock_t start = clock();
    int collapses = 1;
    while(collapses)
    {
        collapses = 0;
        for(auto it = edges.begin(); it != edges.end(); it++)
        {
            auto& edge = *it;
            //get unit normals for faces sharing this edge
            if(edge.f[0] != -1 && edge.f[1] != -1)
            {
                auto& f1 = faces[edge.f[0]];
                auto& f2 = faces[edge.f[1]];
                auto dotprod = dot(f1.getNorm(), f2.getNorm());
                if(dotprod < faceMatchCutoff)
                    continue;
            }
            if(edgeCollapse(it.loc))
            {
                collapses++;
            }
        }
        PRINT("Performed " << collapses << " collapses in " << double(clock() - start) / CLOCKS_PER_SEC << " s.");
        return;
    }
    //fullCorrectnessCheck();
}

bool Mesh::edgeCollapse(int edgeNum)
{
    //First do the eligibility checks
    if(!checkBoundaryBridge(edgeNum) ||
       !collapseConnectivity(edgeNum) ||
       !collapseTriangleSides(edgeNum))
        return false;
    auto& edge = edges[edgeNum];
    if(vertices[edge.v[0]].boundary && vertices[edge.v[1]].boundary)
        boundaryEdgeCollapse(edgeNum);
    else
        interiorEdgeCollapse(edgeNum);
    return true;
}

void Mesh::retriangulate(int vertexToRemove)
{
}

/* Mesh utility functions */

void Mesh::interiorEdgeCollapse(int edgeNum)
{
    //Deletes an edge, merges two pairs of edges, deletes 2 faces, and a vertex
    auto& edge = edges[edgeNum];
    int v1 = edge.v[0];
    int v2 = edge.v[1];
    if(vertices[v1].boundary && vertices[v2].boundary)
    {
        //make sure v1/v2 on same boundary, otherwise can't do collapse
        auto& pos1 = vertices[v1].pos;
        auto& pos2 = vertices[v2].pos;
        if(pos1.x != pos2.x && pos1.y != pos2.y)
            return;
    }
    int f1, f2, f11, f12, f21, f22, e11, e12, e21, e22;
    f1 = edge.f[0];
    f2 = edge.f[1];
    //Since is interior EC, f1 and f2 are guaranteed to be valid faces
    //First determine the indices (0, 1, or 2) the edges into faces[f1].e and faces[f2].e
    for(int i = 0; i < 3; i++)
    {
        if(faces[f1].e[i] != edgeNum)
            e11 = i;
        if(faces[f2].e[i] != edgeNum)
            e21 = i;
    }
    for(int i = 0; i < 3; i++)
    {
        if(i != e11 && faces[f1].e[i] != edgeNum)
            e12 = i;
        if(i != e21 && faces[f2].e[i] != edgeNum)
            e22 = i;
    }
    //while e{xx} are indices to face -> edge links, use getOtherFace to get f{xx}
    f11 = getOtherFace(f1, e11);
    f12 = getOtherFace(f1, e12);
    f21 = getOtherFace(f2, e21);
    f22 = getOtherFace(f2, e22);
    //now want e{xx} to be actual edge pointers
    e11 = faces[f1].e[e11];
    e12 = faces[f1].e[e12];
    e21 = faces[f2].e[e21];
    e22 = faces[f2].e[e22];
    //Fix triangle vertex ordering so that triangle flips can be
    //detected after the collapse
    //note: vertex ordering for faces doesn't affect edges at all
    //fixTriFlips(e12, e22);
    bool moveVertex = true; //by default, move remaining vertex to midpoint of old ones
    //don't want to move or remove a vertex on the map boundary
    if(vertices[v1].corner || (vertices[v1].boundary && !vertices[v2].corner))
    {
        //keep v1, delete v2 instead (just by swapping names)
        SWAP(v1, v2);
        moveVertex = false;
    }
    if(vertices[v2].boundary)
        moveVertex = false;
    replaceVertexLinks(v1, v2); //note: does not free v1, but does free edgeNum
    vertices[v2].boundary |= vertices[v1].boundary;
    vertices[v2].corner |= vertices[v1].corner;
    //e11.v == e12.v, e21.v == e22.v (only face links different, will change)
    if(moveVertex)
        vertices[v2].pos = (vertices[v1].pos + vertices[v2].pos) * 0.5f;
    //mesh no longer contains any links to v1, can safely delete it
    vertices.dealloc(v1);
    vertices[v2].removeEdge(edgeNum);
    //update fxx edge links
    if(f11 != -1)
        faces[f11].replaceEdgeLink(e11, e12);
    if(f11 != f21 && f21 != -1)
        faces[f21].replaceEdgeLink(e21, e22);
    //now e11 and e21 can be deleted
    if(e11 != -1)
        fullyDeleteEdge(e11);
    if(e21 != -1)
        fullyDeleteEdge(e21);
    //prepare to remove f1/f2 by updating e12/e22 face links
    edges[e12].replaceFaceLink(f1, f11);
    edges[e22].replaceFaceLink(f2, f21);
    //nothing links to f1, f2 anymore, delete them
    if(f1 != -1)
        faces.dealloc(f1);
    if(f2 != -1)
        faces.dealloc(f2);
    fixTriFlips(e12, e22);
    /*
    if(!checkFlips(e12, e22))
    {
        retriangulate(v2);
    }
    */
}

void Mesh::boundaryEdgeCollapse(int e)
{
    auto& edge = edges[e];
    int f = edge.f[0];
    if(f == -1)
        f = edge.f[1];
    Face& face = faces[f];
    int v1 = edge.v[0];
    int v2 = edge.v[1];
    //Already know on boundary because edge only has one face adjacent
    //  but must check that at most one of v1, v2 is in a corner
    //  because can't collapse the boundary of the mesh
    //Also decide whether to move the merged vertex
    bool v1corner = vertices[v1].corner;
    bool v2corner = vertices[v2].corner;
    if(v1corner && v2corner)
        return;
    bool moveVertex = !(v1corner || v2corner);
    int e1, e2, f1, f2;
    for(int i = 0; i < 3; i++)
    {
        if(face.e[i] != e)
            e1 = i;
    }
    for(int i = 0; i < 3; i++)
    {
        if(face.e[i] != e && face.e[i] != face.e[e1])
            e2 = i;
    }
    //do tri flip fix before doing collapse so it can be detected later
    //fixTriFlips(e2, -1);
    //have indices within Face::e of e1, e2 (NOT the actual pointers)
    f1 = getOtherFace(f, e1);
    f2 = getOtherFace(f, e2);
    //now e1 and e2 are more useful as actual values
    e1 = faces[f].e[e1];
    e2 = faces[f].e[e2];
    //v1 might be moved, v2 will be deleted
    //don't want to delete a corner vertex
    if(v2corner)
        SWAP(v1, v2);
    replaceVertexLinks(v2, v1); //fixes all E -> V and F -> V links
    vertices[v1].boundary |= vertices[v2].boundary;
    vertices[v1].corner |= vertices[v2].corner;
    if(moveVertex)
        vertices[v1].pos = (vertices[v1].pos + vertices[v2].pos) / 2.0f;
    //e2 will be merged onto e1, and e1 will be deleted
    if(f1 != -1)
        faces[f1].replaceEdgeLink(e1, e2);
    fullyDeleteEdge(e1);
    edges[e2].replaceFaceLink(f, f1);
    faces.dealloc(f);
    /*
    if(!checkFlips(e2, -1))
    {
        retriangulate(v1);
    }
    */
    fixTriFlips(e2, -1);
}

bool Mesh::faceValuesCheck(int edgeNum)
{
    auto& edge = edges[edgeNum];
    int v1 = edge.v[0];
    int v2 = edge.v[1];
    //make sure that all triangles containing v1 or v2 have the same face value
    int goodFace = edge.f[0] == -1 ? edge.f[1] : edge.f[0];
    auto goodVal = faces[goodFace].value;
    //if a single face scalar is different than
    //goodVal in the fans around v1 and v2, return false
    for(auto adj : vertices[v1].edges)
    {
        //determine the indices of the faces to check
        DBASSERT(edges.isAllocated(adj))
        int check1 = edges[adj].f[0];
        int check2 = edges[adj].f[1];
        if(check1 != -1 && faces[check1].value != goodVal)
            return false;
        if(check2 != -1 && faces[check2].value != goodVal)
            return false;
    }
    for(auto adj : vertices[v2].edges)
    {
        //determine the indices of the faces to check
        DBASSERT(edges.isAllocated(adj))
        int check1 = edges[adj].f[0];
        int check2 = edges[adj].f[1];
        if(check1 != -1 && faces[check1].value != goodVal)
            return false;
        if(check2 != -1 && faces[check2].value != goodVal)
            return false;
    }
    return true;
}

bool Mesh::collapseConnectivity(int edge)
{
    int v1 = edges[edge].v[0];
    int v2 = edges[edge].v[1];
    //Check that at most, 3 pairs of edges link v1 to v2
    //this is O(degree^2), there might be a better way (but it probably doesn't matter)
    int connections = 0;
    for(auto a1 : vertices[v1].edges)
    {
        for(auto a2 : vertices[v2].edges)
        {
            if(a1 == a2)
                connections++;
        }
    }
    return connections <= 3;
}

bool Mesh::collapseTriangleSides(int edge)
{
    float edgeLen = edges[edge].getLength();
    //edgeLen must be shorter than all other edges
    int faceIndices[2] = {edges[edge].f[0], edges[edge].f[1]};
    for(int i = 0; i < 2; i++)
    {
        if(faceIndices[i] == -1)
            continue;
        Face& f = faces[faceIndices[i]];
        for(int j = 0; j < 3; j++)
        {
            if(f.e[j] == edge)
                continue;
            if(edgeLen > edges[f.e[j]].getLength())
                return false;
        }
    }
    return true;
}

bool Mesh::checkBoundaryBridge(int edge)
{
    auto& v1 = vertices[edges[edge].v[0]];
    auto& v2 = vertices[edges[edge].v[1]];
    if(!(v1.boundary && v2.boundary))
        return true;    //at least one not on boundary, ok
    if(v1.corner && v2.corner)
        return false;   //bad!
    if(v1.corner || v2.corner)
        return true;    //ok, if one is in corner, can't bridge across boundaries
    //v1, v2 both on boundary, and neither on a corner
    //v1 and v2 either need to have the same x or the same z, otherwise bad
    if(v1.pos[0] != v2.pos[0] && v1.pos[2] != v2.pos[2])
        return false;
    return true;
}

void Mesh::fixTriFlips(int e1, int e2)
{
    //Check that both faces next to edge still face right way
    int checkEdges[2] = {e1, e2};
    int checkFaces[2];
    for(int i = 0; i < 2; i++)
    {
        if(checkEdges[i] == -1)
            continue;
        checkFaces[0] = edges[checkEdges[i]].f[0];
        checkFaces[1] = edges[checkEdges[i]].f[1];
        for(int j = 0; j < 2; j++)
        {
            if(checkFaces[j] == -1)
                continue;
            Face& f = faces[checkFaces[j]];
            f.checkNormal();
        }
    }
}

bool Mesh::checkFlips(int e1, int e2)
{
    //Check that both faces next to edge still face right way
    int checkEdges[2] = {e1, e2};
    int checkFaces[2];
    for(int i = 0; i < 2; i++)
    {
        if(checkEdges[i] == -1)
            continue;
        checkFaces[0] = edges[checkEdges[i]].f[0];
        checkFaces[1] = edges[checkEdges[i]].f[1];
        for(int j = 0; j < 2; j++)
        {
            if(checkFaces[j] == -1)
                continue;
            Face& f = faces[checkFaces[j]];
            if(f.getNorm().y < 0)
                return false;
        }
    }
    return true;
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
    if(f1 != -1)
    {
        getFaceNeighbors(f1, f2, f11, f12); //f11 to f12 is clockwise around f1
    }
    else
    {
        f11 = -1;
        f12 = -1;
    }
    if(f2 != -1)
    {
        getFaceNeighbors(f2, f1, f21, f22); //f21 to f22 is clockwise around f2
    }
    else
    {
        f21 = -1;
        f22 = -1;
    }
}

void Mesh::getBoundaryNeighbors(int e, int& f1, int& f2, int& f11, int& f12, int& f21, int& f22)
{
    auto& edge = edges[e];
    f1 = edge.f[0];
    f2 = edge.f[1];
    if(f1 == -1)
    {
        f11 = -1;
        f12 = -1;
    }
    else
    {
        getFaceNeighbors(f1, f2, f11, f12);
    }
    if(f2 == -1)
    {
        f21 = -1;
        f22 = -1;
    }
    else
    {
        getFaceNeighbors(f2, f1, f21, f22);
    }
}

void Mesh::getFaceNeighbors(int f, int exclude, int& f1, int& f2)
{
    int excludeEdge = 0;
    //ok for exclude = -1
    while(getOtherFace(f, excludeEdge) != exclude)
    {
        excludeEdge++;
    }
    int e1 = (excludeEdge + 1) % 3;
    int e2 = (e1 + 1) % 3;
    f1 = getOtherFace(f, e1);
    f2 = getOtherFace(f, e2);
} 

int Mesh::getOtherFace(int f, int e)
{
    Edge& edge = edges[faces[f].e[e]];
    int rv = edge.f[0] == f ? edge.f[1] : edge.f[0];
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
    for(auto it = vertex.edges.begin(); it != vertex.edges.end(); it++)
    {
        int edgeNum = *it;
        auto& edge = edges[edgeNum];
        if(edge.f[0] != -1)
        {
            faces[edge.f[0]].replaceVertexLink(toReplace, newLink);
            faces[edge.f[0]].checkNormal();
        }
        if(edge.f[1] != -1)
        {
            faces[edge.f[1]].replaceVertexLink(toReplace, newLink);
            faces[edge.f[1]].checkNormal();
        }
        edge.replaceVertexLink(toReplace, newLink);
        vertices[newLink].addEdge(*it);
        //remove the edge completely if now degenerate
        //two faces will have references to that edge -- they will be removed later
        if(edge.v[0] == edge.v[1])
        {
            //removeEdgeRefs(edgeNum);
            edges.dealloc(edgeNum);
        }
    }
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

std::ostream& operator<<(std::ostream& os, const MeshTypes::Edge& edge)
{
    os << "v: " << edge.v[0] << " " << edge.v[1] << " ";
    return os << "f: " << edge.f[0] << " " << edge.f[1];
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
            if(!validEdge(it->e[j]))
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
            if(!validFace(it->f[j]) && it->f[j] != -1)  //-1 is allowed
            {
                PRINT("Edge " << it.loc << " has ref to face " << it->f[j] << " but face not allocated!");
                PRINT("The edge: " << *it);
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

void Mesh::fullyDeleteEdge(int e)
{
    int v1 = edges[e].v[0];
    int v2 = edges[e].v[1];
    if(vertices.isAllocated(v1))
    {
        vertices[v1].removeEdge(e);
    }
    if(vertices.isAllocated(v2))
    {
        vertices[v2].removeEdge(e);
    }
    edges.dealloc(e);
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

void Mesh::deepTest(Heightmap& heights, Heightmap& faceValues)
{
    simpleLoadHeightmap(heights, faceValues);
    int testEdges = edges.size;
    faces.clear();
    edges.clear();
    vertices.clear();
    for(int test = 0; test < testEdges; test++)
    {
        simpleLoadHeightmap(heights, faceValues);
        edgeCollapse(test);
        fullCorrectnessCheck();
    }
    PRINT("*** Tests on all edges passed ***");
}
