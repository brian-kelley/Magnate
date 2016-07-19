#include "GenTypes.h"
#include "DebugTools.h"
#include "GlmHeaders.h"

#ifndef GEOM2D_H
#define GEOM2D_H

namespace Geom2D
{
    bool lineSegmentsCross(vec2 p1, vec2 p2, vec2 q1, vec2 q2);
    int pointLineSide(vec2 p, vec2 q1, vec2 q2);
    float pointLineDist(vec2 p, vec2 q1, vec2 q2);  //EXPENSIVE!
}

#endif 
