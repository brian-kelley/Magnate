 #include "Coord.h"

using namespace glm;
using namespace std;

mat4 Coord::tileToWorldMat;
mat4 Coord::worldToTileMat;
const double Coord::TERRAIN_TILE_SIZE = 0.5;
const double Coord::TERRAIN_Y_SCALE = 0.05;
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

FrustumCorners Coord::getFrustumCorners(const glm::mat4 &view, const glm::mat4 &proj)
{
    FrustumCorners corners;
    //Calculate camera at position from view matrix (is origin in camera space)
    vec4 camPos = view * vec4(0, 0, 0, 1);
    mat4 projInv = glm::inverse(proj);
    mat4 viewInv = glm::inverse(view);
    float back = 1;
    float front = -1;
    float left = -1;
    float right = 1;
    float up = 1;
    float down = -1;
    vec4 lub = {left, up, back, 1};
    vec4 rub = {right, up, back, 1};
    vec4 rdb = {right, down, back, 1};
    vec4 ldb = {left, down, back, 1};
    vec4 luf = {left, up, front, 1};
    vec4 ruf = {right, up, front, 1};
    vec4 rdf = {right, down, front, 1};
    vec4 ldf = {left, down, front, 1};
    lub = projInv * lub;
    rub = projInv * rub;
    rdb = projInv * rdb;
    ldb = projInv * ldb;
    luf = projInv * luf;
    ruf = projInv * ruf;
    rdf = projInv * rdf;
    ldf = projInv * ldf;
    lub /= lub.w;
    rub /= rub.w;
    rdb /= rdb.w;
    ldb /= ldb.w;
    luf /= luf.w;
    ruf /= ruf.w;
    rdf /= rdf.w;
    ldf /= ldf.w;
    lub = viewInv * lub;
    rub = viewInv * rub;
    rdb = viewInv * rdb;
    ldb = viewInv * ldb;
    luf = viewInv * luf;
    ruf = viewInv * ruf;
    rdf = viewInv * rdf;
    ldf = viewInv * ldf;
    //frustum corner points are now world space
    //draw line between front and back point pairs and find where y = 0
    vec4 luVec = normalize(luf - lub);
    vec4 ruVec = normalize(ruf - rub);
    vec4 rdVec = normalize(rdf - rdb);
    vec4 ldVec = normalize(ldf - ldb);
    float luMult = -lub.y / luVec.y;
    float ruMult = -rub.y / ruVec.y;
    float rdMult = -rdb.y / rdVec.y;
    float ldMult = -ldb.y / ldVec.y;
    corners.upperLeft = {camPos.x + luMult * luVec.x, 0, camPos.z + luMult * luVec.z, 1};
    corners.upperRight = {camPos.x + ruMult * ruVec.x, 0, camPos.z + ruMult * ruVec.z, 1};
    corners.lowerRight = {camPos.x + rdMult * rdVec.x, 0, camPos.z + rdMult * rdVec.z, 1};
    corners.lowerLeft = {camPos.x + ldMult * ldVec.x, 0, camPos.z + ldMult * ldVec.z, 1};
    return corners;
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