//
//  ScrollBlock.cpp
//  Magnate
//
//  Created by Brian Kelley on 12/14/14348.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "ScrollBlock.h"

using namespace std;

ScrollBlock::ScrollBlock(int x, int y, int width, int height, bool center)
{
    rect.x = x;
    rect.y = y;
    rect.w = width;
    rect.h = height;
    if(center)
    {
        rect.x -= width / 2;
        rect.y -= height / 2;
    }
}

ScrollBlock::~ScrollBlock()
{
    
}

SDL_Rect& ScrollBlock::getRect()
{
    return this->rect;
}

void ScrollBlock::addButton(Button b)
{
    this->buttons.push_back(b);
}

void ScrollBlock::addField(Field f)
{
    this->fields.push_back(f);
}

void ScrollBlock::addLabel(Label l)
{
    this->labels.push_back(l);
}

vector<Button>& ScrollBlock::getButtons()
{
    return this->buttons;
}

vector<Label>& ScrollBlock::getLabels()
{
    return this->labels;
}

vector<Field>& ScrollBlock::getFields()
{
    return this->fields;
}