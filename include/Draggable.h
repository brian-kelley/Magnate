#ifndef __DRAGGABLE_H__
#define __DRAGGABLE_H__

#include <iostream>
#include "Component.h"
#include "Building.h"

class Draggable : public Component
{
public:
    static Draggable* activeDrag;
    Draggable(int x, int y, int width, int height, u8 stickyFlags, int imageID, Component* parentComp, Callback whenDropped);
    void processLeftClick();
    void processMouseMotion();
    Pos2 getDragPos();
    void deactivate();
    int getImageNum();
private:
    int img;
    Callback onDrop;
    bool dragging;
    int clickX, clickY;     //position of cursor (rel. to my rect)
    int dragX, dragY;       //position of rect being dragged (screen) (analogous to drawRect position)
};

#endif
