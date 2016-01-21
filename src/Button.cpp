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

void Button::mouseButton(const SDL_MouseButtonEvent& event)
{
    if(whenPressed.func && isMouseOver() && event.state == SDL_PRESSED)
        whenPressed(this);
}

CompType Button::getType()
{
    return CompType::button;
}