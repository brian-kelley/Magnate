//
//  Draggable.h
//  Magnate
//
//  Created by Brian Kelley on 4/7/1597.
//
//

#ifndef Magnate_Draggable_h
#define Magnate_Draggable_h

#include <iostream>
#include "Constants.h"
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
    Draggable(int x, int y, int width, int height, u8 stickyFlags, int imageID, Component* parentComp, callback_t whenDropped, DRAG_IMAGE_TYPES imgType);
    void processLeftClick();
    void processMouseMotion();
    Point getDragPos();
    void deactivate();
    DRAG_IMAGE_TYPES getImageType();
    int getImageNum();
private:
    int img;
    callback_t onDrop;
    bool dragging;
    int clickX, clickY;     //position of cursor (rel. to my rect)
    int dragX, dragY;       //position of rect being dragged (screen) (analogous to drawRect position)
    DRAG_IMAGE_TYPES imgType;
};

#endif