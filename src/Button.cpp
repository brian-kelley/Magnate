//
//  Button.cpp
//  MagIndev
//
//  Created by Brian Kelley on 10/22/14295.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "Button.h"

using namespace std;

Button::Button(int x, int y, int width, int height, string text, callback_t callback, Component* parentComp) : Component(x, y, width, height, true, parentComp, CTYPE::BUTTON)
{
    this->callback = callback;
    this->text = text;
    this->over = false;
    calcTextPlacement();
    over = false;
}

SDL_Point& Button::getTextLoc()
{
    return this->textLoc;
}

float Button::getFontScale()
{
    return this->fontScale;
}

string& Button::getText()
{
    return this->text;
}

callback_t Button::getCallback()
{
    return this->callback;
}

void Button::processResize()
{
    Component::processResize();
    calcTextPlacement();
}

void Button::calcTextPlacement()
{
    float fontScaleW;
    float fontScaleH;
    fontScaleW = ((float) localRect.w - constants::BORDER_WIDTH * 2 - constants::PAD * 2) / ((int) text.size() * constants::FONTW);
    fontScaleH = ((float) localRect.h - constants::BORDER_WIDTH * 2 - constants::PAD * 2) / constants::FONTH;
    if(fontScaleW < fontScaleH)
    {
        this->fontScale = fontScaleW;
        this->textLoc.x = constants::BORDER_WIDTH + constants::PAD;
        this->textLoc.y = (localRect.h - constants::FONTH * fontScale) / 2;
    }
    else
    {
        this->fontScale = fontScaleH;
        this->textLoc.y = constants::BORDER_WIDTH + constants::PAD;
        this->textLoc.x = (localRect.w - (int) text.size() * fontScale * constants::FONTW) / 2;
    }
}

void Button::processMouseMotion()
{
    if(this->isMouseOver())
    {
        activate();
    }
}

void Button::processLeftClick()
{
    (*callback) (this);
}