//
//  Component.cpp
//  Magnate
//
//  Created by Brian Kelley on 12/15/14349.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "Component.h"

using namespace std;
using namespace componentHandler;
using namespace constants;

int componentHandler::numIDs = 0;
vector<component_t> componentHandler::componentList;

bool componentHandler::mouseInside(int id)
{
    intRect_t rect = getCompIntRect(id);
    if(rect.x <= mouseX && rect.x + rect.w > mouseX
       && rect.y <= mouseY && rect.y + rect.h > mouseY)
    {
        return true;
    }
    else
    {
        return false;
    }
}

component_t& getComponent(int id)
{
    return componentHandler::componentList[id];
}

int componentHandler::createComponent(int x, int y, int width, int height, bool center)
{
    if(center)
    {
        x -= width / 2;
        y -= height / 2;
    }
    numIDs++;
    component_t* newComp = new component_t;
    newComp->irect.x = x;
    newComp->irect.y = y;
    newComp->irect.w = width;
    newComp->irect.h = height;
    newComp->frect.x = (float) x / WINDOW_W;
    newComp->frect.y = (float) y / WINDOW_H;
    newComp->frect.w = (float) width / WINDOW_W;
    newComp->frect.h = (float) height / WINDOW_H;
    return numIDs;  //return the ID of the component that has just been created
}

intRect_t& componentHandler::getCompIntRect(int id)
{
    return componentList[id].irect;
}

floatRect_t& componentHandler::getCompFloatRect(int id)
{
    return componentList[id].frect;
}

void componentHandler::updateSize(int id)
{
    intRect_t itemp = getCompIntRect(id);
    floatRect_t ftemp = getCompFloatRect(id);
    itemp.x = ftemp.x * WINDOW_W;
    itemp.y = ftemp.y * WINDOW_H;
    itemp.w = ftemp.w * WINDOW_W;
    itemp.h = ftemp.h * WINDOW_H;
}
