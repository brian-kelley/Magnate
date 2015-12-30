//
//  Label.cpp
//  MagIndev
//
//  Created by Brian Kelley on 10/22/14295.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "Label.h"

using namespace std;

Label::Label(int x, int y, int width, int height, u8 stickyFlags, string text, Component* parentComp) : Component(x, y, width, height, stickyFlags, true, parentComp)
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
    float fontScaleX = (local.w - constants::PAD * 2) / (float) constants::FONTW * text.size();
    float fontScaleY = (local.h - constants::PAD * 2) / (float) constants::FONTH;
    fontScale = fontScaleX < fontScaleY ? fontScaleX : fontScaleY;
    textLoc.x = (local.w - (fontScale * constants::FONTW * text.size())) / 2;
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

CompType Label::getType()
{
    return CompType::label;
}