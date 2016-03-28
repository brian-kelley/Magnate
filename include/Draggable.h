#ifndef __DRAGGABLE_H__
#define __DRAGGABLE_H__

#include <iostream>
#include <vector>
#include "Component.h"
#include "Building.h"

//Structure to describe how to draw each model
//angle = yaw rotation
//may need entire model matrix in future
struct ModelDrawParams
{
    float angle;
};

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
    static void initParams();
    static std::vector<ModelDrawParams> params; //params for all models
    static bool initialized;
private:
    int img;
    Callback onDrop;
    bool dragging;
    int clickX, clickY;     //position of cursor (rel. to my rect)
    int dragX, dragY;       //position of rect being dragged (screen) (analogous to drawRect position)
};

#endif
