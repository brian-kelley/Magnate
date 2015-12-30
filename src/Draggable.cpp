//
//  Draggable.cpp
//  Magnate
//
//  Created by Brian Kelley on 4/7/1597.
//
//

#include "Draggable.h"

using namespace std;
using namespace constants;

Draggable* Draggable::activeDrag = nullptr;

Draggable::Draggable(int x, int y, int width, int height, u8 stickyFlags, int imageID, Component* parentComp, callback_t whenDropped, DRAG_IMAGE_TYPES imgType) : Component(x, y, width, height, stickyFlags, true, parentComp)
{
    img = imageID;
    onDrop = whenDropped;
    dragging = false;
    this->imgType = imgType;
}

void Draggable::processLeftClick()
{
    Component::activate();
    dragging = true;
    activeDrag = this;
    clickX = mouseX - screen.x;
    clickY = mouseY - screen.y;
    //Initially set dragX, dragY to match drawRect
    dragX = screen.x;
    dragY = screen.y;
}

void Draggable::processMouseMotion()
{
    if(dragging)
    {
        dragX = mouseX - clickX;
        dragY = mouseY - clickY;
    }
}

Point Draggable::getDragPos()
{
    Point p;
    p.x = dragX;
    p.y = dragY;
    return p;
}

void Draggable::deactivate()
{
    Component::deactivate();
    dragging = false;
    (*onDrop) (this);
    activeDrag = nullptr;
}

DRAG_IMAGE_TYPES Draggable::getImageType()
{
    return imgType;
}

int Draggable::getImageNum()
{
    return img;
}