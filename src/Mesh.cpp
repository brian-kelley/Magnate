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
  else
  {
    PRINT("Expected edge to have face link " << toReplace << " but it didn't!");
    DBASSERT(false);
  }
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

bool Face::isCCW(int v1, int v2)
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
  removeAndRetriangulate(hmVertIndex(2, 2));
  //TIMEIT(simplify());
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

void Mesh::simplify()
{
  int vertsBefore = vertices.size;
  int edgesBefore = edges.size;
  int facesBefore = faces.size;
  const float maxRemoveSpan = Coord::TERRAIN_TILE_SIZE * 8;
  //minimum value of dot(norm1, norm2) for the faces to be coplanar enough to combine
  const float minNormDot = 0.99;
  int vertsRemoved = 0;
  //do a sweep over vertices
  {
    for(int i = 0; i < WORLD_SIZE + 1; i++)
    {
      for(int j = 0; j < WORLD_SIZE + 1; j++)
      {
        int index = hmVertIndex(i, j);
        bool canRemove = true;
        if(vertices.isAllocated(index))
        {
          auto& v = vertices[index];
          if(v.corner)
          {
            canRemove = false;
          }
          //check that: face values around vert are all same, and
          //that face norms are very similar
          //get "representative" face to get value/norm to compare against
          Edge& firstEdge = edges[v.edges.front()];
          Face& reprFace = firstEdge.f[0] >= 0 ? faces[firstEdge.f[0]] : faces[firstEdge.f[1]];
          int faceval = reprFace.value;
          auto facenorm = reprFace.getNorm();
          for(auto e : v.edges)
          {
            Edge& edge = edges[e];
            if(edge.getLength() >= maxRemoveSpan)
            {
              canRemove = false;
              break;
            }
            int val1 = edge.f[0] >= 0 ? faces[edge.f[0]].value : faceval;
            int val2 = edge.f[1] >= 0 ? faces[edge.f[1]].value : faceval;
            auto norm1 = edge.f[0] >= 0 ? faces[edge.f[0]].getNorm() : facenorm;
            auto norm2 = edge.f[1] >= 0 ? faces[edge.f[1]].getNorm() : facenorm;
            if(val1 != faceval || val2 != faceval)
            {
              canRemove = false;
              break;
            }
            else if(dot(facenorm, norm1) < minNormDot || dot(facenorm, norm2) < minNormDot)
            {
              canRemove = false;
              break;
            }
          }
        }
        if(canRemove)
        {
          /*
          int failpoint = 767;
          if(vertsRemoved == failpoint)
          {
            PRINT("Failure vertex index: " << index);
            vertices[index].pos.y += 1;
            //int asdf;
            //auto edgeList = removeVertex(index, asdf);
            try
            {
              removeAndRetriangulate(index);
            } catch(...) {}
            return;
          }
          */
          removeAndRetriangulate(index);
          vertsRemoved++;
        }
      }
    }
  }
  double vertsReduce = (double) (vertsBefore - vertices.size) / vertsBefore;
  double edgesReduce = (double) (edgesBefore - edges.size) / edgesBefore;
  double facesReduce = (double) (facesBefore - faces.size) / facesBefore;
  PRINT("Removed " << vertsBefore - vertices.size << " verts, or " << vertsReduce * 100  << "%");
  PRINT("Removed " << edgesBefore - edges.size << " edges, or " << edgesReduce * 100  << "%");
  PRINT("Removed " << facesBefore - faces.size << " faces, or " << facesReduce * 100  << "%");
}

void Mesh::removeAndRetriangulate(int vertexToRemove)
{
  if(isTriClique(vertexToRemove))
  {
    removeTriClique(vertexToRemove);
    return;
  }
  int terrainVal;
  auto vertLoop = removeVertex(vertexToRemove, terrainVal);
  retriangulate(vertLoop, terrainVal);
}

vector<int> Mesh::removeVertex(int v, int& terrainVal)
{
  PRINT("Removing vertex " << v);
  vector<int> loop;
  //get a vertex connected to vertex (doesn't matter which)
  Vertex& vert = vertices[v];
  if(vert.boundary)
    PRINT(">!>!>!> vert is boundary");
  int loopSize = vert.edges.size();
  //delete all the faces containing vert
  for(auto e : vert.edges)
  {
    for(int i = 0; i < 2; i++)
    {
      int f = edges[e].f[i];
      if(f >= 0)
      {
        Face& face = faces[f];
        terrainVal = face.value;
        edges[face.e[0]].replaceFaceLink(f, INVALID);
        edges[face.e[1]].replaceFaceLink(f, INVALID);
        edges[face.e[2]].replaceFaceLink(f, INVALID);
        faces.dealloc(f);
      }
    }
  }
  //if vert is on boundary, merge the two edges along the boundary into one
  if(vert.boundary)
  {
    int be1 = INVALID;
    int be2 = INVALID;
    for(auto e : vert.edges)
    {
      if(vertices[getOtherVertex(e, v)].boundary)
      {
        if(be1 == INVALID)
        {
          be1 = e;
        }
        else
        {
          be2 = e;
          break;
        }
      }
    }
    //delete be2, then extend be1
    int newVert = getOtherVertex(be2, v);
    edges[be1].replaceVertexLink(v, newVert);
    vert.removeEdge(be1);
    edges.dealloc(be2);
    //know the first 2 vertices in loop to begin CCW traversal
    loop.push_back(getOtherVertex(be1, newVert));
    loop.push_back(newVert);
  }
  else
  {
    //get the first vertex to use in loop (pick arbitrarily)
    loop.push_back(getOtherVertex(vert.edges[0], v));
  }
  //walk around the loop (direction doesn't really matter), adding vertices
  while(loop.size() < loopSize)
  {
    int p = loop.back();
    Vertex& prev = vertices[p];
    for(auto e : prev.edges)
    {
      int otherV = getOtherVertex(e, p);
      if(loop.size() == 1 || otherV != loop[loop.size() - 2])
      {
        Vertex& otherVertex = vertices[otherV];
        if(otherVertex.connectsTo(v))
        {
          loop.push_back(otherV);
          break;
        }
      }
    }
  }
  //now delete all edges connecting to v
  for(auto e : vert.edges)
  {
    int otherVert = getOtherVertex(e, v);
    vertices[otherVert].removeEdge(e);
    edges.dealloc(e);
  }
  //finally, delete vert
  vertices.dealloc(v);
  if(!isLoopOrientedUp(loop))
  {
    //reverse loop
    for(int i = 0; i < loop.size() / 2; i++)
    {
      SWAP(loop[i], loop[loop.size() - 1 - i]);
    }
  }
  return loop;
}

void Mesh::retriangulate(vector<int>& vertLoop, int terrainVal)
{
  auto edgesCross = [&] (pair<int, int> lhs, pair<int, int> rhs) -> bool
  {
    return Geom2D::lineSegmentsCross(vertices[lhs.first].pos.xz(), vertices[lhs.second].pos.xz(),
        vertices[rhs.first].pos.xz(), vertices[rhs.second].pos.xz());
  };
  if(!isLoopOrientedUp(vertLoop))
  {
    //reverse vertLoop
    for(int i = 0; i < vertLoop.size() / 2; i++)
    {
      SWAP(vertLoop[i], vertLoop[vertLoop.size() - 1 - i]);
    }
  }
  //while there are more than 3 vertices in vertLoop, choose the best pair (separated by 1) to form edge
  //form the edge and create a new triangle
  //then remove the vertex in between the connected verts from vertloop
  //when there are only 3 verts remaining, make the last triangle with them
  int count = 0;
  int numToCreate = vertLoop.size() - 3 + 1;
  while(vertLoop.size() > 3)
  {
    //PRINT("Making triangle " << ++count << " of " << numToCreate);
    //PRINTVAR(vertLoop);
    //find the best (potential) new edge
    //constraints:
    //  -indices in vertLoop (mod size) must differ by 2
    //  -can't already be connected
    //  -must not be collinear with the vertex in between
    //  -minimize distance
    pair<int, int> newEdge;
    bool foundEdge = false;
    double bestDist = 1e12;
    //the index to be removed from vertLoop
    int vertToDeleteIndex;
    for(int i = 0; i < vertLoop.size(); i++)
    {
      int v1 = vertLoop[i];
      int vmid = vertLoop[(i + 1) % vertLoop.size()];
      int v2 = vertLoop[(i + 2) % vertLoop.size()];
      pair<int, int> edgeCand(v1, v2);
      //PRINT("Checking edge candidate from " << v1 << " to " << v2);
      Vertex& vert1 = vertices[v1];
      Vertex& vert2 = vertices[v2];
      if(vert1.connectsTo(v2) >= 0)
      {
        //PRINT(">>Can't use because v1, v2 already connected");
        continue;
      }
      if(verticesCollinear(v1, vmid, v2))
      {
        //PRINT(">>Can't use because v1, vmid, v2 are collinear");
        continue;
      }
      double dist = glm::length(vert1.pos - vert2.pos);
      if(foundEdge && dist > bestDist)
      {
        //PRINT(">>Won't use because points are further apart than known best");
        continue;
      }
      //check that no other vertices in the loop are on the line segment from v1 to v2 
      //for each other vertex, test collinearity and then test if between
      //note: common and totally fine if collinear but not in between
      {
        bool hit = false;
        for(int other : vertLoop)
        {
          if(other == v1 || other == v2 || other == vmid)
          {
            continue;
          }
          if(verticesCollinear(v1, other, v2))
          {
            Vertex& otherVert = vertices[other];
            auto diff1 = otherVert.pos - vert1.pos;
            auto diff2 = vert2.pos - otherVert.pos;
            if(glm::dot(diff1, diff2) >= 0)
            {
              hit = true;
              break;
            }
          }
        }
        if(hit)
        {
          //PRINT(">>Can't use because other vert(s) in vertLoop are on segment between v1, v2");
          continue;
        }
      }
      //make sure edge candidate is inside the empty space, and doesn't cross any other edges
      //the edges it may cross are in the faces containing edges (v1, vmid) and (vmid, v2)
      {
        Vertex& midVertex = vertices[vmid];
        if(cross(midVertex.pos - vert1.pos, vert2.pos - midVertex.pos).y <= 0)
        {
          //PRINT(">>Can't use because would make non-manifold flipped face.");
          continue;
        }
      }
      //PRINT("---- Passed all criteria, setting best to this edge.");
      foundEdge = true;
      newEdge.first = v1;
      newEdge.second = v2;
      vertToDeleteIndex = (i + 1) % vertLoop.size();
      bestDist = dist;
    }
    if(!foundEdge)
    {
      //PRINT("Have no suitable candidate for an edge to create!!!!");
      return;
    }
    int v1 = newEdge.first;
    int vmid = vertLoop[vertToDeleteIndex];
    int v2 = newEdge.second;
    //form edge and face
    int e = edges.alloc();
    int f = faces.alloc();
    Edge& edge = edges[e];
    Face& face = faces[f];
    edge.v[0] = v1;
    edge.v[1] = v2;
    edge.f[0] = INVALID;
    edge.f[1] = f;
    //PRINT("Making edge with verts: " << edge.v[0] << ", " << edge.v[1]);
    vertices[v1].addEdge(e);
    vertices[v2].addEdge(e);
    face.v[0] = v1;
    face.v[1] = vmid;
    face.v[2] = v2;
    //PRINT("Making face with verts: " << face.v[0] << ", " << face.v[1] << ", " << face.v[2]);
    Vertex& vert1 = vertices[v1];
    Vertex& vert2 = vertices[v2];
    face.e[0] = vert1.connectsTo(vmid);
    DBASSERT(face.e[0] >= 0);
    edges[face.e[0]].replaceFaceLink(INVALID, f);
    face.e[1] = vert2.connectsTo(vmid);
    DBASSERT(face.e[1] >= 0);
    edges[face.e[1]].replaceFaceLink(INVALID, f);
    face.e[2] = e;
    face.checkNormal();
    face.value = terrainVal;
    //remove vertToDelete from vertLoop
    vertLoop.erase(vertLoop.begin() + vertToDeleteIndex);
  }
  //finally, create last face with 3 remaining triangles
  //all edges already exist
  DBASSERT(vertLoop.size() == 3);
  int f = faces.alloc();
  Face& face = faces[f];
  face.v[0] = vertLoop[0];
  face.v[1] = vertLoop[1];
  face.v[2] = vertLoop[2];
  //PRINT("Creating last face with verts: " << face.v[0] << ", " << face.v[1] << ", " << face.v[2]);
  face.checkNormal();
  face.e[0] = vertices[face.v[0]].connectsTo(face.v[1]);
  DBASSERT(face.e[0] > 0);
  edges[face.e[0]].replaceFaceLink(INVALID, f);
  face.e[1] = vertices[face.v[1]].connectsTo(face.v[2]);
  DBASSERT(face.e[1] > 0);
  edges[face.e[1]].replaceFaceLink(INVALID, f);
  face.e[2] = vertices[face.v[2]].connectsTo(face.v[0]);
  DBASSERT(face.e[2] > 0);
  edges[face.e[2]].replaceFaceLink(INVALID, f);
  face.value = terrainVal;
}

bool Mesh::isTriClique(int vertex)
{
  return vertex >= 0 && vertices[vertex].edges.size() == 3;
}

void Mesh::removeTriClique(int vertex)
{
  if(vertices[vertex].boundary)
    return;
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
  DBASSERT(vert1 != vert2);
  Vertex& v1 = vertices[vert1];
  Vertex& v2 = vertices[vert2];
  c1 = INVALID;
  c2 = INVALID;
  float lineDist1 = 1e30;
  float lineDist2 = 1e30;
  //pick UP TO 1 mutual connections on one side
  vector<int> allMutual;
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
      if(shared != INVALID)
      {
        allMutual.push_back(shared);
        break;
      }
    }
  }
  //sort allMutual verts into left and right
  //let c1 = closest left vert, c2 = closest right vert
  auto getVec2 = [&] (int v) -> vec2
  {
    return vertices[v].pos.xz();
  };
  vec2 base = getVec2(vert1);
  vec2 head = getVec2(vert2);
  for(auto m : allMutual)
  {
    vec2 testHead = getVec2(m);
    int side = Geom2D::pointLineSide(testHead, base, head);
    if(side == 0)   //ignore collinear points, needs to be handled by caller
      continue;
    float mLineDist = Geom2D::pointLineDist(testHead, base, head);
    if(side == 1)
    {
      //replace c1 with m if m is closer to line
      if(mLineDist < lineDist1)
      {
        c1 = m;
        lineDist1 = mLineDist;
      }
    }
    else
    {
      //replace c2 with m if m is closer to line
      if(mLineDist < lineDist2)
      {
        c2 = m;
        lineDist2 = mLineDist;
      }
    }
  }
  if(c1 == INVALID && c2 != INVALID)
    SWAP(c1, c2);
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
  double eps = 1e-8;
  auto& p1 = vertices[v1].pos;
  auto& p2 = vertices[v2].pos;
  auto& p3 = vertices[v3].pos;
  //        [1 p1.x p1.z]
  //if det( |1 p2.x p2.z] ) == 0, then the verts are collinear
  //        [1 p3.x p3.z]
  double det = 0;
  det += (p2.x * p3.z - p2.z * p3.x);
  det -= (p1.x * p3.z - p1.z * p3.x);
  det += (p1.x * p2.z - p1.z * p2.x);
  if(fabs(det) < eps)
    return true;
  else
    return false;
}

bool Mesh::verticesCollinear(vector<int>& v)
{
  if(v.size() < 3)
    return true;
  int v1 = v[0];
  int v2 = v[1];
  for(size_t i = 2; i < v.size(); i++)
  {
    int v3 = v[i];
    if(!verticesCollinear(v1, v2, v3))
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
  return f1.isCCW(edge.v[0], edge.v[1]) == f2.isCCW(edge.v[0], edge.v[1]);
}

void Mesh::orderCollapseVerts(int& v1, int& v2)
{
  Vertex& vert1 = vertices[v1];
  Vertex& vert2 = vertices[v2];
  if(vert1.boundary && !vert2.boundary)
  {
    SWAP(v1, v2);
    return;
  }
  else if(vert1.corner && !vert2.corner)
  {
    SWAP(v1, v2);
    return;
  }
}


vec3 Mesh::getCollapseVertPos(int v1, int v2)
{
  DBASSERT(validVertex(v1) && validVertex(v2));
  Vertex& vert1 = vertices[v1];
  Vertex& vert2 = vertices[v2];
  if(!(vert1.boundary && vert2.boundary))
  {
    //interior edge collapse
    if(vertices[v2].boundary)
      return vert2.pos;
    else
      return (vert1.pos + vert2.pos) / 2.0f;
  }
  else
  {
    //boundary edge collapse
    if(!vert1.corner && !vert2.corner)
      return (vert1.pos + vert2.pos) / 2.0f;
    else
      return vert2.pos;
  }
}
int Mesh::faceNeedsVertRemoval(int f)
{
  Face& face = faces[f];
  float cutoffFactor = 1.07;
  //first check for bad orientation of this face
  //since they should always be fixed immediately, this face is wrong
  //1st possibility: all 3 neighboring faces are correct and this one is wrong
  bool rv = false;
  if(facesWrongOrientation(face.e[0]) &&
      facesWrongOrientation(face.e[1]) &&
      facesWrongOrientation(face.e[2]))
  {
    rv = true;
  }
  int edgeIndices[3];
  float edgeLen[3];
  for(int i = 0; i < 3; i++)
  {
    edgeIndices[i] = face.e[i];
    edgeLen[i] = edges[edgeIndices[i]].getLength();
  }
  //sort edgeLen ascending
  if(edgeLen[0] > edgeLen[1])
  {
    SWAP(edgeLen[0], edgeLen[1]);
    SWAP(edgeIndices[0], edgeIndices[1]);
  }
  if(edgeLen[1] > edgeLen[2])
  {
    SWAP(edgeLen[1], edgeLen[2]);
    SWAP(edgeIndices[1], edgeIndices[2]);
  }
  if(edgeLen[0] > edgeLen[1])
  {
    SWAP(edgeLen[0], edgeLen[1]);
    SWAP(edgeIndices[0], edgeIndices[1]);
  }
  //2nd possibility: triangle is too thin
  DBASSERT(edgeLen[0] <= edgeLen[1] && edgeLen[1] <= edgeLen[2]);
  if(edgeLen[0] + edgeLen[1] < cutoffFactor * edgeLen[2])
  {
    rv = true;
  }
  if(rv)
  {
    //return index of vertex opposite (not in) longest face
    Edge& longEdge  = edges[edgeIndices[2]];
    for(int i = 0; i < 3; i++)
    {
      if(longEdge.v[0] != face.v[i] && longEdge.v[1] != face.v[i])
      {
        PRINT("Face " << f << " has vert " << face.v[i] << " which should be removed.");
        return face.v[i];
      }
    }
  }
  return INVALID;
}

void Mesh::badTriangleRepair()
{
  int toRemove;
  for(auto it = faces.begin(); it != faces.end(); it++)
  {
    if((toRemove = faceNeedsVertRemoval(it.loc)) != INVALID)
    {
      if(toRemove == 16745)
      {
        //vertices[toRemove].pos.y += 3;
        int dontcare;
        auto loop = removeVertex(16745, dontcare);
        PRINTVAR(loop);
        return;
      }
      else
        removeAndRetriangulate(toRemove);
    }
  }
}

int Mesh::getOtherVertex(int e, int v)
{
  return edges[e].v[0] == v ? edges[e].v[1] : edges[e].v[0];
}

int Mesh::getCCWFace(int edge, int v1, int v2)
{
  //get the face adjacent to edge where v1 => v2 is CCW order (in f.v order)
  Edge& e = edges[edge];
  if(e.f[0] >= 0 && faces[e.f[0]].isCCW(v1, v2))
    return e.f[0];
  else if(e.f[1] >= 0 && faces[e.f[1]].isCCW(v1, v2))
    return e.f[1];
  DBASSERT(false);
  return INVALID;
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

