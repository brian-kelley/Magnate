//
//  MultiSelect.h
//  Magnate
//
//  Created by Brian Kelley on 2/22/1553.
//
//

#ifndef __Magnate__MultiSelect__
#define __Magnate__MultiSelect__

#include <stdio.h>
#include <iostream>
#include <vector>
#include "Constants.h"
#include "Component.h"

class MultiSelect : public Component
{
public:
    MultiSelect(int x, int y, int width, int height, int optHeight, Component* parentComp);
    void addOption(std::string newOption);
    int getNumOptions();
    void removeOption(int which);
    int getOptHeight();
    void processLeftClick();
    int getSelection();
    std::string getSelectionText();
    std::vector<std::string>& getOptions();
    void processResize();
    void clearSelection();
    float getFontScale();
private:
    std::vector<std::string> options;
    int optHeight;
    float fOptHeight;
    int selection;
    float fontScale;
};

#endif /* defined(__Magnate__MultiSelect__) */