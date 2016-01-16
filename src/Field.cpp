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

Field::Field(int x, int y, int width, int height, u8 stickyFlags, string text, callback cb, Component* parentComp) : Component(x, y, width, height, stickyFlags, true, parentComp)
{
    this->text = text;
    whenUpdated = cb;
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
            if(this->text.size() > 1)
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

callback Field::getCallback()
{
    return whenUpdated;
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
    if(whenUpdated)
        whenUpdated(this);
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

CompType Field::getType()
{
    return CompType::field;
}