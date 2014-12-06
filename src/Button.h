//
//  Button.h
//  MagIndev
//
//  Created by Brian Kelley on 10/22/14295.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#ifndef __MagIndev__Button__
#define __MagIndev__Button__

#include <stdio.h>
#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include "Constants.h"

class Button
{
public:
    Button(int x, int y, int width, int height, std::string text);
    ~Button();
    void processMouse(int x, int y);
    inline SDL_Rect& getRect();
    inline SDL_Point& getTextLoc();
    inline std::string& getText();
    bool isMouseOver();
    float getFontScale();
private:
    SDL_Rect rect;
    SDL_Point textLoc;
    bool over;
    std::string text;
    float fontScale;
};

#endif /* defined(__MagIndev__Button__) */
