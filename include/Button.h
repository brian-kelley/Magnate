#ifndef __BUTTON_H__
#define __BUTTON_H__

#include <iostream>
#include <string>
#include "Component.h"
#include "SdlHeaders.h"

class Button : public Component
{
public:
    Button(int x, int y, int width, int height, u8 stickyFlags, std::string text, Callback whenPressed, Component* parentComp);
    std::string& getText();
    float getFontScale();
    void processResize();
    Callback getCallback();
    void mouseButton(const SDL_MouseButtonEvent& event);
    CompType getType();
private:
    bool over;
    std::string text;
    Callback whenPressed;
};

#endif