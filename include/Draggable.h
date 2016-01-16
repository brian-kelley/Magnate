#ifndef __DRAGGABLE_H__
#define __DRAGGABLE_H__

#include <iostream>
#include "Component.h"
#include "Building.h"

enum DRAG_IMAGE_TYPES
{
    IMAGE,
    BUILDING
};

class Draggable : public Component
{
public:
    static Draggable* activeDrag;
    Draggable(int x, int y, int width, int height, u8 stickyFlags, int imageID, Component* parentComp, callback whenDropped, DRAG_IMAGE_TYPES imgType);
    void processLeftClick();
    void processMouseMotion();
    Pos2 getDragPos();
    void deactivate();
    DRAG_IMAGE_TYPES getImageType();
    int getImageNum();
private:
    int img;
    callback onDrop;
    bool dragging;
    int clickX, clickY;     //position of cursor (rel. to my rect)
    int dragX, dragY;       //position of rect being dragged (screen) (analogous to drawRect position)
    DRAG_IMAGE_TYPES imgType;
};

#endif