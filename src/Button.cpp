#include "Button.h"

using namespace std;

Button::Button(int x, int y, int width, int height, u8 stickyFlags, string text, Callback whenPressed, Component* parentComp) : Component(x, y, width, height, stickyFlags, true, parentComp)
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

Callback Button::getCallback()
{
    return whenPressed;
}

void Button::processResize()
{
    Component::processResize();
}

void Button::processLeftClick()
{
    if(whenPressed.func && isMouseOver())
        whenPressed(this);
}

CompType Button::getType()
{
    return CompType::button;
}