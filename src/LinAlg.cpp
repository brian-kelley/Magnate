#include "LinAlg.h"

using namespace std;
using namespace glm;

static bool approxZero(double f)
{
    return -1e-12 <= f && f <= 1e-12;
}

bool LinAlg::isOnto(system2 &mat)
{
    //return false if det of first 2 columns is 0, true otherwise
    if(approxZero(mat[0][0] * mat[1][1] - mat[1][0] * mat[0][1]))
        return false;
    return true;
}

bool LinAlg::solveSystem2(system2 &mat)
{
    //use Cramer's rule
    double det = mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
    if(approxZero(det))
    {
        return false; //no clean (single) solution - matrix not invertible
    }
    double x = (mat[2][0] * mat[1][1]) - (mat[1][0] * mat[2][1]);
    double y = (mat[0][0] * mat[2][1]) - (mat[2][0] * mat[0][1]);
    mat[2][0] = x / det;
    mat[2][1] = y / det;
    return true;
}

void LinAlg::rowSwap(system2 &mat, int r1, int r2)
{
    float temp = mat[0][r1];
    mat[0][r1] = mat[0][r2];
    mat[0][r2] = temp;
    temp = mat[1][r1];
    mat[1][r1] = mat[1][r2];
    mat[1][r2] = temp;
    temp = mat[2][r1];
    mat[2][r1] = mat[2][r2];
    mat[2][r2] = temp;
}

void LinAlg::rowScaleAndSub(system2 &mat, int prodRow, int destRow, float scl)
{
    mat[0][destRow] -= scl * mat[0][prodRow];
    mat[1][destRow] -= scl * mat[1][prodRow];
    mat[2][destRow] -= scl * mat[2][prodRow];
}

void LinAlg::rowScale(system2 &mat, int r, float scl)
{
    mat[0][r] *= scl;
    mat[1][r] *= scl;
    mat[2][r] *= scl;
}

void LinAlg::printSys2(system2 &mat)
{
    printf("\n[ %-20.17f %-20.17f %-20.17f ]\n", mat[0][0], mat[1][0], mat[2][0]);
    printf("[ %-20.17f %-20.17f %-20.17f ]\n", mat[0][1], mat[1][1], mat[2][1]);
}

void LinAlg::printMat4(mat4 &mat)
{
    printf("\n[ %-10.4f %-10.4f %-10.4f %-10.4f ]", mat[0][0], mat[1][0], mat[2][0], mat[3][0]);
    printf("\n[ %-10.4f %-10.4f %-10.4f %-10.4f ]", mat[0][1], mat[1][1], mat[2][1], mat[3][1]);
    printf("\n[ %-10.4f %-10.4f %-10.4f %-10.4f ]", mat[0][2], mat[1][2], mat[2][2], mat[3][2]);
    printf("\n[ %-10.4f %-10.4f %-10.4f %-10.4f ]\n\n", mat[0][3], mat[1][3], mat[2][3], mat[3][3]);
}