//
//  MultiSelect.cpp
//  Magnate
//
//  Created by Brian Kelley on 2/22/1553.
//
//

#include "MultiSelect.h"

using namespace std;
using namespace constants;

MultiSelect::MultiSelect(int x, int y, int width, int height, int optHeight, Component* parentComp)
: Component(x, y, width, height, true, parentComp, MULTISELECT)
{
    selection = -1;
    this->optHeight = optHeight;
    fOptHeight = float(optHeight) / height;
    fontScale = (float(optHeight) - 2 * PAD) / FONTH;
}

void MultiSelect::processResize()
{
    //Super method does most of the work i think
    Component::processResize();
    optHeight = fOptHeight * localRect.h;
    fontScale = (float(optHeight) - 2 * PAD) / FONTH;
}

void MultiSelect::addOption(std::string newOption)
{
    options.push_back(newOption);
}

int MultiSelect::getNumOptions()
{
    return int(options.size());
}

void MultiSelect::removeOption(int which)
{
    options.erase(options.begin() + which);
}

int MultiSelect::getOptHeight()
{
    return optHeight;
}

void MultiSelect::processLeftClick()
{
    selection = (mouseY - yOffset) / optHeight;
    if(selection >= options.size())
    {
        selection = -1;
    }
}

int MultiSelect::getSelection()
{
    return selection;
}

string MultiSelect::getSelectionText()
{
    if(selection == -1)
    {
        return "";
    }
    else
    {
        return options[selection];
    }
}

vector<string>& MultiSelect::getOptions()
{
    return options;
}

void MultiSelect::clearSelection()
{
    selection = -1;
}

float MultiSelect::getFontScale()
{
    return fontScale;
}