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

#define INVALID -1  //invalid vert, edge, or face
#define OUTSIDE -2  //face outside the mesh

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
    for(int i = edges.size() - 1; i >= 0; i--)
    {
        if(edges[i] == e)
            edges.erase(edges.begin() + i);
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

int Vertex::connectsTo(int vert)
{
    for(auto edge : edges)
    {
        if((*Edge::edgeArray)[edge].v[0] == vert ||
           (*Edge::edgeArray)[edge].v[1] == vert)
            return edge;
    }
    return INVALID;
}

Edge::Edge()
{
    v[0] = INVALID;
    v[1] = INVALID;
    f[0] = INVALID;
    f[1] = INVALID;
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
        v[i] = INVALID;
        e[i] = INVALID;
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

vec3 Face::getNorm()
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
    int v1loc = INVALID;
    int v2loc = INVALID;
    for(int i = 0; i < 3; i++)
    {
        if(v[i] == v1)
        {
            v1loc = i;
            break;
        }
    }
    if(v1loc == INVALID)
        return false;
    for(int i = 0; i < 3; i++)
    {
        if(v[i] == v2)
        {
            v2loc = i;
            break;
        }
    }
    if(v2loc == INVALID)
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
    //simplify(0.99);
    //fullCorrectnessCheck();
    /*
    for(int i = 3; i < 20; i++)
    {
        for(int j = 3; j < 40; j++)
            removeAndRetriangulate(hmVertIndex(i, j));
    }
    */
    srand(42);
    vector<int> verts;
    verts.reserve(17 * 17);
    for(int i = 3; i < 30; i++)
    {
        for(int j = 3; j < 30; j++)
            verts.push_back(hmVertIndex(i, j));
    }
    std::random_shuffle(verts.begin(), verts.end());
    for(int i = 0; i < 300; i++)
        removeAndRetriangulate(verts[i]);
    fullCorrectnessCheck();
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
    int totalIters = 0;
    while(collapses)
    {
        if(totalIters++ == 2)
            return;
        collapses = 0;
        for(auto it = edges.begin(); it != edges.end(); it++)
        {
            auto& edge = *it;
            //get unit normals for faces sharing this edge
            if(edge.f[0] >= 0 && edge.f[1] >= 0)
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
                //printf("%i collapses\n", collapses);
            }
        }
        PRINT("Performed " << collapses << " collapses in " << double(clock() - start) / CLOCKS_PER_SEC << " s.");
        PRINT("Fixing flipped triangles; detecting and fixing overlaps...");
        for(auto it = faces.begin(); it != faces.end(); it++)
        {
            it->checkNormal();
        }
        for(auto it = vertices.begin(); it != vertices.end(); it++)
        {
            if(isTriClique(it.loc))
                removeTriClique(it.loc);
        }
        PRINT("Done handling tri cliques.");
        for(auto it = edges.begin(); it != edges.end(); it++)
        {
            //use the fact that the upward curl of faces should be in opposing directions along an edge
            if(facesWrongOrientation(it.loc))
            {
                PRINT("Found incorrect fold at edge " << it.loc);
                //Overlap, retriangulate the area
                if(!vertices[it->v[0]].boundary)
                    removeAndRetriangulate(it->v[0]);
                else if(!vertices[it->v[1]].boundary)
                    removeAndRetriangulate(it->v[1]);
                else
                    throw runtime_error("Overlapping faces on map boundary can't be repaired!");
            }
        }
        for(auto it = vertices.begin(); it != vertices.end(); it++)
        {
            if(isTriClique(it.loc))
                removeTriClique(it.loc);
        }
    }
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

void Mesh::removeAndRetriangulate(int vertexToRemove)
{
    Vertex& vert = vertices[vertexToRemove];
    if(vert.boundary)
        throw runtime_error("Can't remove a vertex on the boundary.");
    //create a list of faces to delete (duplicates ok)
    vector<int> facesToRemove;
    {
        for(auto edge : vert.edges)
        {
            if(edge >= 0)
            {
                for(int i = 0; i < 2; i++)
                {
                    if(edges[edge].f[i] >= 0)
                        facesToRemove.push_back(edges[edge].f[i]);
                }
            }
        }
        std::sort(facesToRemove.begin(), facesToRemove.end());
        auto newEnd = std::unique(facesToRemove.begin(), facesToRemove.end());
        facesToRemove.erase(newEnd, facesToRemove.end());
    }
    auto terrainValue = faces[facesToRemove.front()].value;
    //Now create a list of the edges in those faces that don't include vertexToRemove
    //(will not have duplicates)
    vector<int> edgeBoundary;
    {
        for(auto face : facesToRemove)
        {
            if(face >= 0)
            {
                for(int i = 0; i < 3; i++)
                {
                    Edge& edge = edges[faces[face].e[i]];
                    bool alreadyAdded = false;
                    if(edge.v[0] != vertexToRemove && edge.v[1] != vertexToRemove)
                    {
                        for(size_t j = 0; j < edgeBoundary.size(); j++)
                        {
                            if(edgeBoundary[j] == faces[face].e[i])
                            {
                                alreadyAdded = true;
                                break;
                            }
                        }
                        if(!alreadyAdded)
                            edgeBoundary.push_back(faces[face].e[i]);
                    }
                }
            }
        }
    }
    //Check for abort condition before changing any geometry
    if(edgeBoundary.size() < 4 || edgeBoundary.size() > 40)
        return;
    //delete vertexToRemove
    vertices.dealloc(vertexToRemove);
    //delete all faces containing vertexToRemove
    int numFaces = 0;
    for(auto faceToDelete : facesToRemove)
    {
        if(faceToDelete >= 0)
        {
            faces.dealloc(faceToDelete);
            numFaces++;
        }
    }
    //delete all edges leading to vertexToRemove
    for(auto boundEdge : edgeBoundary)
    {
        //note: these vertices are guaranteed to be valid
        Vertex* check[2] = {&vertices[edges[boundEdge].v[0]],
                            &vertices[edges[boundEdge].v[1]]};
        for(int i = 0; i < 2; i++)
        {
            int connection = check[i]->connectsTo(vertexToRemove);
            if(connection != INVALID)
            {
                check[i]->removeEdge(connection);
                edges.dealloc(connection);
            }
        }
    }
    //fix E -> F links along boundary (make links to deleted faces -1)
    for(auto boundEdge : edgeBoundary)
    {
        int facesToCheck[2] = {edges[boundEdge].f[0], edges[boundEdge].f[1]};
        for(int i = 0; i < 2; i++)
        {
            if(facesToCheck[i] >= 0 && !faces.isAllocated(facesToCheck[i]))
                edges[boundEdge].replaceFaceLink(facesToCheck[i], INVALID);
        }
    }
    vector<int> vertLoop;
    vertLoop.reserve(32);
    {
        int startVert = edges[edgeBoundary[0]].v[0];
        int vertIt = startVert;
        int lastEdge = INVALID;
        do
        {
            if(vertIt == vertLoop.back())
                break;
            vertLoop.push_back(vertIt);
            for(auto e : vertices[vertIt].edges)
            {
                if((edges[e].f[0] == INVALID || edges[e].f[1] == INVALID) && e != lastEdge)
                {
                    //have the next edge in loop
                    lastEdge = e;
                    if(edges[e].v[0] == vertIt)
                        vertIt = edges[e].v[1];
                    else
                        vertIt = edges[e].v[0];
                    break;
                }
            }
        }
        while (vertIt != startVert);
    }
    for(size_t i = 0; i < vertLoop.size(); i++)
    {
        DBASSERT(vertices[vertLoop[i]].connectsTo(vertLoop[(i + 1) % vertLoop.size()]) != -1);
        DBASSERT(vertices[vertLoop[(i + 1) % vertLoop.size()]].connectsTo(vertLoop[i]));
    }
    //vertLoop must be clockwise geometrically, check for that
    if(!isLoopOrientedUp(vertLoop))
    {
        for(size_t i = 0; i < vertLoop.size() / 2; i++)
            SWAP(vertLoop[i], vertLoop[vertLoop.size() - 1 - i]);
    }
    retriangulate(vertLoop, terrainValue);
}

void Mesh::retriangulate(vector<int>& vertLoop, int terrainVal)
{
    //precondition: vertLoop is simply connected loop, oriented clockwise
    auto vertDist = [&] (const pair<int, int>& possibleEdge) -> float
    {
        float dist = glm::length(vertices[possibleEdge.first].pos - vertices[possibleEdge.second].pos);
        return dist;
    };
    //operator > for edges
    auto edgeCmp = [&] (const pair<int, int>& lhs, const pair<int, int>& rhs) -> bool
    {
        return vertDist(lhs) > vertDist(rhs);
    };
    auto edgesCross = [&] (pair<int, int>& lhs, pair<int, int>& rhs) -> bool
    {
        return Geom2D::lineSegmentsCross(vertices[lhs.first].pos.xz(), vertices[lhs.second].pos.xz(),
                                         vertices[rhs.first].pos.xz(), vertices[rhs.second].pos.xz());
    };
    auto edgeCrossesPrev = [&] (pair<int, int>& possible, vector<pair<int, int>>& addedEdges) -> bool
    {
        for(auto& edge : addedEdges)
        {
            if(edgesCross(possible, edge))
                return true;
        }
        return false;
    };
    //make a list of geometrically valid new edges (not already connected, ie not adjacent in vertLoop)
    vector<pair<int, int>> validEdges;
    vector<pair<int, int>> addedEdges;
    validEdges.reserve((vertLoop.size() * (vertLoop.size() - 2)) / 2);
    for(size_t i = 0; i < vertLoop.size() - 2; i++)
    {
        for(size_t j = i + 2; j < vertLoop.size(); j++)
        {
            int v1 = vertLoop[i];
            int v2 = vertLoop[j];
            if(vertices[v1].connectsTo(v2) == INVALID)
                validEdges.push_back({vertLoop[i], vertLoop[j]});
        }
    }
    //sort possible edges based on distance between vertices
    sort(validEdges.begin(), validEdges.end(), edgeCmp);
    //validEdges now in descending order by length 
    int trisToMake = vertLoop.size() - 2;
    int trisAdded = 0;
    while(trisAdded < trisToMake)
    {
        //select shortest possible edge that doesn't cross any previous edges
        //create the edge
        //determine whether new faces should be created with the edge
        pair<int, int> newEdge;
        do
        {
            if(validEdges.size() == 0)
                return;
            DBASSERT(validEdges.size() > 0);
            newEdge = validEdges.back();
            validEdges.pop_back();
        }
        while(edgeCrossesPrev(newEdge, addedEdges));
        //reject edge if collinear
        int c1, c2;
        getMutualConnections(newEdge.first, newEdge.second, c1, c2);
        if(c1 != INVALID)
        {
            if(verticesCollinear(newEdge.first, newEdge.second, c1))
                continue;
        }
        if(c2 != INVALID)
        {
            if(verticesCollinear(newEdge.first, newEdge.second, c2))
                continue;
        }
        //check for bad orientation
        if(!retriEdgeOrientation(newEdge.first, newEdge.second, vertLoop))
            continue;
        //create the new edge
        int ei = edges.alloc();
        Edge& edge = edges[ei];
        addedEdges.push_back(newEdge);
        int v1 = newEdge.first;
        int v2 = newEdge.second;
        edge.v[0] = v1;
        edge.v[1] = v2;
        vertices[edge.v[0]].addEdge(ei);
        vertices[edge.v[1]].addEdge(ei);
        edge.f[0] = INVALID;
        edge.f[1] = INVALID;
        //edge complete, now check both sides for a newly closed triangle 
        int mutualVerts[2];
        int newFaces[2];
        int numNewTris = 0;
        {
            if(c1 != INVALID)
                mutualVerts[numNewTris++] = c1;
            if(c2 != INVALID)
                mutualVerts[numNewTris++] = c2;
        }
        //check for 4-vert clique case (don't create two triangles on same side of edge)
        if(numNewTris == 2)
        {
            vec2 ehead = vertices[edge.v[0]].pos.xz();
            vec2 etail = vertices[edge.v[1]].pos.xz();
            vec2 p1 = vertices[c1].pos.xz();
            vec2 p2 = vertices[c2].pos.xz();
            //know points aren't collinear already
            if(Geom2D::pointLineSide(p1, etail, ehead) == Geom2D::pointLineSide(p2, etail, ehead))
            {
                //p1, p2 on same side of edge, can only create 1 face
                //decide which point is closer to the edge
                float p1dist = Geom2D::pointLineDist(p1, etail, ehead);
                float p2dist = Geom2D::pointLineDist(p2, etail, ehead);
                //p1 dist, p2 dist both > 0 (not collinear, and distance always nonnegative)
                if(p1dist > p2dist)
                    mutualVerts[0] = mutualVerts[1];
                numNewTris = 1;
            }
        }
        for(int i = 0; i < numNewTris; i++)
        {
            int v3 = mutualVerts[i];
            int fi = faces.alloc();
            newFaces[i] = fi;
            Face& face = faces[fi];
            trisAdded++;
            face.v[0] = v1;
            face.v[1] = v2;
            face.v[2] = v3;
            face.checkNormal();
            face.e[0] = ei;
            face.e[1] = vertices[v1].connectsTo(v3);
            face.e[2] = vertices[v2].connectsTo(v3);
            face.value = terrainVal;
            //set up edge links to new face
            edges[face.e[0]].replaceFaceLink(INVALID, fi);
            edges[face.e[1]].replaceFaceLink(INVALID, fi);
            edges[face.e[2]].replaceFaceLink(INVALID, fi);
        }
    }
}

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
    fixTriFlips(e12, e22);
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
    if(f11 >= 0)
        faces[f11].replaceEdgeLink(e11, e12);
    if(f11 != f21 && f21 >= 0)
        faces[f21].replaceEdgeLink(e21, e22);
    //now e11 and e21 can be deleted
    if(e11 != INVALID)
        fullyDeleteEdge(e11);
    if(e21 != INVALID)
        fullyDeleteEdge(e21);
    //prepare to remove f1/f2 by updating e12/e22 face links
    edges[e12].replaceFaceLink(f1, f11);
    edges[e22].replaceFaceLink(f2, f21);
    //nothing links to f1, f2 anymore, delete them
    if(f1 >= 0)
        faces.dealloc(f1);
    if(f2 >= 0)
        faces.dealloc(f2);
    fixTriFlips(e12, e22);
    if(isTriClique(v2))
        removeTriClique(v2);
    /*
    if(!checkFlips(e12, e22))
    {
        //PRINT("Removing + retriangulating " << v2);
        retriangulate(v2);
    }
    */
}

void Mesh::boundaryEdgeCollapse(int e)
{
    auto& edge = edges[e];
    int f = edge.f[0];
    if(f < 0)
        f = edge.f[1];
    //f = the only valid face of edge to collapse
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
    vertices[v1].boundary |= vertices[v2].boundary;
    vertices[v1].corner |= vertices[v2].corner;
    if(moveVertex)
        vertices[v1].pos = (vertices[v1].pos + vertices[v2].pos) / 2.0f;
    //e2 will be merged onto e1, and e1 will be deleted
    if(f1 != -1)
        faces[f1].replaceEdgeLink(e1, e2);
    edges[e2].replaceFaceLink(f, f1);
    //remove V->E links to edge e
    replaceVertexLinks(v2, v1); //fixes all E -> V and F -> V links, deleted edge
    vertices.dealloc(v2);
    vertices[v1].removeEdge(e);
    fullyDeleteEdge(e1);
    faces.dealloc(f);
    fixTriFlips(e2, INVALID);
}

bool Mesh::isTriClique(int vertex)
{
    return vertex >= 0 && vertices[vertex].edges.size() == 3;
}

void Mesh::removeTriClique(int vertex)
{
    if(vertices[vertex].boundary)
        return;
    PRINT("Removing tri clique at vertex " << vertex);
    Vertex& vert = vertices[vertex];
    DBASSERT(vert.edges.size() == 3);
    int v[3];
    for(int i = 0; i < 3; i++)
    {
        Edge& e = edges[vert.edges[i]];
        if(e.v[0] == vertex)
            v[i] = e.v[1];
        else
            v[i] = e.v[0];
    }
    DBASSERT(vertices[v[0]].connectsTo(v[1]) != -1);
    DBASSERT(vertices[v[1]].connectsTo(v[2]) != -1);
    DBASSERT(vertices[v[0]].connectsTo(v[2]) != -1);
    DBASSERT(vert.connectsTo(v[0]) != -1);
    DBASSERT(vert.connectsTo(v[1]) != -1);
    DBASSERT(vert.connectsTo(v[2]) != -1);
    int deleteF1 = getFaceFrom3Vertices(vertex, v[0], v[1]);
    int deleteF2 = getFaceFrom3Vertices(vertex, v[1], v[2]);
    int keep = getFaceFrom3Vertices(vertex, v[0], v[2]);
    faces[keep].replaceVertexLink(vertex, v[1]);
    int deleteE1 = vert.connectsTo(v[0]);
    int deleteE2 = vert.connectsTo(v[1]);
    int deleteE3 = vert.connectsTo(v[2]);
    int newE1 = vertices[v[0]].connectsTo(v[1]);
    int newE2 = vertices[v[1]].connectsTo(v[2]);
    faces[keep].replaceEdgeLink(deleteE1, newE1);
    faces[keep].replaceEdgeLink(deleteE3, newE2);
    edges[newE1].replaceFaceLink(deleteF1, keep);
    edges[newE2].replaceFaceLink(deleteF2, keep);
    vertices[v[0]].removeEdge(deleteE1);
    vertices[v[1]].removeEdge(deleteE2);
    vertices[v[2]].removeEdge(deleteE3);
    vertices.dealloc(vertex);
    edges.dealloc(deleteE1);
    edges.dealloc(deleteE2);
    edges.dealloc(deleteE3);
    faces.dealloc(deleteF1);
    faces.dealloc(deleteF2);
    faces[keep].checkNormal();
}

bool Mesh::faceValuesCheck(int edgeNum)
{
    auto& edge = edges[edgeNum];
    int v1 = edge.v[0];
    int v2 = edge.v[1];
    //make sure that all triangles containing v1 or v2 have the same face value
    int goodFace = edge.f[0] < 0 ? edge.f[1] : edge.f[0];
    auto goodVal = faces[goodFace].value;
    //if a single face scalar is different than
    //goodVal in the fans around v1 and v2, return false
    for(auto adj : vertices[v1].edges)
    {
        //determine the indices of the faces to check
        DBASSERT(edges.isAllocated(adj))
        int check1 = edges[adj].f[0];
        int check2 = edges[adj].f[1];
        if(check1 >= 0 && faces[check1].value != goodVal)
            return false;
        if(check2 >= 0 && faces[check2].value != goodVal)
            return false;
    }
    for(auto adj : vertices[v2].edges)
    {
        //determine the indices of the faces to check
        DBASSERT(edges.isAllocated(adj))
        int check1 = edges[adj].f[0];
        int check2 = edges[adj].f[1];
        if(check1 >= 0 && faces[check1].value != goodVal)
            return false;
        if(check2 >= 0 && faces[check2].value != goodVal)
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
        if(faceIndices[i] < 0)
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
        if(checkEdges[i] == INVALID)
            continue;
        checkFaces[0] = edges[checkEdges[i]].f[0];
        checkFaces[1] = edges[checkEdges[i]].f[1];
        for(int j = 0; j < 2; j++)
        {
            if(checkFaces[j] < 0)
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
        if(checkEdges[i] == INVALID)
            continue;
        checkFaces[0] = edges[checkEdges[i]].f[0];
        checkFaces[1] = edges[checkEdges[i]].f[1];
        for(int j = 0; j < 2; j++)
        {
            if(checkFaces[j] < 0)
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
    return INVALID;
}

int Mesh::hmFaceIndex(int x, int y, FaceDir which)
{
    if(x < 0 || x >= WORLD_SIZE || y < 0 || y >= WORLD_SIZE)
        return OUTSIDE;
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
    if(f1 >= 0)
    {
        getFaceNeighbors(f1, f2, f11, f12); //f11 to f12 is clockwise around f1
    }
    else
    {
        f11 = INVALID;
        f12 = INVALID;
    }
    if(f2 >= 0)
    {
        getFaceNeighbors(f2, f1, f21, f22); //f21 to f22 is clockwise around f2
    }
    else
    {
        f21 = INVALID;
        f22 = INVALID;
    }
}

void Mesh::getBoundaryNeighbors(int e, int& f1, int& f2, int& f11, int& f12, int& f21, int& f22)
{
    auto& edge = edges[e];
    f1 = edge.f[0];
    f2 = edge.f[1];
    if(f1 < 0)
    {
        f11 = INVALID;
        f12 = INVALID;
    }
    else
    {
        getFaceNeighbors(f1, f2, f11, f12);
    }
    if(f2 < 0)
    {
        f21 = INVALID;
        f22 = INVALID;
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
    string err = string("Serious warning: expected faces ") + to_string(f1) + " and " + to_string(f2) + " to share an edge but they don't. Returning INVALID.";
    throw runtime_error(err);
}

void Mesh::replaceVertexLinks(int toReplace, int newLink)
{
    auto& vertex = vertices[toReplace];
    for(auto it = vertex.edges.begin(); it != vertex.edges.end(); it++)
    {
        int edgeNum = *it;
        auto& edge = edges[edgeNum];
        if(edge.f[0] >= 0)
        {
            faces[edge.f[0]].replaceVertexLink(toReplace, newLink);
            faces[edge.f[0]].checkNormal();
        }
        if(edge.f[1] >= 0)
        {
            faces[edge.f[1]].replaceVertexLink(toReplace, newLink);
            faces[edge.f[1]].checkNormal();
        }
        edge.replaceVertexLink(toReplace, newLink);
        vertices[newLink].addEdge(*it);
        if(edge.v[0] == edge.v[1])
        {
            //removeEdgeRefs(edgeNum);
            edges.dealloc(edgeNum);
        }
    }
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

std::ostream& operator<<(std::ostream& os, const MeshTypes::Vertex& vert)
{
    os << "connections: " << vert.edges << " ";
    if(vert.boundary)
        os << "[boundary]";
    if(vert.corner)
        os << "[corner]";
    return os;
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
        if(verticesCollinear(it->v[0], it->v[1], it->v[2]))
        {
            PRINT("Face " << it.loc << " has collinear vertices (triangle has 0 area).");
            throw exception();
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
            if(!validFace(it->f[j]) && it->f[j] >= 0)
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
    //check that adjacency lists for duplicates
    for(auto it = vertices.begin(); it != vertices.end(); it++)
    {
        for(size_t i = 0; i < it->edges.size(); i++)
        {
            for(size_t j = 0; j < it->edges.size(); j++)
            {
                if(i == j)
                    continue;
                if(it->edges[i] == it->edges[j])
                {
                    PRINT("Vertex " << it.loc << " has duplicated edge: " << it->edges[i]);
                    throw exception();
                }
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

int Mesh::getFaceFrom2Vertices(int v1, int v2)
{
    //assume there is only one face containing v1, v2
    //also assume that v1 and v2 are connected
    int e = vertices[v1].connectsTo(v2);
    return edges[e].f[0] != INVALID ? edges[e].f[0] : edges[e].f[1];
}

int Mesh::getFaceFrom3Vertices(int v1, int v2, int v3)
{
    int e = vertices[v1].connectsTo(v2);
    if(e == INVALID)
        return INVALID;
    for(int i = 0; i < 2; i++)
    {
        int f = edges[e].f[i];
        if(f >= 0 && faces[f].hasVert(v3))
            return f;
    }
    return INVALID;
}

void Mesh::getMutualConnections(int vert1, int vert2, int& c1, int& c2)
{
    Vertex& v1 = vertices[vert1];
    Vertex& v2 = vertices[vert2];
    c1 = INVALID;
    c2 = INVALID;
    bool foundOne = false;
    for(auto edge1 : v1.edges)
    {
        for(auto edge2 : v2.edges)
        {
            //now if e1, e2 share any one vertex, set c1 or c2 to it
            Edge& e1 = edges[edge1];
            Edge& e2 = edges[edge2];
            int shared = INVALID;
            if(e1.v[0] == e2.v[0])
                shared = e1.v[0];
            else if(e1.v[1] == e2.v[0])
                shared = e1.v[1];
            else if(e1.v[0] == e2.v[1])
                shared = e1.v[0];
            else if(e1.v[1] == e2.v[1])
                shared = e1.v[1];
            if(shared != INVALID && shared != vert1 && shared != vert2)
            {
                if(!foundOne)
                {
                    foundOne = true;
                    c1 = shared;
                }
                else
                {
                    c2 = shared;
                    return;
                }
            }
        }
    }
}

bool Mesh::faceExists(int v1, int v2, int v3)
{
    int c1 = vertices[v1].connectsTo(v2);
    if(c1 == -1)
        return false;
    int f1 = edges[c1].f[0];
    int f2 = edges[c1].f[0];
    if(f1 >= 0 && faces[f1].hasVert(v3))
        return true;
    if(f2 >= 0 && faces[f2].hasVert(v3))
        return true;
    return false;
}

bool Mesh::isLoopOrientedUp(vector<int>& loop)
{
    float sum = 0;
    for(size_t i = 0; i < loop.size(); i++)
    {
        auto& v1 = vertices[loop[i]].pos;
        auto& v2 = vertices[loop[(i + 1) % loop.size()]].pos;
        sum += (v1.x - v2.x) * (v1.z + v2.z);
    }
    //for a closed loop, the sum shouldn't ever be 0 (only for a symmetrical figure 8)
    return sum <= 0;
}

bool Mesh::verticesCollinear(int v1, int v2, int v3)
{
    float eps = 1e-6;
    auto& p1 = vertices[v1].pos;
    auto& p2 = vertices[v2].pos;
    auto& p3 = vertices[v3].pos;
    auto vec = p1 - p2;
    if(fabs(vec.x) < eps)
        return fabs(p3.x - p2.x) < eps;
    if(fabs(vec.z) < eps)
        return fabs(p3.z - p2.z) < eps;
    float mult1 = (p1.x - p3.x) / vec.x;
    float mult2 = (p1.z - p3.z) / vec.z;
    return fabs(mult1 - mult2) < eps;
}

bool Mesh::verticesCollinear(vector<int>& v)
{
    float eps = 1e-6;
    if(v.size() < 3)
        return true;
    auto vec = vertices[v[0]].pos - vertices[v[1]].pos;
    if(fabs(vec.x) < eps)
    {
        for(size_t i = 2; i < v.size(); i++)
        {
            if(fabs(vertices[v[i]].pos.x - vertices[v[0]].pos.x) > eps)
                return false;
        }
        return true;
    }
    if(fabs(vec.z) < eps)
    {
        for(size_t i = 2; i < v.size(); i++)
        {
            if(fabs(vertices[v[i]].pos.z - vertices[v[0]].pos.z) > eps)
                return false;
        }
        return true;
    }
    for(size_t i = 2; i < v.size(); i++)
    {
        float mult1 = (vertices[v[0]].pos.x - vertices[v[i]].pos.x) / vec.x;
        float mult2 = (vertices[v[0]].pos.z - vertices[v[i]].pos.z) / vec.z;
        if(fabs(mult1 - mult2) > eps)
            return false;
    }
    return true;
}

bool Mesh::facesWrongOrientation(int e)
{
    Edge& edge = edges[e];
    if(edge.f[0] < 0 || edge.f[1] < 0)
        return false;
    Face& f1 = faces[edge.f[0]];
    Face& f2 = faces[edge.f[1]];
    return f1.isClockwise(edge.v[0], edge.v[1]) == f2.isClockwise(edge.v[0], edge.v[1]);
}

bool Mesh::retriEdgeOrientation(int v1, int v2, vector<int>& vertLoop)
{
    int i1 = -1;
    int i2 = -1;
    //first find the indices of v1, v2 in vertLoop (i1, i2)
    for(size_t i = 0; i < vertLoop.size(); i++)
    {
        if(vertLoop[i] == v1)
        {
            i1 = i;
            if(i2 >= 0)
                break;
        }
        if(vertLoop[i] == v2)
        {
            i2 = i;
            if(i1 >= 0)
                break;
        }
    }
    DBASSERT(i1 >= 0 && i2 >= 0);
    for(size_t i3 = 0; i3 < vertLoop.size(); i3++)
    {
        if((int) i3 == i1 || (int) i3 == i2)
            continue;
        //reorder the indices so they are clockwise (in order wrt vertLoop)
        int j1, j2, j3;
        sort3<int>(i1, i2, i3, j1, j2, j3);
        if(cross(vertices[vertLoop[j3]].pos - vertices[vertLoop[j2]].pos,
                 vertices[vertLoop[j1]].pos - vertices[vertLoop[j2]].pos).y < 1e-7)
            return false;
    }
    return true;
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
