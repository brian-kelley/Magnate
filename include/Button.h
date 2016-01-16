#ifndef __BUTTON_H__
#define __BUTTON_H__

#include <iostream>
#include <string>
#include "Component.h"
#include "SdlHeaders.h"

class Button : public Component
{
public:
    Button(int x, int y, int width, int height, u8 stickyFlags, std::string text, callback whenPressed, Component* parentComp);
    std::string& getText();
    float getFontScale();
    void processResize();
    callback getCallback();
    void processLeftClick();
    CompType getType();
private:
    bool over;
    std::string text;
    callback whenPressed;
};

#endif