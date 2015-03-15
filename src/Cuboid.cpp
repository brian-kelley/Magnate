//
//  Cuboid.cpp
//  Magnate
//
//  Created by Brian Kelley on 10/15/14288.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "Cuboid.h"

using namespace std;

Cuboid::Cuboid(double x, double y, double z, double length, double width, double height, int leftWall, int rightWall, int roof)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->length = length;
    this->width = width;
    this->height = height;
    this->left = leftWall;
    this->right = rightWall;
    this->roof = roof;
    initDrawCoords();
}

Cuboid::~Cuboid()
{
    //no deletes necessary
}

void Cuboid::initDrawCoords()
{
    int baseX = coord::ix(x, y);
    int baseY = coord::jy(x, y);
    draw.x2 = coord::ix(x, y) - baseX;
    draw.y2 = coord::jy(x, y) - (int) (coord::ISO_HEIGHT * (z + height)) - baseY;
    draw.x3 = coord::ix(x + length, y) - baseX;
    draw.y3 = coord::jy(x + length, y) - (int) (coord::ISO_HEIGHT * (z + height)) - baseY;
    draw.x4 = coord::ix(x + length, y) - baseX;
    draw.y4 = coord::jy(x + length, y) - (int) (coord::ISO_HEIGHT * z) - baseY;
    draw.x5 = coord::ix(x, y + width) - baseX;
    draw.y5 = coord::jy(x, y + width) - (int) (coord::ISO_HEIGHT * (z + height)) - baseY;
    draw.x6 = coord::ix(x + length, y + width) - baseX;
    draw.y6 = coord::jy(x + length, y + width) - (int) (coord::ISO_HEIGHT * (z + height)) - baseY;
    draw.x7 = coord::ix(x + length, y + width) - baseX;
    draw.y7 = coord::jy(x + length, y + width) - (int) (coord::ISO_HEIGHT * (z)) - baseY;
}

double Cuboid::getX()
{
    return this->x;
}
double Cuboid::getY()
{
    return this->y;
}
double Cuboid::getZ()
{
    return this->z;
}
double Cuboid::getWidth()
{
    return this->width;
}
double Cuboid::getLength()
{
    return this->length;
}
double Cuboid::getHeight()
{
    return this->height;
}
int Cuboid::getLeft()
{
    return this->left;
}
int Cuboid::getRight()
{
    return this->right;
}
int Cuboid::getRoof()
{
    return this->roof;
}