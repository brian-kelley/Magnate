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
using namespace componentHandler;

ScrollBlock::ScrollBlock(int x, int y, int width, int height, Component* parentComp, int canvh, bool center)
{
    Component(x, y, width, height, parentComp);
    viewport = 0;
    canvH = canvh;
    fCanvH = (float) canvH / WINDOW_H;
    refreshModifiers();
    calcBarPlacement();
}

void ScrollBlock::addButton(Button b)
{
    this->buttons.push_back(b);
}

void ScrollBlock::addField(Field f)
{
    this->fields.push_back(f);
}

void ScrollBlock::addLabel(Label l)
{
    this->labels.push_back(l);
}

vector<Button>& ScrollBlock::getButtons()
{
    return this->buttons;
}

vector<Label>& ScrollBlock::getLabels()
{
    return this->labels;
}

vector<Field>& ScrollBlock::getFields()
{
    return this->fields;
}

void ScrollBlock::updateSBSize()      //updateSize will handle resizing subcomponents as well
{
    componentHandler::updateSize(compID);   //updates top-level rectangle rel. to window
    canvH = fCanvH * WINDOW_H;              //also adjust canvas height
    for(int i = 0; i < int(buttons.size()); i++)
    {
        updateSize(buttons[i].getCompID());
        buttons[i].calcTextPlacement();
    }
    for(int i = 0; i < int(fields.size()); i++)
    {
        updateSize(fields[i].getCompID());
        fields[i].calcTextPlacement();
    }
    for(int i = 0; i < int(labels.size()); i++)
    {
        updateSize(labels[i].getCompID());
        fields[i].calcTextPlacement();
    }
}

void ScrollBlock::updateCanvasHeight(int newHeight) //call this when need more space for subcomponents
{
    //minimum height of canvas is height of top-level rect; least height where no scrolling
    if(newHeight < getCompIntRect(compID).h)
    {
        canvH = getCompIntRect(compID).h;
    }
    else
    {
        canvH = newHeight;
    }
    calcBarPlacement();
}

void ScrollBlock::processButtonEvent(SDL_MouseButtonEvent &e)
{
    //e occured at (mouseX, mouseY)
    //need to convert that to canvas position to compare to local subcomponents
    if(e.button == SDL_BUTTON_LEFT)
    {
        int canvMouseX = mouseX - xOffset;
        int canvMouseY = mouseY - yOffset;
        for(int i = 0; i < int(this->buttons.size()); i++)
        {
            if(buttons[i].isMouseOver())
            {
                (*buttons[i].getCallback()) (buttons[i].getCompID());
            }
        }
        for(int i = 0; i < int(this->fields.size()); i++)
        {
            if(fields[i].isMouseOver())
            {
                active = true;
                currentField = &fields[i];
                fields[i].activate();
            }
        }
    }
}

void ScrollBlock::processScrollEvent(SDL_MouseWheelEvent& e)
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
        refreshModifiers();
        calcBarPlacement();
    }
}

void ScrollBlock::processMouseMotionEvent(SDL_MouseMotionEvent &e)
{
    intRect_t* rect;
    int localMX = mouseX - xOffset;
    int localMY = mouseY - yOffset;
    for(int i = 0; i < int(buttons.size()); i++)
    {
        buttons[i].setMouseOver(buttons[i].isMouseOver());
    }
}

void ScrollBlock::refreshModifiers()
{
    xOffset = getCompIntRect(compID).x;
    yOffset = getCompIntRect(compID).y - viewport;
}

bool ScrollBlock::isActive()
{
    return active;
}

void ScrollBlock::activate()
{
    active = true;
}

void ScrollBlock::deactivate()
{
    active = false;
    //right now, the only effect of deactivating SB is to deactivate all its fields
    for(int i = 0; i < int(fields.size()); i++)
    {
        if(fields[i].isActive())
        {
            (*fields[i].getCallback()) (fields[i].getCompID());
            fields[i].deactivate();
        }
    }
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

int ScrollBlock::numButtons()
{
    return int(buttons.size());
}

int ScrollBlock::numLabels()
{
    return int(labels.size());
}

int ScrollBlock::numFields()
{
    return int(fields.size());
}

int ScrollBlock::getXOffset()
{
    return xOffset;
}

int ScrollBlock::getYOffset()
{
    return yOffset;
}

Field* ScrollBlock::getCurrentField()
{
    return currentField;
}

void ScrollBlock::calcOffsets()
{
    Component::calcOffsets();   //otherwise, use same xy values from Comp method
    yOffset += viewport;
}