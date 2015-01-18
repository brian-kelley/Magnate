//
//  Button.cpp
//  MagIndev
//
//  Created by Brian Kelley on 10/22/14295.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "Button.h"

using namespace std;

Button::Button(int x, int y, int width, int height, string text, callback_t callback)
{
    this->callback = callback;
    this->text = text;
    this->over = false;
    this->compID = componentHandler::createComponent(x, y, width, height);
    calcTextPlacement();
    over = false;
}

Button::~Button()
{
    
}

void Button::processMouse(int x, int y)
{
    this->over = componentHandler::mouseInside(compID);
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

void Button::updateSize()
{
    componentHandler::updateSize(compID);
    calcTextPlacement();
}

void Button::calcTextPlacement()
{
    float fontScaleW;
    float fontScaleH;
    intRect_t irect = componentHandler::getCompIntRect(compID);
    fontScaleW = ((float) irect.w - constants::BORDER_WIDTH * 2 - constants::PAD * 2) / ((int) text.size() * constants::FONTW);
    fontScaleH = ((float) irect.h - constants::BORDER_WIDTH * 2 - constants::PAD * 2) / constants::FONTH;
    if(fontScaleW < fontScaleH)
    {
        this->fontScale = fontScaleW;
        this->textLoc.x = constants::BORDER_WIDTH + constants::PAD;
        this->textLoc.y = (irect.h - constants::FONTH * fontScale) / 2;
    }
    else
    {
        this->fontScale = fontScaleH;
        this->textLoc.y = constants::BORDER_WIDTH + constants::PAD;
        this->textLoc.x = (irect.w - (int) text.size() * fontScale * constants::FONTW) / 2;
    }
}

int Button::getCompID()
{
    return compID;
}

bool Button::isMouseOver()
{
    return over;
}

void Button::setMouseOver(bool isOver)
{
    over = isOver;
}