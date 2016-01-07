#ifndef __HEIGHTMAP_H__
#define __HEIGHTMAP_H__

#include <iostream>
#include <climits>
#include "RandomUtils.h"
#include "Coord.h"
#include "VecField.h"
#include "GlmHeaders.h"

class Heightmap
{
public:
    Heightmap();
    Heightmap(int x, int y);
    Heightmap(const Heightmap& toCopy);
    ~Heightmap();
    void diamondSquare(double roughness, short cornerStart, short centerStart, bool isZeroMin);
    void fillSquare(int x, int y, int sz, double roughness);
    void fillDiamond(int x, int y, int sz, double roughness);
    void set(short val, int x, int y);
    void set(short val, Pos2 loc);
    short get(int x, int y) const;
    short get(Pos2 loc) const;
    void add(short val, int x, int y);
    void add(short val, Pos2 loc);
    bool validPoint(int x, int y) const;
    int getSteepness(int x, int y); //get a relative measure of steepness
    int getSteepness(Pos2 loc);
    void normalize(int val = 1000);
    glm::vec2 grad(int x, int y);   //gradient at tile center
    glm::vec2 grad(Pos2 loc);
    short* buf;
    void operator+=(Heightmap& hm);
    void operator-=(Heightmap& hm);
    Heightmap operator*(Heightmap& hm);
    void operator*=(Heightmap& hm); //2nd operand is [0,1000) scalar field
    Heightmap& operator=(Heightmap& hm);
    int getW() const;
    int getH() const;
private:
    short calcVal(int avg, int size, double roughness);
    int w;
    int h;
    bool floorZero;
};

#endif