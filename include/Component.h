//
//  Component.h
//  Magnate
//
//  Created by Brian Kelley on 12/15/14349.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#ifndef __Magnate__Component__
#define __Magnate__Component__

#include <stdio.h>
#include <vector>
#include "Constants.h"

class Component
{
public:
    Component(int x, int y, int width, int height, Component* parentComp);
    Component* getParent();
    void calcDrawRect();        //calculate absolute x, y of comp
    void calcOffsets();
    void processResize();       //calculate int sizes based on float & parent
    int getXOffset();           //offsets used in processing mouse, etc
    int getYOffset();           //scroll panel will override
private:
    intRect_t localRect;        //int rectangle with respect to parent
    intRect_t drawRect;         //store draw coords with offset for speed
    floatRect_t localFloatRect; //store float (0 <= x,y <= 1) coords inside parent
    int xOffset;
    int yOffset;
    Component* parent;
    std::vector<Component*> children;   //components with this as parent
};

#endif