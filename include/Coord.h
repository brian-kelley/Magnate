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

struct Plane
{
    Plane() : a(1), b(0), c(0), d(0) {}
    Plane(float a, float b, float c, float d) : a(a), b(b), c(c), d(d) {}
    float a;
    float b;
    float c;
    float d;
};

enum Direction
{
    UP,    //-y
    DOWN,  //+y
    LEFT,  //-x
    RIGHT, //+x
    NO_DIRECTION
};

namespace Geom
{
    glm::vec2 leftNormal(glm::vec2 dir);    //rotate dir left 90 degrees
    glm::vec2 rightNormal(glm::vec2 dir);   //rotate right 90
    bool intersect(glm::vec2& result, glm::vec2 p, glm::vec2 dir, glm::vec2 testP, glm::vec2 testDir);
    Direction sideOfRay(glm::vec2 p, glm::vec2 dir, glm::vec2 test); //LEFT, RIGHT, NO_DIR
    void findTriVal(float target, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float v1, float v2, float v3);
    float triInterp(glm::vec3 target, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float v1, float v2, float v3);
}

namespace Coord
{
    void initCoord();
    bool rectInside(Rectangle& small, Rectangle& big);  //returns true if small is completely inside big
    glm::vec4 tileToWorld(int x, unsigned short height, int z);
    Pos2 worldToTile(glm::vec4 worldPos);
    extern const double TERRAIN_TILE_SIZE;
    extern const double TERRAIN_Y_SCALE;
    glm::vec4 getViewCenter(const glm::mat4& view); //get the point on XZ plane at center of view
    extern const glm::vec4 tileI;
    extern const glm::vec4 tileJ;
    extern const glm::vec4 tileK;
    Pos2 getTileInDir(Pos2 loc, int dir);
    extern glm::mat4 tileToWorldMat;
    extern glm::mat4 worldToTileMat;
};
#endif
