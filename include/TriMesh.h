#ifndef TRIMESH_H
#define TRIMESH_H

#include "DebugTools.h"
#include "Heightmap.h"
#include <vector>

/* Unstructured triangular mesh for
 * efficiently storing terrain */

struct Triangle
{
    int v1;
    int v2;
    int v3;
    int terrain;
};

struct MeshVertex
{
    float x;
    float y;
    float val;
};

struct TriMesh
{
    TriMesh(Heightmap& mesh, Heightmap& faceValues);  //for terrain, pass heights/biomes
    std::vector<MeshVertex> vertices;
    std::vector<Triangle> triangles;
};

#endif
