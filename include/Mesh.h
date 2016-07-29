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
        bool isClockwise(int v1, int v2);   //is v2 clockwise of v1
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
    void simplify(float faceMatchCutoff);
    vector<int> removeVertex(int vertex, int& terrainVal);
    void retriangulate(vector<int>& vertLoop, int terrainVal);
    int getMaxVertices();
    int getMaxEdges();
    int getMaxFaces();
    bool isTriClique(int vertex);
    void removeTriClique(int vertex);
    void fixTriFlips(int e1, int e2);
    void interiorEdgeCollapse(int e);
    void boundaryEdgeCollapse(int e);
    int hmVertIndex(int x, int y);
    int hmEdgeIndex(int x, int y, EdgeDir which);
    int hmFaceIndex(int x, int y, FaceDir which);
    bool faceValuesCheck(int edge);
    //NOTE: connectivity, boundary bridge, tri sides and flip checks return true if ok
    //check connectivity ok for edge collapse (no holes/overlaps will happen)
    bool collapseConnectivity(int edge); 
    //check that the two vertices, if both on map boundary and neither on corner,
    //make sure that they are on the same boundary (i.e. both along the north boundary)
    bool checkBoundaryBridge(int edge);
    //check the neighboring triangle side lengths to make sure collapse is an improvement
    bool collapseTriangleSides(int edge);
    //check for triangle flips, don't modify mesh
    bool checkFlips(int e1, int e2);
    //get 4 neighboring faces for collapse
    void getNeighbors(int e, int& f1, int& f2, int& f11, int& f12, int& f21, int& f22);
    //check if edge (v1, v2) is valid to be created
    bool retriEdgeOrientation(int v1, int v2, vector<int>& vertLoop);
    //check if faces adjacent to edge e are overlapping
    bool facesWrongOrientation(int e);
    void getBoundaryNeighbors(int e, int& f1, int& f2, int& f11, int& f12, int& f21, int& f22);
    //get the two faces neighboring the face, but excluding "exclude"
    void getFaceNeighbors(int f, int exclude, int& f1, int& f2);
    int getOtherFace(int f, int e);    //get the face neighboring f across the eth edge of f
    int getSharedEdge(int f1, int f2);
    //when a pair of verts only shares 1 valid face, return the face
    int getFaceFrom2Vertices(int v1, int v2);
    int getFaceFrom3Vertices(int v1, int v2, int v3);
    //c1, c2 set to vertices connected to both v1, v2 (or -1)
    void getMutualConnections(int v1, int v2, int& c1, int& c2);    
    bool faceExists(int v1, int v2, int v3);
    bool isLoopOrientedUp(vector<int>& loop);
    //are the vertices collinear in x/z?
    bool verticesCollinear(int v1, int v2, int v3);
    bool verticesCollinear(vector<int>& v);
    //replace all links to toReplace with newLink (changes links in faces and edges)
    void replaceVertexLinks(int toReplace, int newLink);
    void fullyDeleteEdge(int e); //completely deletes edge. Faces must not link to it!
    void mergeEdges(int e1, int e2, int f); //pass the edges to be merged, and shared face
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
