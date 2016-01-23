#ifndef __MULTISELECT_H__
#define __MULTISELECT_H__

#include <stdio.h>
#include <iostream>
#include <vector>
#include "Component.h"

class MultiSelect : public Component
{
public:
    MultiSelect(int x, int y, int width, int height, u8 stickyFlags, int optHeight, Component* parentComp, bool center = true);
    void addOption(std::string newOption);
    int getNumOptions();
    void removeOption(int which);
    int getOptHeight();
    void mouseButton(const SDL_MouseButtonEvent &event);
    int getSelection();
    std::string& getSelectionText();
    std::vector<std::string>& getOptions();
    void processResize();
    void clearSelection();
    int findSelection(std::string text);
    void setOptions(std::vector<std::string>& opts);
    CompType getType();
private:
    std::vector<std::string> options;
    int optHeight;
    int selection;
    void matchSizeToOptions();
};

#endif