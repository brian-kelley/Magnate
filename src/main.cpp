//
//  main.cpp
//  MagIndev
//
//  Created by Brian Kelley on 10/20/14293.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include "Control.h"
#include "DirManager.h"

using namespace std;

int main(int argc, const char * argv[])
{
    DirManager::findPath();
    Control* c = new Control();
    int ticks;
    while(true)
    {
        ticks = SDL_GetTicks();
        c->update();
        if(c->isTerminating())
        {
            break;
        }
        int delay = 16 - (SDL_GetTicks() - ticks);
        if(delay > 0)
        {
            SDL_Delay(delay);
        }
    }
    delete c;
    return 0;
}