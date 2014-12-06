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
    this->rect.w = width;
    this->rect.h = height;
    this->calcTextRect();
}

Label::~Label()
{
    //No heap members yet...
}

void Label::updateText(string newText)
{
    this->text = newText;
    this->rect.w = this->fontScale * newText.size() * constants::FONTW;
}

void Label::processSizeChange(int oldWindowW, int oldWindowH)
{
    if(oldWindowW != constants::WINDOW_W)
    {
        float xscl = (float) constants::WINDOW_W / oldWindowW;
        this->rect.x *= xscl;
        this->rect.w *= xscl;
    }
    if(oldWindowH != constants::WINDOW_H)
    {
        float yscl = (float) constants::WINDOW_H / oldWindowH;
        this->rect.y *= yscl;
        this->rect.w *= yscl;
    }
    calcTextRect();
}

void Label::calcTextRect()
{
    float fontScaleX = (rect.w - constants::PAD * 2) / (float) constants::FONTW;
    float fontScaleY = (rect.h - constants::PAD * 2) / (float) constants::FONTH;
    if(fontScaleX < fontScaleY)
    {
        this->fontScale = fontScaleX;
        this->textLoc.x = constants::PAD;
        this->textLoc.y = constants::PAD + 0.5 * constants::FONTH * (fontScaleY - fontScaleX);
    }
    else
    {
        this->fontScale = fontScaleY;
        this->textLoc.x = constants::PAD + 0.5 * constants::FONTH * (int) text.length() * (fontScaleX - fontScaleY);
        this->textLoc.y = constants::PAD;
    }
}