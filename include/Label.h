//
//  Label.h
//  MagIndev
//
//  Created by Brian Kelley on 10/22/14295.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#ifndef __MagIndev__Label__
#define __MagIndev__Label__

#include <stdio.h>
#include <iostream>
#include <string>
#include "Component.h"
#include "Constants.h"
#include "SdlHeaders.h"

class Label : public Component
{
public:
    Label(int x, int y, int width, int height, u8 stickyFlags, std::string text, Component* parentComp);
    void updateText(std::string text);
    void processSizeChange(int oldWindowW, int oldWindowH);
    std::string getText();
    float getFontScale();
    SDL_Point& getTextLoc();
    void updateSize();
    void calcTextPlacement();
    CompType getType();
private:
    std::string text;
    float fontScale;
    SDL_Point textLoc;
};

#endif
