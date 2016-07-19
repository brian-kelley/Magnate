#include "Geom2D.h"

namespace Geom2D
{

bool lineSegmentsCross(vec2 p1, vec2 p2, vec2 q1, vec2 q2)
{
    //first, if segments are identical, return true
    if((p1 == q1 && p2 == q2) || (p1 == q2 && p2 == q1))
        return true;
    //if p and q share one endpoint, return false
    if(p1 == q1 || p2 == q1 || p1 == q2 || p2 == q2)
        return false;
    vec3 pp1(p1.x, 0, p1.y);
    vec3 pp2(p2.x, 0, p2.y);
    vec3 qq1(q1.x, 0, q1.y);
    vec3 qq2(q2.x, 0, q2.y);
    //check if points of q1 is on opposite side of p as q2 (assuming p infinite)
    vec3 pdir = pp2 - pp1;
    vec3 d1 = qq1 - pp1;
    vec3 d2 = qq2 - pp1;
    if(cross(d1, pdir).y > 0 == cross(d2, pdir).y > 0)
    {
        //q1, q2 on same side of p, so can't cross
        return false;
    }
    vec3 qdir = qq2 - qq1;
    d1 = pp1 - qq1;
    d2 = pp2 - qq1;
    if(cross(d1, qdir).y > 0 == cross(d2, qdir).y > 0)
    {
        //p1, p2 on same side of q, so can't cross
        return false;
    }
    return true;
}

int pointLineSide(vec2 p, vec2 q1, vec2 q2)
{
    //return 0 if p collinear with (q1 -> q2), -1 if left, 1 if right
    vec2 pdir = p - q1;
    vec2 qdir = q2 - q1;
    //take 2d cross product (x and y are zero, so compute z)
    float crossProd = pdir.x * qdir.y - pdir.y * qdir.x;
    if(fabs(crossProd) < 1e-8)
        return 0;
    else if(crossProd < 0)
        return -1;
    return 1;
}

float pointLineDist(vec2 p0, vec2 p1, vec2 p2)
{
    //from wikipedia
    float numer = fabsf((p2.y - p1.y) * p0.x -
                        (p2.x - p1.x) * p0.y +
                        p2.x * p1.y - p2.y * p1.x);
    float denom = sqrtf((p2.y - p1.y) * (p2.y - p1.y)
                      + (p2.x - p1.x) * (p2.x - p1.x));
    return numer / denom;
}

}
