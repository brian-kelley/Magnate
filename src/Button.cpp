//
//  Button.cpp
//  MagIndev
//
//  Created by Brian Kelley on 10/22/14295.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "Button.h"

using namespace std;

Button::Button(int x, int y, int width, int height, string text)
{
    this->rect.x = x;
    this->rect.y = y;
    this->rect.w = width;
    this->rect.h = height;
    this->text = text;
    float fontScaleW;
    float fontScaleH;
    fontScaleW = ((float) width - constants::BORDER_WIDTH * 2 - constants::PAD * 2) / ((int) text.size() * constants::FONTW);
    fontScaleH = ((float) height - constants::BORDER_WIDTH * 2 - constants::PAD * 2) / constants::FONTH;
    if(fontScaleW < fontScaleH)
    {
        this->fontScale = fontScaleW;
        this->textLoc.x = constants::BORDER_WIDTH + constants::PAD;
        this->textLoc.y = (this->rect.h - constants::FONTH * fontScale) / 2;
    }
    else
    {
        this->fontScale = fontScaleH;
        this->textLoc.y = constants::BORDER_WIDTH + constants::PAD;
        this->textLoc.x = (this->rect.w - (int) text.size() * fontScale * constants::FONTW) / 2;
    }
    this->over = false;
}

Button::~Button()
{
    
}

void Button::processMouse(int x, int y)
{
    if(x > this->rect.x && x < this->rect.x + this->rect.w && y > this->rect.y && y < this->rect.y + this->rect.h)
    {
        this->over = true;
    }
    else
    {
        this->over = false;
    }
}

bool Button::isMouseOver()
{
    return over;
}