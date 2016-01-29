#include "Field.h"

using namespace std;

Field* Field::currentField = nullptr;

Field::Field(int x, int y, int width, int height, u8 stickyFlags, string text, Callback cb, Component* parentComp) : Component(x, y, width, height, stickyFlags, true, parentComp)
{
    this->text = text;
    whenUpdated = cb;
}

void Field::staticMouseButton(const SDL_MouseButtonEvent &event)
{
    if(currentField && !currentField->isMouseOver())
        currentField->loseFocus();
}

string& Field::getText()
{
    return this->text;
}

void Field::setText(string newtext)
{
    this->text = newtext;
}

void Field::keyEvent(const SDL_KeyboardEvent &event)
{
    if(isActive())
    {
        if(event.state == SDL_PRESSED)
        {
            if(event.keysym.scancode == SDL_SCANCODE_BACKSPACE)
            {
                if(this->text.size() > 1)
                {
                    this->text = this->text.substr(0, (unsigned long) text.length() - 2) + "_";
                }
            }
            else if(event.keysym.scancode == SDL_SCANCODE_RETURN)
            {
                loseFocus();
            }
        }
    }
}

void Field::keyTyped(const SDL_TextInputEvent &event)
{
    if(isActive())
        text = text.substr(0, text.length() - 1) + string(event.text) + "_";
}

Callback Field::getCallback()
{
    return whenUpdated;
}

void Field::gainFocus()
{
    if(currentField)
    {
        currentField->loseFocus();
    }
    text = text.append("_");
    currentField = this;
}

void Field::loseFocus()
{
    if(currentField == this)
    {
        text = text.substr(0, int(text.length() - 1));
        currentField = nullptr;
        if(whenUpdated.func)
            whenUpdated(this);
    }
}

void Field::mouseButton(const SDL_MouseButtonEvent &event)
{
    if(isMouseOver())
    {
        if(!isActive())
        {
            gainFocus();
            currentField = this;
        }
    }
    else
    {
        if(isActive())
            loseFocus();
    }
}

CompType Field::getType()
{
    return CompType::field;
}

bool Field::isActive()
{
    return this == currentField;
}