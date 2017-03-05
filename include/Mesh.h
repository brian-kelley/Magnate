#ifndef TRIMESH_H
#define TRIMESH_H

#include "Heightmap.h"
#include "Pool.h"
#include "GlmHeaders.h"
#include "Geom2D.h"
#include <vector>
#include <utility>

namespace MeshTypes
{
  struct Vertex
  {
    vec3 pos;
    vector<int> edges;
    bool boundary;
    bool corner;
    void addEdge(int e);
    void removeEdge(int e);
    bool hasEdge(int e);
    int connectsTo(int vert);   //return edge index or -1 if no connection
  };
  struct Edge
  {
    Edge();
    Edge(int v1, int v2, int f1, int f2);
    int v[2]; //vertex endpoints
    int f[2]; //triangles containing the edge
    bool hasVert(int query);
    bool hasFace(int query);
    float getLength();
    void replaceVertexLink(int toReplace, int newLink);
    void replaceFaceLink(int toReplace, int newLink);
    static Pool<Vertex>* vertArray;
    static Pool<Edge>* edgeArray;
  };
  struct Face     //Vertices 0,1,2 clockwise: (2-1)x(1-0) points outward (up)
  {
    Face();
    Face(int v1, int v2, int v3, int e1, int e2, int e3, int value);
    int v[3];
    int e[3];   //links to neighboring edges, which have links to neighboring faces
    int value;
    static int NONE_VALUE;
    vec3 getNorm();
    static Pool<Vertex>* vertArray;
    bool hasVert(int query);
    bool hasEdge(int query);
    void checkNormal();
    bool isCCW(int v1, int v2);   //is v2 clockwise of v1
    void replaceEdgeLink(int toReplace, int newLink);
    void replaceVertexLink(int toReplace, int newLink);
  };
}

enum struct EdgeDir
{
  LEFT,
  RIGHT,
  TOP,
  BOTTOM,
  DIAGONAL
};

enum struct FaceDir
{
  UPPER_LEFT,
  LOWER_RIGHT
};

struct Mesh 
{
  //for terrain, pass heights/biomes; world seed important for consistent output!
  //faceMatchCutoff = minimum value of dot(n1hat, n2hat) for merging faces by edge collapse
  Mesh();
  void initWorldMesh(Heightmap& heights, Heightmap& faceValues, float faceMatchCutoff = 0.96);
  bool edgeCollapse(int edge);    //returns false if the operation was aborted because of geometry
  void removeAndRetriangulate(int vertex);
  Pool<MeshTypes::Vertex> vertices;
  Pool<vec3> normals;
  Pool<MeshTypes::Edge> edges;
  Pool<MeshTypes::Face> faces;
  void simpleLoadHeightmap(Heightmap& heights, Heightmap& faceValues);
  void simplify();
  vector<int> removeVertex(int vertex, int& terrainVal);
  vector<int> removeVertexBoundary(int vertex, int& terrainVal);
  void retriangulate(vector<int>& vertLoop, int terrainVal);
  int getMaxVertices();
  int getMaxEdges();
  int getMaxFaces();
  bool isTriClique(int vertex);
  void removeTriClique(int vertex);
  int hmVertIndex(int x, int y);
  int hmEdgeIndex(int x, int y, EdgeDir which);
  int hmFaceIndex(int x, int y, FaceDir which);
  //check if one face adjacent to edge e is oriented wrong
  bool facesWrongOrientation(int e);
  void getBoundaryNeighbors(int e, int& f1, int& f2, int& f11, int& f12, int& f21, int& f22);
  //get the two faces neighboring the face, but excluding "exclude"
  void getFaceNeighbors(int f, int exclude, int& f1, int& f2);
  int getOtherFace(int f, int e);    //get the face neighboring f across the eth edge of f
  int getOtherVertex(int e, int v);    //get the vertex from e that is not v
  int getSharedEdge(int f1, int f2);
  //when a pair of verts only shares 1 valid face, return the face
  int getFaceFrom2Vertices(int v1, int v2);
  int getFaceFrom3Vertices(int v1, int v2, int v3);
  int getCCWFace(int edge, int v1, int v2);
  //c1, c2 set to vertices connected to both v1, v2 (or -1)
  void getMutualConnections(int v1, int v2, int& c1, int& c2);    
  bool isLoopOrientedUp(vector<int>& loop);
  //are the vertices collinear in x/z?
  bool verticesCollinear(int v1, int v2, int v3);
  bool verticesCollinear(vector<int>& v);
  //replace all links to toReplace with newLink (changes links in faces and edges)
  void replaceVertexLinks(int toReplace, int newLink);
  void fullyDeleteEdge(int e); //completely deletes edge. Faces must not link to it!
  //order v1, v2 for EC so that v1 could be deleted and v2 could be kept
  void orderCollapseVerts(int& v1, int& v2);
  //return INVALID, or the index of a vert in f that needs removing
  int faceNeedsVertRemoval(int f);
  //get the position of the vertex merged from v1, v2 in EC
  vec3 getCollapseVertPos(int v1, int v2);  
  //scan whole mesh again for bad orientation/thin triangles and repair
  void badTriangleRepair();
  void fullCorrectnessCheck();
  void deepTest(Heightmap& heights, Heightmap& faceValues);
  bool validFace(int f);
  bool validEdge(int e);
  bool validVertex(int v);
};

std::ostream& operator<<(std::ostream& os, const MeshTypes::Face& face);
std::ostream& operator<<(std::ostream& os, const MeshTypes::Edge& edge);
std::ostream& operator<<(std::ostream& os, const MeshTypes::Vertex& vert);

#endif
