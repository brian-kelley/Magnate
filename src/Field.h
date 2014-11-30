//
//  Field.h
//  MagIndev
//
//  Created by Brian Kelley on 10/22/14295.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#ifndef __MagIndev__Field__
#define __MagIndev__Field__

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>

class Field
{
public:
    Field(int x, int y, int width, int height, bool centered, int font);
    ~Field();
    void setText(std::string text);
    void setLocation(int x, int y);
    void setSize(int width, int height);
    void processKey(SDL_Event* ev);
    std::string getText();
private:
    std::vector<char> text;
    int x;
    int y;
    int width;
    int height;
    bool centered;
    //0 (small), 1 (med), 2 (large) or something
    int font;
};

#endif