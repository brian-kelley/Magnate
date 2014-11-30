//
//  Label.h
//  MagIndev
//
//  Created by Brian Kelley on 10/22/14295.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#ifndef __MagIndev__Label__
#define __MagIndev__Label__

#include <stdio.h>
#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include "Constants.h"

class Label
{
public:
    Label(int x, int y, int width, int height, std::string text);
    Label(int x, int y, std::string text, float fontScale);
    void updateText(std::string text);
    void updateLocation(int x, int y);
    void updateFontSize(float newSize);
    inline std::string getText()
    {
        return this->text;
    }
    inline float getFontScale()
    {
        return this->fontScale;
    }
    inline SDL_Rect* getRect()
    {
        return &this->rect;
    }
    inline SDL_Point* getTextLoc()
    {
        return &this->textLoc;
    }
    ~Label();
private:
    std::string text;
    float fontScale;
    SDL_Rect rect;
    SDL_Point textLoc;
};

#endif