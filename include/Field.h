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
#include "SDL.h"
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

class Field : public Component
{
public:
    //The field object that currently has kbd focus, if any
    static Field* currentField;
    Field(int x, int y, int width, int height, std::string text, callback_t callback, Component* parentComp);
    void setText(std::string text);
    void setLocation(int x, int y);
    void setSize(int width, int height);
    void processKey(SDL_Event& e);
    float getFontScale();
    std::string& getText();
    void updateSize();
    intRect_t& getIntRect();
    floatRect_t& getFloatRect();
    callback_t getCallback();
    bool isActive();
    void activate();
    void deactivate();
    void calcTextPlacement();
    void processLeftClick();
    SDL_Point& getTextLoc();
private:
    std::string text;
    float fontScale;
    callback_t callback;
    bool active;
    bool hasCursor;
    SDL_Point textLoc;
};

#endif