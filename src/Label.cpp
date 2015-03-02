//
//  Label.cpp
//  MagIndev
//
//  Created by Brian Kelley on 10/22/14295.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "Label.h"

using namespace std;

Label::Label(int x, int y, int width, int height, string text, Component* parentComp) : Component(x, y, width, height, true, parentComp, CTYPE::LABEL)
{
    this->text = text;
    calcTextPlacement();
}

void Label::updateText(string newText)
{
    this->text = newText;
    calcTextPlacement();
}

void Label::calcTextPlacement()
{
    float fontScaleX = (localRect.w - constants::PAD * 2) / (float) constants::FONTW * text.size();
    float fontScaleY = (localRect.h - constants::PAD * 2) / (float) constants::FONTH;
    fontScale = fontScaleX < fontScaleY ? fontScaleX : fontScaleY;
    textLoc.x = (localRect.w - (fontScale * constants::FONTW * text.size())) / 2;
    textLoc.y = constants::PAD;
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