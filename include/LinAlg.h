#ifndef __LIN_ALG__
#define __LIN_ALG__

#include <cstdio>
#include <iostream>
#include "GlmHeaders.h"

namespace LinAlg
{
    typedef glm::dmat3x2 system2; //describe a system with 2 unknowns and 2 linear eqns
    bool solveSystem2(system2& mat);
    bool isOnto(system2& mat);
    void rowSwap(system2& mat, int r1, int r2);
    void rowScaleAndSub(system2& mat, int prodRow, int destRow, float scl);
    void rowScale(system2& mat, int r, float scl);
    void printSys2(system2& mat);
    void printMat4(glm::mat4& mat);
}

#endif