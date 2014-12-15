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

#elif _WIN32

#endif

#include <stdio.h>
#include <vector>
#include <string>
#include "Button.h"
#include "Label.h"
#include "Field.h"

class ScrollBlock
{
public:
    ScrollBlock(int x, int y, int width, int height, bool center = true);
    ~ScrollBlock();
    void addButton(Button b);
    void addLabel(Label l);
    void addField(Field f);
    SDL_Rect& getRect();
    std::vector<Button>& getButtons();
    std::vector<Field>& getFields();
    std::vector<Label>& getLabels();
private:
    SDL_Rect rect;
    std::vector<Button> buttons;
    std::vector<Field> fields;
    std::vector<Label> labels;
};

#endif /* defined(__Magnate__ScrollBlock__) */
