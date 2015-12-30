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

ScrollBlock::ScrollBlock(int x, int y, int width, int height, u8 stickyFlags, Component* parentComp, int canvh, bool center) : Component(x, y, width, height, stickyFlags, true, parentComp)
{
    canvas.h = canvh;
    viewport = 0;   //start at top of scrollable area
    calcBarPlacement();
}

void ScrollBlock::updateCanvasHeight(int newHeight) //call this to get more space for subcomponents
{
    //minimum height of canvas is height of top-level rect; least height where no scrolling
    if(newHeight < local.h)
        canvas.h = local.h;
    else
        canvas.h = newHeight;
    calcBarPlacement();
    updateScreenRect();
}

void ScrollBlock::processScroll(SDL_MouseWheelEvent& e)
{
    if(this->hasBar())
    {
        int delta = e.y * -4;
        if(delta < 0)	//upward scrolling
        {
            viewport += delta;
            //clamp top of viewport
            if(viewport < 0)
                viewport = 0;
        }
        else if(delta > 0)
        {
            viewport += delta;
            //clamp bottom of viewport
            if(viewport + screen.h > canvas.h)
                viewport = canvas.h - screen.h;
        }
        updateScreenRect();
        calcBarPlacement();
        for(Component* c : children)
        {
            c->updateScreenRect();
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
    if(local.h >= canvas.h)
    {
        barHeight = -1;     //-1 means don't draw any bar
        barPos = -1;
    }
    else
    {
        barHeight = float(local.h) / canvas.h * (local.h - 2 * PAD);
        barPos = float(viewport + local.h / 2) * (local.h - 2 * PAD) / canvas.h - barHeight / 2;
    }
}

bool ScrollBlock::hasBar()
{
    if(barHeight == -1)
        return false;
    else
        return true;
}

intRect_t ScrollBlock::getBarRect()
{
    intRect_t out;
    out.w = BAR_WIDTH;
    out.h = barHeight;
    out.x = screen.x + screen.w - PAD - BAR_WIDTH;  //use actual screen pos
    out.y = screen.y + PAD + barPos;
    return out;
}

Field* ScrollBlock::getCurrentField()
{
    return currentField;
}

int ScrollBlock::getCanvasHeight()
{
    return canvas.h;
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

void ScrollBlock::updateScreenRect()
{
    Component::updateScreenRect();
    calcBarPlacement();
}