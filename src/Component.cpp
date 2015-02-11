//
//  Component.cpp
//  Magnate
//
//  Created by Brian Kelley on 12/15/14349.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "Component.h"

using namespace std;
using namespace constants;

Component::Component(int x, int y, int width, int height, Component* parentComp)
{
    intRect_t local;
    local.x = x;
    local.y = y;
    local.w = width;
    local.h = height;
    drawRect.w = width;
    drawRect.h = height;
    this->parent = parentComp;
    if(parent)
    {
        parent->addChild(this);
        localFloatRect.x = float(local.x) / parent->localRect.w;
        localFloatRect.y = float(local.y) / parent->localRect.h;
        localFloatRect.w = float(local.w) / parent->localRect.w;
        localFloatRect.h = float(local.h) / parent->localRect.h;
    }
    else
    {
        localFloatRect.x = float(local.x) / WINDOW_W;
        localFloatRect.y = float(local.y) / WINDOW_H;
        localFloatRect.w = float(local.w) / WINDOW_W;
        localFloatRect.h = float(local.h) / WINDOW_H;
    }
    calcDrawRect();
    processResize();
}

void Component::calcDrawRect()
{
    if(parent)
    {
        drawRect.x = parent->drawRect.x + localRect.x;
        drawRect.y = parent->drawRect.y + localRect.y;
    }
    else
    {
        drawRect.x = localRect.x;
        drawRect.y = localRect.y;
    }
    //w,h already set
}

void Component::processResize()
{
    if(parent)
    {
        localRect.x = localFloatRect.x * parent->localRect.w;
        localRect.y = localFloatRect.y * parent->localRect.h;
        localRect.w = localFloatRect.w * parent->localRect.w;
        localRect.h = localFloatRect.h * parent->localRect.h;
    }
    else
    {
        localRect.x = localFloatRect.x * WINDOW_W;
        localRect.y = localFloatRect.y * WINDOW_H;
        localRect.w = localFloatRect.w * WINDOW_W;
        localRect.h = localFloatRect.h * WINDOW_H;
    }
    calcOffsets();
    for(int i = 0; i < int(children.size()); i++)
    {
        children[i]->processResize();
    }
}

void Component::calcOffsets()
{
    if(parent)
    {
        xOffset = localRect.x + parent->getXOffset();
        yOffset = localRect.y + parent->getYOffset();
    }
    else
    {
        xOffset = localRect.x;
        yOffset = localRect.y;
    }
}

bool Component::isMouseOver()
{
    if(drawRect.x <= mouseX && drawRect.x + drawRect.w > mouseX
       && drawRect.y <= mouseY && drawRect.y + drawRect.h > mouseY)
    {
        return true;
    }
    else
    {
        return false;
    }
}

intRect_t& Component::getDrawRect()
{
    return drawRect;
}