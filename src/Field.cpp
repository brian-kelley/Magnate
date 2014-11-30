//
//  Field.cpp
//  MagIndev
//
//  Created by Brian Kelley on 10/22/14295.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "Field.h"

using namespace std;

Field::Field(int x, int y, int width, int height, bool centered, int font)
{
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->centered = centered;
    this->font = font;
}

Field::~Field() {}

string Field::getText()
{
    string s(this->text.begin(), this->text.end());
    return s;
}

void Field::setText(string newtext)
{
    this->text.clear();
    vector<char> newvec(newtext.begin(), newtext.end());
    this->text = newvec;
}

void Field::setLocation(int x, int y)
{
    this->x = x;
    this->y = y;
}

void Field::setSize(int width, int height)
{
    this->width = width;
    this->height = height;
}

void Field::processKey(SDL_Event *ev)
{
    if(ev->type == SDL_TEXTINPUT)
    {
        this->text.push_back(ev->text.text[0]);
    }
    else if(ev->type == SDL_KEYDOWN)
    {
        if(ev->key.keysym.scancode == SDL_SCANCODE_BACKSPACE)
        {
            if(this->text.size() > 0)
            {
                this->text.pop_back();
            }
        }
    }
    cout << getText() << endl;
}