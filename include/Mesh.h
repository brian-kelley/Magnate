#ifndef TRIMESH_H
#define TRIMESH_H

#include "Heightmap.h"
#include "Pool.h"
#include "GlmHeaders.h"
#include <vector>
#include <utility>

/* Unstructured triangular mesh for
 * efficiently storing terrain */

bool minWorldX(glm::vec3& loc);
bool maxWorldX(glm::vec3& loc);
bool minWorldY(glm::vec3& loc);
bool maxWorldY(glm::vec3& loc);

namespace MeshTypes
{
    typedef glm::vec3 Vec3;
    struct Vertex
    {
        Vec3 pos;
        vector<int> edges;
        void addEdge(int e);
        void removeEdge(int e);
        bool isOnBoundary();    //is the vertex on the world boundary?
    };
    struct Edge
    {
        Edge();
        Edge(int v1, int v2, int f1, int f2);
        int v[2]; //vertex endpoints
        int f[2]; //triangles containing the edge
        bool hasVert(int query);
        bool hasFace(int query);
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
    void edgeCollapse(int edge);
    int getMaxVertices();
    int getMaxEdges();
    int getMaxFaces();
    Pool<MeshTypes::Vertex> vertices;
    Pool<MeshTypes::Vec3> normals;
    Pool<MeshTypes::Edge> edges;
    Pool<MeshTypes::Face> faces;
    //Utility functions
    int hmVertIndex(int x, int y);
    int hmEdgeIndex(int x, int y, EdgeDir which);
    int hmFaceIndex(int x, int y, FaceDir which);
    //check connectivity ok for edge collapse (no holes/overlaps will happen)
    bool collapseConnectivity(int edge); 
    //check that no triangles flip in collapse (without modifying mesh)
    bool collapseFlip(int edge); 
    //get 4 neighboring faces for collapse
    void getNeighbors(int e, int& f1, int& f2, int& f11, int& f12, int& f21, int& f22);
    //get the two faces neighboring the face, but excluding "exclude"
    void getFaceNeighbors(int f, int exclude, int& f1, int& f2);
    int getOtherFace(int f, int e);    //get the face neighboring f across the eth edge of f
    int getSharedEdge(int f1, int f2);
    //replace all links to toReplace with newLink (changes links in faces and edges)
    void replaceVertexLinks(int toReplace, int newLink);
    //remove vertices' edge reference
    void removeEdgeRefs(int e);
};

std::ostream& operator<<(std::ostream& os, const MeshTypes::Face& face);

#endif
