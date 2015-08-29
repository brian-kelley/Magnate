//
//  Coord.cpp
//  Magnate
//
//  Created by Brian Kelley on 10/15/14288.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "Coord.h"

using namespace constants;
using namespace glm;
using namespace std;

mat4 Coord::view3;
mat4 Coord::proj3;
vec3 Coord::camPos;
vec3 Coord::camUp;
vec3 Coord::camDir;
float Coord::camAngle;
mat4 Coord::tileToWorldMat;
mat4 Coord::worldToTileMat;

void Coord::initCoord()
{
    tileToWorldMat[0] = tileI;
    tileToWorldMat[1] = tileJ;
    tileToWorldMat[2] = tileK;
    worldToTileMat = inverse(tileToWorldMat);
}

bool Coord::rectInside(intRect_t* small, intRect_t* big)
{
    if(big->x <= small->x && big->x + big->w > small->x + small->w
       && big->y <= small->y && big->y + big->h > small->y + small->h)
    {
        return true;
    }
    return false;
}

vec3 Coord::tileToWorld(int x, unsigned short height, int z)
{
    vec4 tileVec = {x, height, z, 1};
    return vec3(tileToWorldMat * tileVec);
}

Pos2 Coord::worldToTile(vec4 worldPos)
{
    vec4 tileVec = worldToTileMat * worldPos;
    return {short(tileVec.x), short(tileVec.z)};
}

//solve a 2x3 matrix (2 equations, 2 unknowns)
vec2 Coord::getY0Intersect(Plane& p1, Plane& p2)
{
    LinAlg::system2 probMat;
    probMat[0][0] = p1.a;
    probMat[1][0] = p1.c;
    probMat[2][0] = p1.d;
    probMat[0][1] = p2.a;
    probMat[1][1] = p2.c;
    probMat[2][1] = p2.d;
    if(!LinAlg::solveSystem2(probMat))
    {
        PRINT("Non-fatal error: lin alg problem matrix not onto!")
    }
    vec2 soln = {probMat[2][0], probMat[2][1]};
    //APPROX_EQ(p1.a * soln[0] + p1.c * soln[1] + p1.d, 0)
    //APPROX_EQ(p2.a * soln[0] + p2.c * soln[1] + p2.d, 0)
    return soln;
}

void Coord::transformPlane(Plane& p, mat4& mat)
{
    vec4 newNorm = {p.a, p.b, p.c, 0};
    newNorm = transpose(inverse(mat)) * newNorm;
    vec4 newPos = {p.a, p.b, p.c, 1};
    newPos *= p.d;
    newPos.w = 1;
    newPos = mat * newPos;
    //reconstruct plane with new normal and x0
    p.a = newNorm.x;
    p.b = newNorm.y;
    p.c = newNorm.z;
    p.d = (p.a * newPos.x + p.b * newPos.y + p.c * newPos.z);
    normalizePlane(p);
}

FrustumCorners Coord::getFrustumCorners()
{
    FrustumCorners corners;
    mat4 projInv = glm::inverse(proj3);
    mat4 viewInv = glm::inverse(view3);
    float back = 1;
    float front = -1;
    float left = -1;
    float right = 1;
    float up = 1;
    float down = -1;
    vec4 lub = {left, up, back, 1};
    //PRINT("lub 1: <" << lub.x << ", " << lub.y << ", " << lub.z << ", " << lub.w << ">")
    vec4 rub = {right, up, back, 1};
    vec4 rdb = {right, down, back, 1};
    vec4 ldb = {left, down, back, 1};
    vec4 luf = {left, up, front, 1};
    vec4 ruf = {right, up, front, 1};
    vec4 rdf = {right, down, front, 1};
    vec4 ldf = {left, down, front, 1};
    lub = projInv * lub;
    //PRINT("lub 2: <" << lub.x << ", " << lub.y << ", " << lub.z << ", " << lub.w << ">")
    //PRINT("luf 2: <" << luf.x << ", " << luf.y << ", " << luf.z << ", " << luf.w << ">")
    rub = projInv * rub;
    rdb = projInv * rdb;
    ldb = projInv * ldb;
    luf = projInv * luf;
    ruf = projInv * ruf;
    rdf = projInv * rdf;
    ldf = projInv * ldf;
    lub /= lub.w;
    //PRINT("lub 3: <" << lub.x << ", " << lub.y << ", " << lub.z << ", " << lub.w << ">")
    rub /= rub.w;
    rdb /= rdb.w;
    ldb /= ldb.w;
    luf /= luf.w;
    ruf /= ruf.w;
    rdf /= rdf.w;
    ldf /= ldf.w;
    lub = viewInv * lub;
    //PRINT("lub 4: <" << lub.x << ", " << lub.y << ", " << lub.z << ", " << lub.w << ">")
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
    //PRINT("lub final: <" << lub.x << ", " << lub.y << ", " << lub.z << ", " << lub.w << ">")
    //PRINT("luf final: <" << luf.x << ", " << luf.y << ", " << luf.z << ", " << luf.w << ">")
    vec4 ruVec = normalize(ruf - rub);
    vec4 rdVec = normalize(rdf - rdb);
    vec4 ldVec = normalize(ldf - ldb);
    float luMult = -lub.y / luVec.y;
    float ruMult = -rub.y / ruVec.y;
    float rdMult = -rdb.y / rdVec.y;
    float ldMult = -ldb.y / ldVec.y;
    corners.upperLeft = {camPos.x + luMult * luVec.x, camPos.z + luMult * luVec.z};
    corners.upperRight = {camPos.x + ruMult * ruVec.x, camPos.z + ruMult * ruVec.z};
    corners.lowerRight = {camPos.x + rdMult * rdVec.x, camPos.z + rdMult * rdVec.z};
    corners.lowerLeft = {camPos.x + ldMult * ldVec.x, camPos.z + ldMult * ldVec.z};
    /*
    PRINT("Corners:")
    PRINT("Upper-left: " << corners.upperLeft.x << ", " << corners.upperLeft.y)
    PRINT("Upper-right: " << corners.upperRight.x << ", " << corners.upperRight.y)
    PRINT("Lower-right: " << corners.lowerRight.x << ", " << corners.lowerRight.y)
    PRINT("Lower-left: " << corners.lowerLeft.x << ", " << corners.lowerLeft.y)
     */
    return corners;
}

void Coord::normalizePlane(Plane& p)
{
    float mag = sqrt(p.a * p.a + p.b * p.b + p.c * p.c);
    p.a /= mag;
    p.b /= mag;
    p.c /= mag;
    p.d /= mag;
}

Plane Coord::getClipPlane(ClipPlane which, mat4& mat)
{
    Plane p;
    switch(which)
    {
        case TOP:
            p.a = mat[3][0] - mat[1][0];
            p.b = mat[3][1] - mat[1][1];
            p.c = mat[3][2] - mat[1][2];
            p.d = mat[3][3] - mat[1][3];
            normalizePlane(p);
            break;
        case LEFT:
            p.a = mat[3][0] + mat[0][0];
            p.b = mat[3][1] + mat[0][1];
            p.c = mat[3][2] + mat[0][2];
            p.d = mat[3][3] + mat[0][3];
            normalizePlane(p);
            break;
        case RIGHT:
            p.a = mat[3][0] - mat[0][0];
            p.b = mat[3][1] - mat[0][1];
            p.c = mat[3][2] - mat[0][2];
            p.d = mat[3][3] - mat[0][3];
            normalizePlane(p);
            break;
        case BOTTOM:
            p.a = mat[3][0] + mat[1][0];
            p.b = mat[3][1] + mat[1][1];
            p.c = mat[3][2] + mat[1][2];
            p.d = mat[3][3] + mat[1][3];
            normalizePlane(p);
            break;
        case NEAR:
            p.a = mat[3][0] + mat[2][0];
            p.b = mat[3][1] + mat[2][1];
            p.c = mat[3][2] + mat[2][2];
            p.d = mat[3][3] + mat[2][3];
            normalizePlane(p);
            break;
        case FAR:
            p.a = mat[3][0] - mat[2][0];
            p.b = mat[3][1] - mat[2][1];
            p.c = mat[3][2] - mat[2][2];
            p.d = mat[3][3] - mat[2][3];
            normalizePlane(p);
            break;
    }
    return p;
}