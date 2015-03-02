//
//  ScrollBlock.cpp
//  Magnate
//
//  Created by Brian Kelley on 12/14/14348.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "ScrollBlock.h"

using namespace std;
using namespace constants;

ScrollBlock::ScrollBlock(int x, int y, int width, int height, Component* parentComp, int canvh, bool center) : Component(x, y, width, height, true, parentComp, CTYPE::SCROLLBLOCK)
{
    viewport = 0;
    canvH = canvh;
    fCanvH = (float) canvH / WINDOW_H;
    calcOffsets();  //need to call this here because Component constructor doesn't
    calcDrawRect();
    calcBarPlacement();
}

void ScrollBlock::updateCanvasHeight(int newHeight) //call this when need more space for subcomponents
{
    //minimum height of canvas is height of top-level rect; least height where no scrolling
    if(newHeight < localRect.h)
    {
        canvH = localRect.h;
    }
    else
    {
        canvH = newHeight;
    }
    fCanvH = float(canvH) / WINDOW_H;
    calcBarPlacement();
}

void ScrollBlock::processScroll(SDL_MouseWheelEvent& e)
{
    if(this->hasBar())
    {
        int delta = e.y * -4;
        if(delta < 0)	//upward scrolling
        {
            viewport += delta;
            if(viewport < 0)
            {
                viewport = 0;
            }
        }
        else if(delta > 0)
        {
            viewport += delta;
            if(viewport + drawRect.h > canvH)
            {
                viewport = canvH - drawRect.h;
            }
        }
        calcOffsets();
        calcDrawRect();
        calcBarPlacement();
        for(Component* c : children)
        {
            c->calcOffsets();
            c->calcDrawRect();
        }
    }
}

void ScrollBlock::processMouseMotionEvent(SDL_MouseMotionEvent &e) {}

bool ScrollBlock::isActive()
{
    return active;
}

void ScrollBlock::calcBarPlacement()
{
    if(localRect.h >= canvH)
    {
        barHeight = -1;     //-1 means don't draw any bar
        barPos = -1;
    }
    else
    {
        barHeight = float(localRect.h) / canvH * (localRect.h - 2 * PAD);
        barPos = float(viewport + localRect.h / 2) * (localRect.h - 2 * PAD) / canvH - barHeight / 2;
    }
}

bool ScrollBlock::hasBar()
{
    if(barHeight == -1)
    {
        return false;
    }
    else
    {
        return true;
    }
}

intRect_t ScrollBlock::getBarRect()
{
    intRect_t out;
    out.w = BAR_WIDTH;
    out.h = barHeight;
    out.x = drawRect.x + drawRect.w - PAD - BAR_WIDTH;  //use actual screen pos
    out.y = drawRect.y + PAD + barPos;
    return out;
}

Field* ScrollBlock::getCurrentField()
{
    return currentField;
}

void ScrollBlock::calcOffsets()
{
    Component::calcOffsets();   //otherwise, use same xy values from Component
    yOffset -= viewport;
}

int ScrollBlock::getCanvasHeight()
{
    return canvH;
}

void ScrollBlock::matchCanvasToContents()
{
    int maxY = 0;
    int bottom;
    for(Component* c : children)
    {
        bottom = c->getLocalRect().y + c->getLocalRect().h;
        if(bottom > maxY)
        {
            maxY = bottom;
        }
    }
    updateCanvasHeight(maxY + PAD);
}