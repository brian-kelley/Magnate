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
#include <boost/filesystem.hpp>
#include "Control.h"
#include "DirManager.h"

using namespace std;
using namespace boost::filesystem;

int main(int argc, const char* argv[])
{
    for(int i = 0; i < argc; i++)
    {
        cout << "Here comes argv #" << i << ": " << argv[i] << endl;
    }
    DirManager::findPath();
    Control::init();
    int ticks;
    while(true)
    {
        ticks = SDL_GetTicks();
        Control::update();
        if(Control::isTerminating())
        {
            break;
        }
        int delay = 16 - (SDL_GetTicks() - ticks);
        if(delay > 0)
        {
            SDL_Delay(delay);
        }
    }
    return 0;
}