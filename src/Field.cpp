//
//  Field.cpp
//  MagIndev
//
//  Created by Brian Kelley on 10/22/14295.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "Field.h"

using namespace std;

Field* Field::currentField = nullptr;

Field::Field(int x, int y, int width, int height, string text, callback_t callback, Component* parentComp) : Component(x, y, width, height, true, parentComp, CTYPE::FIELD)
{
    this->text = text;
    this->callback = callback;
    calcTextPlacement();
}

string& Field::getText()
{
    return this->text;
}

void Field::setText(string newtext)
{
    this->text = newtext;
}

void Field::processKey(SDL_Event& e)
{
    if(e.type == SDL_TEXTINPUT)
    {
        text = text.substr(0, text.length() - 1) + string(e.text.text) + "_";
        calcTextPlacement();
    }
    else if(e.type == SDL_KEYDOWN)
    {
        if(e.key.keysym.scancode == SDL_SCANCODE_BACKSPACE)
        {
            if(this->text.size() > 1)
            {
                this->text = this->text.substr(0, (unsigned long) text.length() - 2) + "_";
                calcTextPlacement();
            }
        }
        else if(e.key.keysym.scancode == SDL_SCANCODE_RETURN)
        {
            deactivate();
        }
    }
}

float Field::getFontScale()
{
    return this->fontScale;
}

void Field::calcTextPlacement()
{
    float horiScale;
    if(text.size() > 0)
    {
        horiScale = float(localRect.w - constants::PAD * 2) / (text.size() * constants::FONTW);
    }
    else
    {
        horiScale = 100000;
    }
    float vertScale = float(localRect.h - constants::PAD * 2) / constants::FONTH;
    fontScale = horiScale < vertScale ? horiScale : vertScale;
    textLoc.x = constants::PAD;
    textLoc.y = (localRect.h / 2) - (fontScale * constants::FONTH / 2);
}

callback_t Field::getCallback()
{
    return callback;
}

bool Field::isActive()
{
    return active;
}

void Field::activate()
{
    text = text.append("_");
    hasCursor = true;
    this->calcTextPlacement();
}

void Field::deactivate()
{
    if(hasCursor)
    {
        text = text.substr(0, int(text.length() - 1));
        hasCursor = false;
    }
    this->calcTextPlacement();
    if(callback)
    {
        (*callback) (0);
    }
}

void Field::processLeftClick()
{
    activate();
    currentField = this;
}

SDL_Point& Field::getTextLoc()
{
    return textLoc;
}