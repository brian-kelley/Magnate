#ifndef __SCROLLBLOCK_H__
#define __SCROLLBLOCK_H__

#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include "GenTypes.h"
#include "Button.h"
#include "Label.h"
#include "Field.h"
#include "SdlHeaders.h"

class ScrollBlock : public Component
{
public:
    ScrollBlock(int x, int y, int width, int height, u8 stickyFlags, Component* parentComp, int canvh, bool center = true);
    void updateCanvasHeight(int newHeight);
    void processScroll(SDL_MouseWheelEvent& e);
    void processMouseMotionEvent(SDL_MouseMotionEvent& e);
    void calcBarPlacement();
    bool hasBar();
    int getCanvasHeight();
    void matchCanvasToContents();
    void updateScreenRect();
    CompType getType();
    float getBarHeight();
    float getBarPos();
private:
    int viewport;         //how far from top of canvas top of viewable area is (pixels)
    float barHeight;      //local rect height / canvas rect height: -1 means no bar
    float barPos;         //proportion of total canvas above top of viewport: -1 means no bar
};

#endif