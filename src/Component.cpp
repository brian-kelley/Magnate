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

Component::Component(int x, int y, int width, int height, bool center, Component* parentComp, CTYPE newType)
{
    this->type = newType;
    localRect.x = x;
    localRect.y = y;
    if(center)
    {
        localRect.x -= (width >> 1);
        localRect.y -= (height >> 1);
    }
    localRect.w = width;
    localRect.h = height;
    drawRect.w = width;
    drawRect.h = height;
    parent = parentComp;
    if(parent)
    {
        parent->addChild(this);
        localFloatRect.x = float(localRect.x) / parent->localRect.w;
        localFloatRect.y = float(localRect.y) / parent->localRect.h;
        localFloatRect.w = float(localRect.w) / parent->localRect.w;
        localFloatRect.h = float(localRect.h) / parent->localRect.h;
    }
    else
    {
        localFloatRect.x = float(localRect.x) / WINDOW_W;
        localFloatRect.y = float(localRect.y) / WINDOW_H;
        localFloatRect.w = float(localRect.w) / WINDOW_W;
        localFloatRect.h = float(localRect.h) / WINDOW_H;
    }
    calcOffsets();
    calcDrawRect();
}

Component::~Component()
{
    for(int i = 0; i < int(children.size()); i++)
    {
        delete children[i];
    }
    children.clear();
}

void Component::calcDrawRect()
{
    if(parent)
    {
        drawRect.x = localRect.x + parent->getXOffset();
        drawRect.y = localRect.y + parent->getYOffset();
    }
    else
    {
        drawRect.x = localRect.x;
        drawRect.y = localRect.y;
    }
}

void Component::calcOffsets()
{
    xOffset = localRect.x;
    yOffset = localRect.y;
    if(parent)
    {
        xOffset += parent->getXOffset();
        yOffset += parent->getYOffset();
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

vector<Component*>& Component::getChildren()
{
    return children;
}

void Component::processLeftClick()
{
    for(Component* c : children)
    {
        if(c->isMouseOver())
        {
            c->processLeftClick();
        }
    }
}

void Component::processMouseMotion()
{
    
}

void Component::processScroll(SDL_MouseWheelEvent)
{
    
}

void Component::activate()
{
    active = true;
}

void Component::deactivate()
{
    active = false;
}

bool Component::isActive()
{
    return active;
}

void Component::addChild(Component* child)
{
    children.push_back(child);
}

int Component::getXOffset()
{
    return xOffset;
}

int Component::getYOffset()
{
    return yOffset;
}

Component* Component::getParent()
{
    return parent;
}

void Component::processResize()
{
    if(parent)
    {
        localRect.x = parent->localRect.w * localFloatRect.x;
        localRect.y = parent->localRect.h * localFloatRect.y;
        localRect.w = parent->localRect.w * localFloatRect.w;
        localRect.h = parent->localRect.h * localFloatRect.h;
        drawRect.w = localRect.w;
        drawRect.h = localRect.h;
        calcOffsets();
    }
    else
    {
        localRect.x = WINDOW_W * localFloatRect.x;
        localRect.y = WINDOW_H * localFloatRect.y;
        localRect.w = WINDOW_W * localFloatRect.w;
        localRect.h = WINDOW_H * localFloatRect.h;
        drawRect.w = localRect.w;
        drawRect.h = localRect.h;
        calcOffsets();
        for(Component* c : children)
        {
            c->processResize();
        }
    }
    calcDrawRect();
}

CTYPE Component::getType()
{
    return type;
}