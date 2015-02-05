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
#include <SDL2/SDL.h>
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

class ScrollBlock
{
public:
    ScrollBlock(int x, int y, int width, int height, int canvh, bool center = true);
    void addButton(Button b);
    void addLabel(Label l);
    void addField(Field f);
    SDL_Rect& getRect();
    std::vector<Button>& getButtons();
    std::vector<Field>& getFields();
    std::vector<Label>& getLabels();
    int numButtons();
    int numFields();
    int numLabels();
    int getXOffset();
    int getYOffset();
    void updateSBSize();
    void updateCanvasHeight(int newHeight);
    void processScrollEvent(SDL_MouseWheelEvent& e);
    void processButtonEvent(SDL_MouseButtonEvent& e);
    void processMouseMotionEvent(SDL_MouseMotionEvent& e);
    int compID;
    Field* getCurrentField();
    int getCompID();
    bool isActive();
    void activate();
    void deactivate();
    void calcBarPlacement();
    intRect_t getBarRect();
    bool hasBar();
private:
    SDL_Rect rect;
    std::vector<Button> buttons;
    std::vector<Field> fields;
    std::vector<Label> labels;
    int canvH;            //height of the canvas, in pixels
    float fCanvH;         //height of the canvas relative to window height
    int viewport;         //how far from top of canvas top of viewable area is
    int xOffset;          //what needs to be added to screen x coordinate to get local coordinates
    int yOffset;
    void refreshModifiers();  //use component/canvas/viewport rectangles to calculate offsets
    bool active;
    int barHeight;        //how many pixels tall is the bar?
    float fBarHeight;     //relative to component size?
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