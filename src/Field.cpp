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

Field::Field(int x, int y, int width, int height, string text, fieldCallback_t callback)
{
	compID = createComponent(x, y, width, height, true);
    this->text = text;
    this->callback = callback;
    calcTextPlacement();
}

Field::~Field() {}

string& Field::getText()
{
    return this->text;
}

void Field::setText(string newtext)
{
    this->text = newtext;
}

void Field::processKey(SDL_Event *ev)
{
    if(ev->type == SDL_TEXTINPUT)
    {
        this->text.append(string(ev->text.text));
    }
    else if(ev->type == SDL_KEYDOWN)
    {
        if(ev->key.keysym.scancode == SDL_SCANCODE_BACKSPACE)
        {
            if(this->text.size() > 0)
            {
                this->text = this->text.substr(0, (unsigned long) text.length() - 1);
            }
        }
    }
}

float Field::getFontScale()
{
    return this->fontScale;
}

SDL_Point& Field::getTextLoc()
{
    return this->textLoc;
}

void Field::calcTextPlacement()
{
	textLoc.x = getCompIntRect(compID).x + 3 * constants::PAD / 2;
	textLoc.y = textLoc.x;
    this->fontScale = (float) (getCompIntRect(compID).h - constants::PAD * 3) / constants::FONTH;
}

int Field::getCompID()
{
    return compID;
}