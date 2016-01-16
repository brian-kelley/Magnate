#ifndef __LABEL_H__
#define __LABEL_H__

#include <iostream>
#include <string>
#include "Component.h"
#include "SdlHeaders.h"

class Label : public Component
{
public:
    Label(int x, int y, int width, int height, u8 stickyFlags, std::string text, Component* parentComp);
    void updateText(std::string text);
    std::string& getText();
    void updateSize();
    CompType getType();
private:
    std::string text;
};

#endif
