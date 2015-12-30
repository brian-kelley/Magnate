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

Field::Field(int x, int y, int width, int height, u8 stickyFlags, string text, callback_t callback, Component* parentComp) : Component(x, y, width, height, stickyFlags, true, parentComp)
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
        horiScale = float(local.w - constants::PAD * 2) / (text.size() * constants::FONTW);
    }
    else
    {
        horiScale = 100000;
    }
    float vertScale = float(local.h - constants::PAD * 2) / constants::FONTH;
    fontScale = horiScale < vertScale ? horiScale : vertScale;
    textLoc.x = constants::PAD;
    textLoc.y = (local.h / 2) - (fontScale * constants::FONTH / 2);
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
    if(currentField)
    {
        currentField->deactivate();
    }
    text = text.append("_");
    active = true;
    hasCursor = true;
    currentField = this;
    this->calcTextPlacement();
}

void Field::deactivate()
{
    if(isActive())
    {
        active = false;
        text = text.substr(0, int(text.length() - 1));
        hasCursor = false;
        currentField = nullptr;
    }
    calcTextPlacement();
    if(callback)
    {
        (*callback) (this);
    }
}

void Field::processLeftClick()
{
    if(isMouseOver())
    {
        if(!active)
        {
            activate();
            currentField = this;
        }
    }
    else
    {
        if(active)
            deactivate();
    }
}

SDL_Point& Field::getTextLoc()
{
    return textLoc;
}

CompType Field::getType()
{
    return CompType::field;
}