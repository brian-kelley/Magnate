#ifndef __FIELD_H__
#define __FIELD_H__

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include "Component.h"
#include "SdlHeaders.h"

class Field : public Component
{
public:
    //The field object that currently has kbd focus, if any
    static Field* currentField;
    Field(int x, int y, int width, int height, u8 stickyFlags, std::string text, Callback cb, Component* parentComp);
    static void staticMouseButton(const SDL_MouseButtonEvent& event);
    void mouseButton(const SDL_MouseButtonEvent &event);
    void keyTyped(const SDL_TextInputEvent& event);
    void keyEvent(const SDL_KeyboardEvent& event);
    void setText(std::string text);
    void setLocation(int x, int y);
    void setSize(int width, int height);
    void processKey(SDL_Event& e);
    float getFontScale();
    std::string& getText();
    void updateSize();
    Callback getCallback();
    CompType getType();
    std::string text;
    void loseFocus();
    void gainFocus();
    bool isActive();
private:
    Callback whenUpdated;
};

#endif
