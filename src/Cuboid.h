//
//  Cuboid.h
//  Magnate
//
//  Created by Brian Kelley on 10/15/14288.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#ifndef __Magnate__Cuboid__
#define __Magnate__Cuboid__

#include <iostream>
#include "Coord.h"
#include "Constants.h"

class Cuboid
{
public:
    Cuboid(double x, double y, double z,
           double length, double width, double height,
           int leftWall, int rightWall, int roof);
    virtual ~Cuboid();
    void initDrawCoords();
    double getX();
    double getY();
    double getZ();
    double getWidth();
    double getLength();
    double getHeight();
    int getLeft();
    int getRight();
    int getRoof();
    /*************************
        ----5---
     2--|      |---6
     |   ---3---    |
     |      |       |
     1---   |   ----7
         ---4---
     *************************/
    struct draw_t
    {
        short x2;
        short y2;
        short x3;
        short y3;
        short x4;
        short y4;
        short x5;
        short y5;
        short x6;
        short y6;
        short x7;
        short y7;
    } draw;
protected:
private:
    double x;
    double y;
    double z;
    double length;
    double width;
    double height;
    int left;
    int right;
    int roof;
};

#endif /* defined(__Magnate__Cuboid__) */