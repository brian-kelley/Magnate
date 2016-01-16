#ifndef __COORD_H__
#define __COORD_H__

#include <iostream>
#include <algorithm>
#include "GlmHeaders.h"
#include "LinAlg.h"
#include "GenTypes.h"

bool operator==(const Pos2& p1, const Pos2& p2);
bool operator!=(const Pos2& p1, const Pos2& p2);
std::ostream& operator<<(std::ostream& os, const Pos2& loc);

typedef glm::vec3 Pos3;

struct Plane
{
    Plane() : a(1), b(0), c(0), d(0) {}
    Plane(float a, float b, float c, float d) : a(a), b(b), c(c), d(d) {}
    float a;
    float b;
    float c;
    float d;
};

struct FrustumCorners
{
    glm::vec4 upperLeft;
    glm::vec4 upperRight;
    glm::vec4 lowerRight;
    glm::vec4 lowerLeft;
};

enum Direction
{
    UP,    //-y
    DOWN,  //+y
    LEFT,  //-x
    RIGHT, //+x
    NO_DIRECTION
};

namespace Coord
{
    void initCoord();
    bool rectInside(Rectangle& small, Rectangle& big);  //returns true if small is completely inside big
    glm::vec4 tileToWorld(int x, unsigned short height, int z);
    Pos2 worldToTile(glm::vec4 worldPos);
    extern const double TERRAIN_TILE_SIZE;
    extern const double TERRAIN_Y_SCALE;
    glm::vec4 getViewCenter(const glm::mat4& view); //get the point on XZ plane at center of view
    FrustumCorners getFrustumCorners(const glm::mat4& view, const glm::mat4& proj);
    //Component vectors for constructing worldspace coords from tile indices and heightmap heights
    extern const glm::vec4 tileI;
    extern const glm::vec4 tileJ;
    extern const glm::vec4 tileK;
    Pos2 getTileInDir(Pos2 loc, int dir);
    extern glm::mat4 tileToWorldMat;
    extern glm::mat4 worldToTileMat;
};
#endif