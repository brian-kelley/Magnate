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
    Field(int x, int y, int width, int height, u8 stickyFlags, std::string text, callback cb, Component* parentComp);
    void setText(std::string text);
    void setLocation(int x, int y);
    void setSize(int width, int height);
    void processKey(SDL_Event& e);
    float getFontScale();
    std::string& getText();
    void updateSize();
    callback getCallback();
    bool isActive();
    void activate();
    void deactivate();
    void processLeftClick();
    CompType getType();
private:
    std::string text;
    float fontScale;
    callback whenUpdated;
    bool active;
    bool hasCursor;
    SDL_Point textLoc;
};

#endif
