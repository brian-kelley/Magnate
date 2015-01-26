//
//  Field.h
//  MagIndev
//
//  Created by Brian Kelley on 10/22/14295.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#ifndef __MagIndev__Field__
#define __MagIndev__Field__
#ifdef __APPLE__
#include <SDL2/SDL.h>
#elif __linux
#include <SDL2/SDL.h>
#elif _WIN32
#include <SDL2\SDL.h>
#endif
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include "Component.h"
#include "Constants.h"

class Field
{
public:
    Field(int x, int y, int width, int height, std::string text, callback_t callback);
    ~Field();
    void setText(std::string text);
    void setLocation(int x, int y);
    void setSize(int width, int height);
    void processKey(SDL_Event& e);
    SDL_Point& getTextLoc();
    float getFontScale();
    std::string& getText();
    void updateSize();
    intRect_t& getIntRect();
    floatRect_t& getFloatRect();
    int getCompID();
    callback_t getCallback();
    bool isActive();
    void activate();
    void deactivate();
    void calcTextPlacement();
private:
    int compID;
    std::string text;
    SDL_Point textLoc;
    float fontScale;
    callback_t callback;
    bool active;
};

#endif
