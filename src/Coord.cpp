 #include "Coord.h"

using namespace glm;
using namespace std;

mat4 Coord::tileToWorldMat;
mat4 Coord::worldToTileMat;
const double Coord::TERRAIN_TILE_SIZE = 0.5;
const double Coord::TERRAIN_Y_SCALE = 0.02;
const vec4 Coord::tileI = {TERRAIN_TILE_SIZE, 0, 0, 0};
const vec4 Coord::tileJ = {0, TERRAIN_Y_SCALE, 0, 0};
const vec4 Coord::tileK = {0, 0, TERRAIN_TILE_SIZE, 0};

void Coord::initCoord()
{
    tileToWorldMat[0] = tileI;
    tileToWorldMat[1] = tileJ;
    tileToWorldMat[2] = tileK;
    worldToTileMat = inverse(tileToWorldMat);
}

bool Coord::rectInside(Rectangle &small, Rectangle &big)
{
    if(big.x <= small.x && big.x + big.w > small.x + small.w
       && big.y <= small.y && big.y + big.h > small.y + small.h)
        return true;
    return false;
}

vec4 Coord::tileToWorld(int x, unsigned short height, int z)
{
    vec4 tile = {x, height, z, 1};
    return tileToWorldMat * tile;
}

Pos2 Coord::worldToTile(vec4 worldPos)
{
    vec4 tileVec = worldToTileMat * worldPos;
    return {short(tileVec.x), short(tileVec.z)};
}

Pos2 Coord::getTileInDir(Pos2 loc, int dir)
{
    switch(dir)
    {
        case UP:
            loc.y--;
            return loc;
        case DOWN:
            loc.y++;
            return loc;
        case LEFT:
            loc.x--;
            return loc;
        case RIGHT:
            loc.x++;
            return loc;
        default:
            return {-1, -1};
    }
}

glm::vec4 Coord::getViewCenter(const glm::mat4 &view)
{
    //Get vectors just in front and just behind camera (in view space)
    glm::vec4 front = {0, 0, -1, 1};
    glm::vec4 behind = {0, 0, 1, 1};
    //Unproject both (back to world space)
    auto viewInv = inverse(view);
    front = viewInv * front;
    behind = viewInv * behind;
    //Trace the line formed by the two vectors to y=0 in world space
    vec4 diff = front - behind;
    return front + diff * (-front.y / diff.y);
}

bool operator==(const Pos2& p1, const Pos2& p2)
{
    if(p1.x == p2.x && p1.y == p2.y)
        return true;
    return false;
}

bool operator!=(const Pos2& p1, const Pos2& p2)
{
    return !(p1 == p2);
}

ostream& operator<<(ostream& os, const Pos2& loc)
{
    os << "(" << loc.x << ", " << loc.y << ")";
    return os;
}