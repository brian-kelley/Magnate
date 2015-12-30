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
#include <SDL2/SDL.h>
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
    ScrollBlock(int x, int y, int width, int height, u8 stickyFlags, Component* parentComp, int canvh, bool center = true);
    void updateCanvasHeight(int newHeight);
    void processScroll(SDL_MouseWheelEvent& e);
    void processMouseMotionEvent(SDL_MouseMotionEvent& e);
    Field* getCurrentField();
    bool isActive();
    void calcBarPlacement();
    intRect_t getBarRect();
    bool hasBar();
    int getCanvasHeight();
    void matchCanvasToContents();
    void updateScreenRect();
    CompType getType();
private:
    int viewport;         //how far from top of canvas top of viewable area is (pixels)
    int barHeight;        //how many pixels tall is the bar?
    int barPos;
    Field* currentField;
};

#endif