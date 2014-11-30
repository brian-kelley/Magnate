//
//  Label.cpp
//  MagIndev
//
//  Created by Brian Kelley on 10/22/14295.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "Label.h"

using namespace std;

Label::Label(int x, int y, int width, int height, string text)
{
    this->rect.x = x;
    this->rect.y = y;
    this->text = text;
    this->rect.w = constants::FONTW * (int) text.size() + constants::PAD * 2;
    this->rect.h = constants::FONTH * (int) text.size() + constants::PAD * 2;
    this->fontScale = 0.2;
}

Label::Label(int x, int y, string text, float fontSize)
{
    this->text = text;
    this->rect.x = x;
    this->rect.y = y;
    this->rect.w = (int) text.size() * constants::FONTW + constants::PAD * 2;
    this->rect.h = constants::FONTH + constants::PAD * 2;
    this->fontScale = fontSize;
}

Label::~Label()
{
    
}

void Label::updateText(string newText)
{
    this->text = newText;
    this->rect.w = this->fontScale * newText.size() * constants::FONTW;
}

void Label::updateLocation(int x, int y)
{
    this->rect.x = x;
    this->rect.y = y;
}

void Label::updateFontSize(float newScale)
{
    this->fontScale = newScale;
}