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
    canvas.w = width;
    canvas.h = canvh;
    canvas.x = 0;
    canvas.y = 0;
    fcanvas.w = 1;
    fcanvas.h = (float) canvas.x / WINDOW_H;
    fcanvas.x = 0;
    fcanvas.y = 0;
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

intRect_t& ScrollBlock::getCanvasRect()
{
    return canvas;
}

intRect_t& ScrollBlock::getViewportRect()
{
    return viewport;
}

void ScrollBlock::updateSize()      //updateSize will handle resizing subcomponents as well
{
	componentHandler::updateSize(compID);
    canvas.x = fcanvas.x * WINDOW_W;
    canvas.y = fcanvas.y * WINDOW_H;
    canvas.w = fcanvas.w * WINDOW_W;
    canvas.h = fcanvas.h * WINDOW_H;
    intRect_t itemp;
    floatRect_t ftemp;
    for(int i = 0; i < (int) buttons.size(); i++)
    {
        //set the new dimensions of the button to
        itemp = componentHandler::getCompIntRect(buttons[i].getCompID());
        ftemp = componentHandler::getCompFloatRect(buttons[i].getCompID());
        itemp.x = ftemp.x * canvas.w;
        itemp.y = ftemp.y * canvas.h;
        itemp.w = ftemp.w * canvas.w;
        itemp.h = ftemp.h * canvas.h;
        buttons[i].calcTextPlacement();
    }
}

void ScrollBlock::updateCanvasRect(intRect_t newCanvas) //call this when need more space for subcomponents
{
    this->canvas = newCanvas;
}

void ScrollBlock::processButtonEvent(SDL_MouseButtonEvent &e)
{
    //e occured at (mouseX, mouseY)
    //need to convert that to canvas position to compare to local subcomponents
    int canvMouseX = mouseX;
    canvMouseX -= componentHandler::getCompIntRect(compID).x;
    canvMouseX += viewport.x;
    int canvMouseY = mouseY;
    canvMouseY -= componentHandler::getCompIntRect(compID).y;
    canvMouseY += viewport.y;
    for(int i = 0; i < int(this->buttons.size()); i++)
    {
        if(mouseInside(buttons[i].getCompID()))
        {
            (*buttons[i].getCallback()) ();
        }
    }
}

void ScrollBlock::processScrollEvent(SDL_MouseWheelEvent& e)
{
    int delta = 10 * e.y;
	if(delta > 0)	//upward scrolling
	{
        viewport.y -= delta;
        if(viewport.y < canvas.y)   //clip viewport to canvas to not go too far
        {
            viewport.y = canvas.y;  //this will probably just be 0
        }
	}
	else if(delta < 0)	//downward scrolling
	{
        viewport.y += delta;
        if(viewport.y + viewport.h > canvas.y + canvas.h)
        {
            viewport.y = canvas.y + canvas.h - viewport.h;
        }
	}
}

int ScrollBlock::getCompID()
{
	return compID;
}
