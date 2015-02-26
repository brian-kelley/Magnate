//
//  ScrollBlock.h
//  Magnate
//
//  Created by Brian Kelley on 12/14/14348.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#ifndef __Magnate__ScrollBlock__
#define __Magnate__ScrollBlock__

#ifdef __APPLE__
#include "SDL.h"
#elif __linux
#include <SDL2/SDL.h>
#elif _WIN32
#include <SDL2\SDL.h>
#endif

#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include "Constants.h"
#include "Button.h"
#include "Label.h"
#include "Field.h"

class ScrollBlock : public Component
{
public:
    ScrollBlock(int x, int y, int width, int height, Component* parentComp, int canvh, bool center = true);
    void updateCanvasHeight(int newHeight);
    void processScroll(SDL_MouseWheelEvent& e);
    void processMouseMotionEvent(SDL_MouseMotionEvent& e);
    Field* getCurrentField();
    bool isActive();
    void activate();
    void deactivate();
    void calcBarPlacement();
    intRect_t getBarRect();
    bool hasBar();
    void calcOffsets(); //override component method since SB can scroll
private:
    int canvH;            //height of the canvas, in pixels
    float fCanvH;         //height of the canvas relative to window height
    int viewport;         //how far from top of canvas top of viewable area is
    bool active;
    int barHeight;        //how many pixels tall is the bar?
    int barPos;
    Field* currentField;
};

#endif /* defined(__Magnate__ScrollBlock__) */

/*
 -when drawing scroll blocks, compare each subcomponent to the top/bottom edge of viewport and alter subcomponent drawing routine to clip to edge
 -when scroll block receives mouse wheel event, modify viewport accordingly and then clamp viewport to canvas boundaries
 -when scrollblock receives click event, convert to local canvas position and process like Control does for scenes
 -scroll block needs to have Label* currentLabel to process kbd input (the sb itself will have the keyboard focus within top-level scene
 */