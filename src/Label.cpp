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
}

void Label::updateText(string newText)
{
    this->text = newText;
}

string& Label::getText()
{
    return text;
}

CompType Label::getType()
{
    return CompType::label;
}
