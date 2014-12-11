//
//  Field.cpp
//  MagIndev
//
//  Created by Brian Kelley on 10/22/14295.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "Field.h"

using namespace std;

Field::Field(int x, int y, int width, int height, string text, void(*callback) ())
{
    x -= width / 2;
    y -= height / 2;
    this->rect.x = x;
    this->rect.y = y;
    this->rect.w = width;
    this->rect.h = height;
    this->fontScale = (float) (width - constants::PAD * 3) / constants::FONTH;
    this->text = text;
    this->callback = callback;
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
    cout << getText() << endl;
}

float Field::getFontScale()
{
    return this->fontScale;
}

SDL_Rect& Field::getRect()
{
    return this->rect;
}

SDL_Point& Field::getTextLoc()
{
    return this->textLoc;
}