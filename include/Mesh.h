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
    extern const int NO_FACE;
    typedef glm::vec3 Vertex;
    typedef glm::vec3 Vec3;
    struct Edge
    {
        Edge(int v1, int v2, int f1, int f2);
        int v[2]; //vertex endpoints
        int f[2]; //triangles containing the edge
    };
    struct Face     //Vertices 0,1,2 clockwise: (2-1)x(1-0) points outward (up)
    {
        Face(int v1, int v2, int v3, int e1, int e2, int e3, int value);
        int v[3];
        int e[3];   //links to neighboring edges, which have links to neighboring faces
        int value;
        static int NONE_VALUE;
        Vec3 getNorm();
        static Pool<Vertex>* vertArray;
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
    bool collapseConnectivity(int edge);
};

#endif
