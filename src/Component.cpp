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

Component::Component(int x, int y, int width, int height, u8 stickyFlags, bool center, Component* parentComp) : local(x, y, width, height), canvas(x, y, width, height)
{
    this->parent = parentComp;
    this->stickyFlags = stickyFlags;
    updateScreenRect();
}

Component::~Component()
{
    for(int i = 0; i < int(children.size()); i++)
    {
        delete children[i];
    }
    children.clear();
}

bool Component::isMouseOver()
{
    if(screen.x <= mouseX && screen.x + screen.w > mouseX
       && screen.y <= mouseY && screen.y + screen.h > mouseY)
        return true;
    else
        return false;
}

vector<Component*>& Component::getChildren()
{
    return children;
}

void Component::processLeftClick()
{
    for(Component* c : children)
    {
        c->processLeftClick();
    }
}

void Component::processMouseMotion()
{
    
}

void Component::processScroll(SDL_MouseWheelEvent& e)
{
    for(Component* c : children)
    {
        if(c->isMouseOver())
        {
            c->processScroll(e);
        }
    }
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

Component* Component::getParent()
{
    return parent;
}

void Component::processResize()
{
    //Get the change in parent's dimensions
    float xchg = float(parent->local.w - parentWidth) / parent->local.w;
    parentWidth = parent->local.w;
    float ychg = float(parent->local.h - parentHeight) / parent->local.h;
    parentHeight = parent->local.h;
    //If sticky (true), distance to parent edge stays constant
    //If floating, scale edge to edge distance proportionally in that direction
    int left = local.x;
    int right = parentWidth - local.x - local.w;
    int top = local.y;
    int bottom = parentHeight - local.y - local.h;
    if(!(stickyFlags & StickyDirs::left))
        left *= xchg;
    if(!(stickyFlags & StickyDirs::right))
        right *= xchg;
    if(!(stickyFlags & StickyDirs::top))
        top *= ychg;
    if(!(stickyFlags & StickyDirs::bottom))
        bottom *= ychg;
    //now set new dimensions based on new bounding box
    local.x = left;
    local.y = top;
    local.w = parentWidth - local.x;
    local.h = parentHeight - local.y;
    updateScreenRect();                 //refresh drawing rectangle
    for(auto c : children)              //pass to children
        c->processResize();
}

void Component::updateScreenRect()
{
    //w, h are same local
    screen.w = local.w;
    screen.h = local.h;
    //x, y depend on parent screen position
    //parent needs to have already calculated its own screen position
    screen.x = local.x + parent ? parent->getScreenRect().x : 0;
    screen.y = local.y + parent ? parent->getScreenRect().y : 0;
}

const Rectangle& Component::getLocalRect()
{
    return local;
}

const Rectangle& Component::getScreenRect()
{
    return screen;
}