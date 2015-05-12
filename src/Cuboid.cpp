//
//  Cuboid.cpp
//  Magnate
//
//  Created by Brian Kelley on 10/15/14288.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "Cuboid.h"

using namespace std;

Cuboid::Cuboid(double x, double y, double z, double length, double width, double height, int wall1, int wall2, int wall3, int wall4, int roof)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->length = length;
    this->width = width;
    this->height = height;
    nwWall = wall1;
    neWall = wall2;
    seWall = wall3;
    swWall = wall4;
    this->roof = roof;
}

Cuboid::~Cuboid()
{
    //no deletes necessary
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
int Cuboid::getNEWall()
{
    return neWall;
}
int Cuboid::getNWWall()
{
    return nwWall;
}
int Cuboid::getSEWall()
{
    return seWall;
}
int Cuboid::getSWWall()
{
    return swWall;
}
int Cuboid::getRoof()
{
    return this->roof;
}