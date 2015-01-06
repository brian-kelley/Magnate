//
//  Label.cpp
//  MagIndev
//
//  Created by Brian Kelley on 10/22/14295.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "Label.h"

using namespace std;
using namespace componentHandler;

Label::Label(int x, int y, int width, int height, string text)
{
	this->compID = createComponent(x, y, width, height, true);
    this->text = text;
    calcTextPlacement();
}

Label::~Label()
{
    //No heap members yet...
}

void Label::updateText(string newText)
{
    this->text = newText;
    calcTextPlacement();
}

void Label::calcTextPlacement()
{
    float fontScaleX = (getCompIntRect(compID).w - constants::PAD * 2) / (float) constants::FONTW;
    float fontScaleY = (getCompIntRect(compID).h - constants::PAD * 2) / (float) constants::FONTH;
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

string Label::getText()
{
    return text;
}

SDL_Point& Label::getTextLoc()
{
    return textLoc;
}

float Label::getFontScale()
{
    return fontScale;
}
