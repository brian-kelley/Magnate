//
//  Building.cpp
//  Magnate
//
//  Created by Brian Kelley on 10/15/14288.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "Building.h"

using namespace std;

Building::Building()
{
    boxes = new vector<Cuboid>;
    //default constrcutor just tests out the rendering
    //simply make a cube that will show up on the screen when upper-left is at (0,0)
    boxes->push_back(*(new Cuboid(0, 2, 0, 1, 1, 1, 0, 0, 1)));
}

Building::~Building()
{
    delete[] this->boxes;
}

void Building::addCuboid(Cuboid* c)
{
    if(this->boxes->size() == 0)
    {
        boxes->push_back(*c);
    }
    else
    {
        for(vector<Cuboid>::iterator iter = boxes->begin(); iter != boxes->end(); ++iter)
        {
            //&*iter is a reference to the object pointed to by iter (i.e. pointer to actual cuboid)
            if(isFront(c, &(*(iter))))
            {
                this->boxes->insert(iter, *c);
                return;
            }
        }
    }
    boxes->push_back(*c);
}
//Returns true iff c1 should be drawn after (in front of) c2
bool Building::isFront(Cuboid* c1, Cuboid* c2)
{
    //vertical component decides first
    if(c1->getZ() > c2->getZ())
    {
        return true;
    }
    if(c1->getZ() < c2->getZ())
    {
        return false;
    }
    //otherwise, determine which box is "in front":
    if(jy(c1->getX(), c1->getY() + c1->getWidth()) > jy(c2->getX(), c2->getY() + c2->getWidth()))
    {
        return true;
    }
    return false;
}

Cuboid& Building::getCuboidRef(int index)
{
    return (*this->boxes)[index];
}

int Building::numCuboids()
{
    return (int) this->boxes->size();
}
