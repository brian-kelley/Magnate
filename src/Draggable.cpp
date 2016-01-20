#include "Draggable.h"

using namespace std;

Draggable* Draggable::activeDrag = nullptr;

Draggable::Draggable(int x, int y, int width, int height, u8 stickyFlags, int imageID, Component* parentComp, Callback whenDropped, DRAG_IMAGE_TYPES imgType) : Component(x, y, width, height, stickyFlags, true, parentComp)
{
    img = imageID;
    onDrop = whenDropped;
    dragging = false;
    this->imgType = imgType;
}

void Draggable::processLeftClick()
{
    dragging = true;
    activeDrag = this;
    clickX = Input::mouseX - screen.x;
    clickY = Input::mouseY - screen.y;
    //Initially set dragX, dragY to match drawRect
    dragX = screen.x;
    dragY = screen.y;
}

void Draggable::processMouseMotion()
{
    if(dragging)
    {
        dragX = Input::mouseX - clickX;
        dragY = Input::mouseY - clickY;
    }
}

Pos2 Draggable::getDragPos()
{
    return Pos2(dragX, dragY);
}

void Draggable::deactivate()
{
    dragging = false;
    onDrop(this);
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