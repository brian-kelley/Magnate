//
//  Button.cpp
//  MagIndev
//
//  Created by Brian Kelley on 10/22/14295.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "Button.h"

using namespace std;

Button::Button(int x, int y, int width, int height, u8 stickyFlags, string text, callback whenPressed, Component* parentComp) : Component(x, y, width, height, stickyFlags, true, parentComp)
{
    this->whenPressed = whenPressed;
    this->text = text;
    this->over = false;
    over = false;
}

string& Button::getText()
{
    return text;
}

callback Button::getCallback()
{
    return whenPressed;
}

void Button::processResize()
{
    Component::processResize();
}

void Button::processLeftClick()
{
    if(whenPressed && isMouseOver())
        whenPressed(this);
}

CompType Button::getType()
{
    return CompType::button;
}