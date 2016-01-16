#include "MultiSelect.h"

using namespace std;

MultiSelect::MultiSelect(int x, int y, int width, int height, u8 stickyFlags, int optHeight, Component* parentComp, bool center)
: Component(x, y, width, height, stickyFlags, center, parentComp)
{
    selection = -1;
    this->optHeight = optHeight;
}

void MultiSelect::processResize()
{
    Component::processResize();
    //increase opt height to fill new space
    optHeight = 0.5 + float(local.h) / options.size();
}

void MultiSelect::addOption(std::string newOption)
{
    options.push_back(newOption);
    matchSizeToOptions();
}

int MultiSelect::getNumOptions()
{
    return int(options.size());
}

void MultiSelect::removeOption(int which)
{
    options.erase(options.begin() + which);
    matchSizeToOptions();
}

int MultiSelect::getOptHeight()
{
    return optHeight;
}

void MultiSelect::processLeftClick()
{
    if(isMouseOver())
    {
        selection = (Input::mouseY - screen.y) / optHeight;
        if(selection >= options.size() || selection < 0)
        {
            selection = -1;
        }
    }
}

int MultiSelect::getSelection()
{
    return selection;
}

string& MultiSelect::getSelectionText()
{
    return options[selection];
}

vector<string>& MultiSelect::getOptions()
{
    return options;
}

void MultiSelect::clearSelection()
{
    selection = -1;
}

void MultiSelect::matchSizeToOptions()
{
    //Set height based on number of options and height of each
    local.h = optHeight * options.size();
    updateScreenRect();
}

int MultiSelect::findSelection(std::string text)
{
    for(int index = 0; index < int(options.size()); index++)
    {
        if(options[index] == text)
        {
            return index;
        }
    }
    return -1;
}

CompType MultiSelect::getType()
{
    return CompType::multiSelect;
}