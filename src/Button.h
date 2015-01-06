//
//  Button.h
//  MagIndev
//
//  Created by Brian Kelley on 10/22/14295.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#ifndef __MagIndev__Button__
#define __MagIndev__Button__

#ifdef __APPLE__
#include <SDL2/SDL.h>
#elif __linux
#include <SDL2/SDL.h>
#elif _WIN32
#include <SDL2\SDL.h>
#endif
#include <stdio.h>
#include <iostream>
#include <string>
#include "Component.h"
#include "Constants.h"

class Button
{
public:
    Button(int x, int y, int width, int height, std::string text, callback_t callback);
    ~Button();
    void processMouse(int x, int y);
    SDL_Point& getTextLoc();
    std::string& getText();
    bool isMouseOver();
    float getFontScale();
    void updateSize();
    callback_t getCallback();
    intRect_t& getIntRect();
    floatRect_t& getFloatRect();
    void calcTextPlacement();
    void setHover(bool hovering);
    bool getHover();
    int getCompID();
private:
    int compID;
    SDL_Point textLoc;
    bool over;
    std::string text;
    float fontScale;
    callback_t callback;
    bool hover;
};

#endif /* defined(__MagIndev__Button__) */
