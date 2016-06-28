#ifndef TRIMESH_H
#define TRIMESH_H

#include "Heightmap.h"
#include "Pool.h"
#include "GlmHeaders.h"
#include <vector>
#include <utility>

/* Unstructured triangular mesh for
 * efficiently storing terrain */

namespace MeshTypes
{
    typedef glm::vec3 Vec3;
    struct Vertex
    {
        Vec3 pos;
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
        Vec3 getNorm();
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

class Mesh 
{
public:
    //for terrain, pass heights/biomes; world seed important for consistent output!
    //faceMatchCutoff = minimum value of dot(n1hat, n2hat) for merging faces by edge collapse
    Mesh();
    void initWorldMesh(Heightmap& heights, Heightmap& faceValues, float faceMatchCutoff = 0.96);
    void simpleLoadHeightmap(Heightmap& heights, Heightmap& faceValues);
    void simplify(float faceMatchCutoff);
    bool edgeCollapse(int edge);    //returns false if the operation was aborted because of geometry
    void retriangulate(int vertexToRemove);    //delete a vertex and then fill in the hole
    bool isTriClique(int vertex);
    void removeTriClique(int vertex);
    void fixTriFlips(int e1, int e2);
    int getMaxVertices();
    int getMaxEdges();
    int getMaxFaces();
    Pool<MeshTypes::Vertex> vertices;
    Pool<MeshTypes::Vec3> normals;
    Pool<MeshTypes::Edge> edges;
    Pool<MeshTypes::Face> faces;
    //Utility functions
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
    void getBoundaryNeighbors(int e, int& f1, int& f2, int& f11, int& f12, int& f21, int& f22);
    //get the two faces neighboring the face, but excluding "exclude"
    void getFaceNeighbors(int f, int exclude, int& f1, int& f2);
    int getOtherFace(int f, int e);    //get the face neighboring f across the eth edge of f
    int getSharedEdge(int f1, int f2);
    //when a pair of verts only shares 1 valid face, return the face
    int getFaceFrom2Vertices(int v1, int v2);
    int getFaceFrom3Vertices(int v1, int v2, int v3);
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

#endif
