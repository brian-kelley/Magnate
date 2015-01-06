//
//  Label.h
//  MagIndev
//
//  Created by Brian Kelley on 10/22/14295.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#ifndef __MagIndev__Label__
#define __MagIndev__Label__

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

class Label
{
public:
    Label(int x, int y, int width, int height, std::string text);
    int compID;
    void updateText(std::string text);
    void processSizeChange(int oldWindowW, int oldWindowH);
    std::string getText();
    float getFontScale();
    SDL_Point& getTextLoc();
    void updateSize();
    ~Label();
private:
    std::string text;
    float fontScale;
    SDL_Point textLoc;
    void calcTextPlacement();
};

#endif
