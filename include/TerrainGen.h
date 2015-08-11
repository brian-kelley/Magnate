#ifndef __TERRAIN_GEN_INCLUDED__
#define __TERRAIN_GEN_INCLUDED__

#include <iostream>
#define ROUGHNESS 10
#include "Constants.h"
#include "Chunk.h"
#include "World.h"

class Chunk;    //have to forward-declare World and Chunk because of mutual includes

/*
 //Bird's eye view of world coordinates & QTnode format
 
 O---> +x
 | 1 2
 | 4 3
 V
 
 +z
 
 */
//Quadtree node. Has pointers to the four squares inside of it.
class QTNode
{
public:
    QTNode(float h, float x, float z, float size, QTNode* master);
    static QTNode* treeRoot; //largest square, has no master
    bool isRoot();
    QTNode* find(float xo, float zo); //Smallest node in tree containing the point (xo, zo)
    QTNode* findWithSize(float xo, float zo, float size); //like find but stops when the node is a certain size
    QTNode* up();    //move in a direction on this node's level
    QTNode* left();
    QTNode* right();
    QTNode* down();
    int getDepth();
    //altitudes at the 4 corners
    float h; //height of upper-left (min x/max y in world space)
    float x;  //coordinates of the upper-left corner
    float z;
    float size;
    QTNode* master;
    QTNode* n1;
    QTNode* n2;
    QTNode* n3;
    QTNode* n4;
};

namespace TerrainGen
{
    void generate();
    void diamondSquare();
    void fillSquare(int x, int y, int size);
    void fillDiamond(int x, int y, int size);
    //Tests whether the mesh
    bool inMesh(int x, int y);
    Height getHeight(int avg, int size);
    extern const int DETAIL_ITERS;
}

#endif