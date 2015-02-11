//
//  Field.cpp
//  MagIndev
//
//  Created by Brian Kelley on 10/22/14295.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "Field.h"

using namespace std;
using namespace componentHandler;

Field::Field(int x, int y, int width, int height, string text, callback_t callback, Component* parentComp)
{
    Component(x, y, width, height, parentComp);
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
    }
    else if(e.type == SDL_KEYDOWN)
    {
        if(e.key.keysym.scancode == SDL_SCANCODE_BACKSPACE)
        {
            if(this->text.size() > 0)
            {
                this->text = this->text.substr(0, (unsigned long) text.length() - 2) + "_";
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
    this->fontScale = float(getCompIntRect(compID).h - constants::PAD * 2) / constants::FONTH;
}

int Field::getCompID()
{
    return compID;
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
    (*callback) (compID);
}