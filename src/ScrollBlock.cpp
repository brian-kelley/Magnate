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

ScrollBlock::ScrollBlock(int x, int y, int width, int height, int canvh, bool center)
{
	compID = createComponent(x, y, width, height, center);
    this->canvH = canvh;
    fCanvH = (float) canvH / WINDOW_H;
    refreshModifiers();
}

ScrollBlock::~ScrollBlock()
{
    
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

void ScrollBlock::updateSize()      //updateSize will handle resizing subcomponents as well
{
    componentHandler::updateSize(compID);   //updates top-level rectangle rel. to window
    canvH = fCanvH * WINDOW_H;              //also adjust canvas height
    intRect_t* sbRect = &getCompIntRect(compID);
    intRect_t* itemp;
    floatRect_t* ftemp;
    for(int i = 0; i < int(buttons.size()); i++)
    {
        itemp = &getCompIntRect(buttons[i].getCompID());
        ftemp = &getCompFloatRect(buttons[i].getCompID());      //remember, float rect rel. to SB
        itemp->x = ftemp->x * sbRect->w;       //canvas and top-level widths equal
        itemp->y = ftemp->y * canvH;
        itemp->w = ftemp->w * sbRect->w;
        itemp->h = ftemp->h * canvH;
        buttons[i].calcTextPlacement();
    }
    for(int i = 0; i < int(fields.size()); i++)
    {
        itemp = &getCompIntRect(fields[i].getCompID());
        ftemp = &getCompFloatRect(buttons[i].getCompID());
        itemp->x = ftemp->x * sbRect->w;
        itemp->y = ftemp->y * canvH;
        itemp->w = ftemp->w * sbRect->w;
        itemp->h = ftemp->h * canvH;
        fields[i].calcTextPlacement();
    }
    for(int i = 0; i < int(labels.size()); i++)
    {
        itemp = &getCompIntRect(labels[i].getCompID());
        ftemp = &getCompFloatRect(labels[i].getCompID());
        itemp->x = ftemp->x * sbRect->w;
        itemp->y = ftemp->y * canvH;
        itemp->w = ftemp->w * sbRect->w;
        itemp->h = ftemp->h * canvH;
        fields[i].calcTextPlacement();
    }
}

void ScrollBlock::updateCanvasRect(int newHeight) //call this when need more space for subcomponents
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
}

void ScrollBlock::processButtonEvent(SDL_MouseButtonEvent &e)
{
    //e occured at (mouseX, mouseY)
    //need to convert that to canvas position to compare to local subcomponents
    if(e.button == SDL_BUTTON_LEFT)
    {
        int canvMouseX = mouseX + xOffset;
        int canvMouseY = mouseY + yOffset;
        for(int i = 0; i < int(this->buttons.size()); i++)
        {
            intRect_t* btnRect = &getCompIntRect(buttons[i].getCompID());
            if(btnRect->x <= canvMouseX && btnRect->x + btnRect->w > canvMouseX
               && btnRect->y <= canvMouseY && btnRect->y + btnRect->h > canvMouseY)
            {
                (*buttons[i].getCallback()) (buttons[i].getCompID());     //deref & exec button's callback
            }
        }
        for(int i = 0; i < int(this->fields.size()); i++)
        {
            intRect_t* fldRect = &getCompIntRect(fields[i].getCompID());
            if(fldRect->x <= canvMouseX && fldRect->x + fldRect->w > canvMouseX
               && fldRect->y <= canvMouseY && fldRect)
            {
                
            }
        }
    }
}

void ScrollBlock::processScrollEvent(SDL_MouseWheelEvent& e)
{
    int delta = 10 * e.y;
	if(delta > 0)	//upward scrolling
	{
        viewport -= delta;
        if(viewport < 0)
        {
            viewport = 0;
        }
    }
    else if(delta < 0)
    {
        viewport += delta;
        if(viewport + getCompIntRect(compID).h > canvH)
        {
            viewport = canvH - getCompIntRect(compID).h;
        }
    }
    refreshModifiers();
}

int ScrollBlock::getCompID()
{
	return compID;
}

void ScrollBlock::refreshModifiers()
{
    xOffset = -getCompIntRect(compID).x;
    yOffset = viewport - getCompIntRect(compID).y;
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
            
        }
    }
}